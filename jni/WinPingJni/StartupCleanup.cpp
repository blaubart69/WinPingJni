#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>

#include <IPHlpApi.h>
#include <IcmpAPI.h>

#include <jni.h>

#include "WinPingJni.h"

WIN_PING_GLOBAL* gWinPing;

JNIEXPORT jint JNICALL Java_at_spindi_WinPing_native_1WinPing_1Startup(JNIEnv *, jclass) {

	gWinPing = (WIN_PING_GLOBAL*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WIN_PING_GLOBAL));
	if (gWinPing == NULL) {
		// TODO: maybe a better retcode here
		return 1;
	}

	gWinPing->asyncCounter = -1L;

	gWinPing->hIcmpFile = IcmpCreateFile();
	if (gWinPing->hIcmpFile == INVALID_HANDLE_VALUE) {
		gWinPing->hIcmpFile = NULL;
		return GetLastError();
	}
	return 0;
}

JNIEXPORT jint JNICALL Java_at_spindi_WinPing_native_1WinPing_1Cleanup(JNIEnv *, jclass) {
	//
	// free up resources
	//
	jint rc = 0;
	if (!IcmpCloseHandle(gWinPing->hIcmpFile)) {
		rc = GetLastError();
	}
	return rc;
}
