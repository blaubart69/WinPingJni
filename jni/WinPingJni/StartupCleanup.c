#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>

#include <IPHlpApi.h>
#include <IcmpAPI.h>

#include <jni.h>

#include "WinPingJni.h"

WIN_PING_GLOBAL* gWinPing;
DWORD WINAPI ThreadProc(LPVOID lpThreadParameter);



JNIEXPORT jint JNICALL Java_at_spindi_WinPing_native_1WinPing_1Startup(JNIEnv *env, jclass clazz) {

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

	(*env)->GetJavaVM(env, &(gWinPing->vm));

	PING_ASYNC* pAsync = &(gWinPing->async);

	pAsync->_hTread = CreateThread(NULL, 1, (LPTHREAD_START_ROUTINE)ThreadProc, NULL, 0, NULL);
	if (pAsync->_hTread == NULL) {
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

JNIEXPORT jint JNICALL Java_at_spindi_WinPing_native_1WinPing_1Cleanup(JNIEnv *env, jclass clazz) {

	DWORD rc = 0;

	if (!SetEvent(gWinPing->shutdownEvent)) {
		rc = GetLastError();
	}

	return rc;
}

BOOLEAN WINAPI
DllMain(IN HINSTANCE hDllHandle, IN DWORD nReason, IN LPVOID Reserved)
{
	BOOLEAN bSuccess = TRUE;
	return bSuccess;
}
