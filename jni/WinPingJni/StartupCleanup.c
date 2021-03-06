#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>

#include <IPHlpApi.h>
#include <IcmpAPI.h>

#include <jni.h>

#include "WinPingJni.h"

WIN_PING_GLOBAL* gWinPing;
//DWORD WINAPI ThreadProc(LPVOID lpThreadParameter);

// -----------------------------------------------------------------------------
DWORD WINAPI ThreadProc(LPVOID lpThreadParameter) {
// -----------------------------------------------------------------------------

	JavaVMAttachArgs args;
	args.version = JNI_VERSION_1_6; // choose your JNI version
	args.name = "WinPingApcPing"; // you might want to give the java thread a name
	args.group = NULL; // you might want to assign the java thread to a ThreadGroup

	JNIEnv *env = (JNIEnv*)lpThreadParameter;

	jint jrc;

	JavaVM* vm;
	jrc = (*env)->GetJavaVM(env, &vm);
	if (jrc != 0)
	{
		logError(L"ApcThread", L"GetJavaVM", jrc);
	}

	jrc = (*vm)->AttachCurrentThread(
							vm,
							(void**)(&(gWinPing->ApcJniEnv)),
							&args);

	if (jrc != JNI_OK)
	{
		logError(L"ApcThread", L"AttachCurrentThread", jrc);
		return jrc;
	}

	// signal that we have attached the thread to the JVM 
	// and initialized the gWinPing->ApcJniEnv for the call on this thread
	
	if (!SetEvent(gWinPing->ApcThreadInitFinished))
	{
		jrc = logLastWin32Error(L"ApcThread", L"SetEvent", L"ApcThreadInitFinished");
	}
	else
	{
		DWORD rc;
		while ((rc=WaitForSingleObjectEx(gWinPing->shutdownEvent, INFINITE, TRUE)) == WAIT_IO_COMPLETION)
			;

		if (rc == WAIT_OBJECT_0)
		{
			// shutdown was signaled. everything ok.
		}
		else if (rc == WAIT_FAILED)
		{
			jrc = logLastWin32Error(L"ApcThread", L"WaitForSingleObjectEx(WAIT_FAILED)", L"shutdownEvent");
		}
		else 
		{
			jrc = rc;
			logError(L"ApcThread", L"WaitForSingleObjectEx", rc);
		}

	}

	jrc = (*vm)->DetachCurrentThread(vm);
	if (jrc != JNI_OK)
	{
		logError(L"ApcThread", L"DetachCurrentThread", jrc);
	}

	return jrc;
}

// -----------------------------------------------------------------------------
JNIEXPORT jint JNICALL Java_at_spindi_WinPing_native_1WinPing_1Startup(JNIEnv *env, jclass clazz) {
// -----------------------------------------------------------------------------

	DWORD rc = 0;

	gWinPing = (WIN_PING_GLOBAL*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WIN_PING_GLOBAL));
	if (gWinPing == NULL) {
		rc = ERROR_OUTOFMEMORY;
		logError(L"Startup", L"HeapAlloc", rc);
		goto fail;
	}

	if ((gWinPing->hIcmpFile = IcmpCreateFile()) == INVALID_HANDLE_VALUE) {
		rc = logLastWin32Error(L"Startup", L"IcmpCreateFile", L"invalid handle");
		goto fail;
	}

	if ( (gWinPing->hIcmp6File = Icmp6CreateFile()) == INVALID_HANDLE_VALUE) {
		rc = logLastWin32Error(L"Startup", L"Icmp6CreateFile", L"invalid handle");
		goto fail;
	}

	if ((gWinPing->shutdownEvent = CreateEvent(NULL, TRUE, FALSE, L"WinPingApcEnd")) == NULL)
	{
		rc = logLastWin32Error(L"Startup", L"CreateEvent", L"shutdownEvent");
		goto fail;
	}

	if ((gWinPing->ApcThreadInitFinished = CreateEvent(NULL, TRUE, FALSE, L"WinPingApcInit")) == NULL)
	{
		rc = logLastWin32Error(L"Startup", L"CreateEvent", L"ApcThreadInitFinished");
		goto fail;
	}

	if ((gWinPing->hTread = CreateThread(NULL, 1, (LPTHREAD_START_ROUTINE)ThreadProc, (LPVOID)env, 0, NULL)) == NULL)
	{
		rc = logLastWin32Error(L"Startup", L"CreateThread", L"ThreadProc");
		goto fail;
	}

	if ((rc=WaitForSingleObject(gWinPing->ApcThreadInitFinished, 3000)) != WAIT_OBJECT_0)
	{
		logError(L"Startup", L"ApcThread did not initialize within 3s.", rc);
		goto fail;
	}
	else {
		CloseHandle(gWinPing->ApcThreadInitFinished);
		gWinPing->ApcThreadInitFinished = NULL;
	}

	goto ok;

fail:

	if (gWinPing != NULL)
	{
		if (gWinPing->hIcmpFile  != NULL)				IcmpCloseHandle(gWinPing->hIcmpFile);
		if (gWinPing->hIcmp6File != NULL)				IcmpCloseHandle(gWinPing->hIcmp6File);
		if (gWinPing->shutdownEvent != NULL)			CloseHandle(gWinPing->shutdownEvent);
		if (gWinPing->ApcThreadInitFinished != NULL)	CloseHandle(gWinPing->ApcThreadInitFinished);
		HeapFree(GetProcessHeap(), 0, gWinPing);
	}

ok:

	return rc;
}

// -----------------------------------------------------------------------------
JNIEXPORT jint JNICALL Java_at_spindi_WinPing_native_1WinPing_1Shutdown(JNIEnv *env, jclass clazz) {
// -----------------------------------------------------------------------------
	DWORD rc = 0;

	if (!SetEvent(gWinPing->shutdownEvent)) {
		rc = logLastWin32Error(L"Shutdown", L"SetEvent", L"shutdownEvent");
	}

	IcmpCloseHandle(gWinPing->hIcmpFile);
	IcmpCloseHandle(gWinPing->hIcmp6File);

	if ((rc=WaitForSingleObject(gWinPing->hTread, 1000)) != WAIT_OBJECT_0)
	{
		logError(L"Shutdown", L"WaitForSingleObject(gWinPing->hTread)", rc);
	}

	CloseHandle(gWinPing->shutdownEvent);
	CloseHandle(gWinPing->hTread);

	HeapFree(GetProcessHeap(), 0, gWinPing);

	return rc;
}
// -----------------------------------------------------------------------------
BOOLEAN WINAPI
DllMain(IN HINSTANCE hDllHandle, IN DWORD nReason, IN LPVOID Reserved)
// -----------------------------------------------------------------------------
{
	BOOLEAN bSuccess = TRUE;
	return bSuccess;
}
// -----------------------------------------------------------------------------
void logError(LPCWSTR method, LPCWSTR text, DWORD rc)
// -----------------------------------------------------------------------------
{
	WCHAR msg[256];
	wsprintfW(msg, L"WinPing.%s: rc(%ld) %s", method, rc, text);
	OutputDebugString(msg);
}
// -----------------------------------------------------------------------------
DWORD logLastWin32Error(LPCWSTR method, LPCWSTR win32Api, LPCWSTR text)
// -----------------------------------------------------------------------------
{
	WCHAR msg[256];
	DWORD lastrc = GetLastError();

	wsprintfW(msg, L"WinPing(%s) LastError(%ld) Win32API(%s) %s", method, lastrc, win32Api, text);
	OutputDebugString(msg);

	return lastrc;
}
