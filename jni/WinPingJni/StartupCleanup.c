#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>

#include <IPHlpApi.h>
#include <IcmpAPI.h>

#include <jni.h>

#include "WinPingJni.h"

WIN_PING_GLOBAL* gWinPing;

JNIEXPORT jint JNICALL Java_at_spindi_WinPing_native_1WinPing_1Startup(JNIEnv *env, jclass clazz) {

	gWinPing = (WIN_PING_GLOBAL*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WIN_PING_GLOBAL));
	if (gWinPing == NULL) {
		// TODO: maybe a better retcode here
		return 1;
	}

	gWinPing->hIcmpFile = IcmpCreateFile();
	if (gWinPing->hIcmpFile == INVALID_HANDLE_VALUE) {
		return GetLastError();
	}

	(*env)->GetJavaVM(env, &(gWinPing->vm));

	// async
	gWinPing->async._enqueuedPings	= 0;
	gWinPing->async._sentPings		= 0;
	InitializeCriticalSection(&gWinPing->async._criticalEnqueue);

	return 0;
}

JNIEXPORT jint JNICALL Java_at_spindi_WinPing_native_1WinPing_1Cleanup(JNIEnv *env, jclass clazz) {

	jint rc = 0;

	if (gWinPing->async._enqueuedPings == 0) {
		rc = FreePingResouces();
	}
	else {
		rc = 4;  
	}

	return rc;
}

DWORD FreePingResouces() {
	DeleteCriticalSection(&gWinPing->async._criticalEnqueue);

	DWORD rc = 0;
	if (!IcmpCloseHandle(gWinPing->hIcmpFile)) {
		rc = GetLastError();
	}

	HeapFree(GetProcessHeap(), 0, gWinPing);

	return rc;
}
