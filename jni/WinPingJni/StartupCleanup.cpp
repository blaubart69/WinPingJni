#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>
#include <IcmpAPI.h>

#include <jni.h>

HANDLE g_hIcmpFile;

JNIEXPORT jint JNICALL Java_at_spindi_WinPing_native_1WinPing_1Startup(JNIEnv *, jclass) {

	g_hIcmpFile = IcmpCreateFile();

	if (g_hIcmpFile == INVALID_HANDLE_VALUE) {
		g_hIcmpFile = NULL;
		return GetLastError();
	}
	return 0;
}

JNIEXPORT jint JNICALL Java_at_spindi_WinPing_native_1WinPing_1Cleanup(JNIEnv *, jclass) {
	//
	// free up resources
	//
	jint rc = 0;
	if (!IcmpCloseHandle(g_hIcmpFile)) {
		rc = GetLastError();
	}
	return rc;
}
