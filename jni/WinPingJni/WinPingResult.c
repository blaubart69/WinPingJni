#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>

#include <jni.h>

jobject newWinPingResult(JNIEnv* env, jint lastError, jlong status, jint roundtrip) {
	//
	// create return object
	//            
	jclass	   WinPingResultClazz = (*env)->FindClass  (env, "at/spindi/WinPingResult");
	jmethodID  WinPingResultCtor  = (*env)->GetMethodID(env, WinPingResultClazz, "<init>", "(IJI)V");
	jobject    WinPingResultObj   = (*env)->NewObject  (env, WinPingResultClazz, WinPingResultCtor, lastError, status, roundtrip);

	return WinPingResultObj;
}