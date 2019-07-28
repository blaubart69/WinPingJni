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
		WCHAR msg[128];
		wsprintfW(msg, L"WinPingJni-ApcThread: GetJavaVM() returned: %ld", jrc);
		OutputDebugStringW(msg);
	}

	jrc = (*vm)->AttachCurrentThread(
							vm,
							(void**)(&(gWinPing->ApcJniEnv)),
							&args);

	if (jrc != JNI_OK)
	{
		WCHAR msg[128];
		wsprintfW(msg, L"WinPingJni-ApcThread: AttachCurrentThread() returned: %ld", jrc);
		OutputDebugStringW(msg);
	}

	// signal that we have attached the thread to the JVM 
	// and initialized the gWinPing->ApcJniEnv for the call on this thread
	
	if (!SetEvent(gWinPing->ApcThreadInitFinished))
	{
		WCHAR msg[128]; 
		wsprintfW(msg, L"WinPingJni-ApcThread: LastError: %d. could not set event WinPingApcInit", GetLastError());
		OutputDebugStringW(msg);
		(*vm)->DetachCurrentThread(vm);
		
		return 99;
	}

	while (WaitForSingleObjectEx(gWinPing->shutdownEvent, INFINITE, TRUE) == WAIT_IO_COMPLETION)
		;

	jrc = (*vm)->DetachCurrentThread(vm);
	if (jrc != JNI_OK)
	{
		WCHAR msg[128];
		wsprintfW(msg, L"WinPingJni-ApcThread: DetachCurrentThread() returned: %ld", jrc);
		OutputDebugStringW(msg);
	}

	return 0;
}

// -----------------------------------------------------------------------------
JNIEXPORT jint JNICALL Java_at_spindi_WinPing_native_1WinPing_1Startup(JNIEnv *env, jclass clazz) {
// -----------------------------------------------------------------------------

	DWORD rc = 0;

	gWinPing = (WIN_PING_GLOBAL*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WIN_PING_GLOBAL));
	if (gWinPing == NULL) {
		rc = ERROR_OUTOFMEMORY;
		goto fail;
	}

	if ((gWinPing->hIcmpFile = IcmpCreateFile()) == INVALID_HANDLE_VALUE) {
		rc = GetLastError();
		goto fail;
	}

	if ( (gWinPing->hIcmp6File = Icmp6CreateFile()) == INVALID_HANDLE_VALUE) {
		rc = GetLastError();
		goto fail;
	}

	if ((gWinPing->shutdownEvent = CreateEvent(NULL, TRUE, FALSE, L"WinPingApcEnd")) == NULL)
	{
		rc = GetLastError();
		goto fail;
	}

	if ((gWinPing->ApcThreadInitFinished = CreateEvent(NULL, TRUE, FALSE, L"WinPingApcInit")) == NULL)
	{
		rc = GetLastError();
		goto fail;
	}

	if ((gWinPing->hTread = CreateThread(NULL, 1, (LPTHREAD_START_ROUTINE)ThreadProc, env, 0, NULL)) == NULL)
	{
		rc = GetLastError();
		goto fail;
	}

	if (WaitForSingleObject(gWinPing->ApcThreadInitFinished, 3000) != WAIT_OBJECT_0)
	{
		OutputDebugString(L"WinPing: ApcThread did not initialize within 3s.");
		rc = 99;
		goto fail;
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
JNIEXPORT jint JNICALL Java_at_spindi_WinPing_native_1WinPing_1Cleanup(JNIEnv *env, jclass clazz) {
// -----------------------------------------------------------------------------
	DWORD rc = 0;

	if (!SetEvent(gWinPing->shutdownEvent)) {
		rc = GetLastError();
	}

	IcmpCloseHandle(gWinPing->hIcmpFile);
	IcmpCloseHandle(gWinPing->hIcmp6File);
	CloseHandle(gWinPing->shutdownEvent);
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
