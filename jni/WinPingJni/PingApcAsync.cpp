#include "at_spindi_WinPing.h"
#include "WinPingJni.h"

/*
* Class:     at_spindi_WinPing
* Method:    native_icmp_WinPing4Async
* Signature: (IILjava/util/function/Consumer;)I
*/
JNIEXPORT jint JNICALL Java_at_spindi_WinPing_native_1icmp_1WinPing4Async
(JNIEnv *env, jclass clazz, jint bigEndianv4Address, jint timeoutMs, jobject ConsumerCallback) {
	//
	// Do something! Do something!
	//
	int myLastErr = 1;
	int myIpStatus = 2;
	int myRoundTrip = 123;
	//
	// create return object
	//
	const jclass	WinPingResultClazz	= env->FindClass("at/spindi/WinPingResult");
	const jmethodID WinPingResultCtor	= env->GetMethodID(WinPingResultClazz, "<init>", "(III)V");
	const jobject   WinPingResultObj	= env->NewObject(WinPingResultClazz, WinPingResultCtor, myLastErr, myIpStatus, myRoundTrip);
	//
	// calling the "consumer" callback
	//
	//const jclass	consumerClazz	= env->FindClass("java/util/function/Consumer");
	const jclass callBackClass = env->GetObjectClass(ConsumerCallback);
	const jmethodID	acceptMethod	= env->GetMethodID(callBackClass, "accept", "(Ljava/lang/Object;)V");
	env->CallVoidMethod(ConsumerCallback, acceptMethod, WinPingResultObj);

	return 0;
}