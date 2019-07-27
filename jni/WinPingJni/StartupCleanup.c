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

	JavaVM* vm;
	(*env)->GetJavaVM(env, &vm);

	const jint attachRc =
		(*vm)->AttachCurrentThread(
							vm,
							(void**)(&(gWinPing->ApcJniEnv)),
							&args);

	while (WaitForSingleObjectEx(gWinPing->shutdownEvent, INFINITE, TRUE) == WAIT_IO_COMPLETION)
		;

	const jint detachRc = (*vm)->DetachCurrentThread(vm);


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

	gWinPing->hIcmpFile = IcmpCreateFile();
	if (gWinPing->hIcmpFile == INVALID_HANDLE_VALUE) {
		goto fail;
	}

	if ( (gWinPing->hIcmp6File = Icmp6CreateFile()) == INVALID_HANDLE_VALUE) {
		goto fail;
	}

	gWinPing->hTread = CreateThread(NULL, 1, (LPTHREAD_START_ROUTINE)ThreadProc, env, 0, NULL);
	if (gWinPing->hTread == NULL) {
		goto fail;
	}

	/*
	if (QueueUserAPC(ApcAttachThreadToJavaVM, pAsync->_hTread, (ULONG_PTR)NULL) == 0) {
		goto fail;
	}
	*/

	gWinPing->shutdownEvent = CreateEvent(NULL, TRUE, FALSE, L"WinPingApcEndEvent");
	if (gWinPing->shutdownEvent == NULL)
	{
		goto fail;
	}

	goto ok;

fail:
	if (rc == 0) {
		rc = GetLastError();
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
