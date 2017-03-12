#include "at_spindi_WinPing.h"
#include "WinPingJni.h"
#include "ApcWorkerCpp.h"

// global structure initialized via "startup" call
extern WIN_PING_GLOBAL* gWinPing;


void jniCallback(IPAddr ip, DWORD roundtrip, int pingStatus, int LastError, void* ctx);

/*
* Class:     at_spindi_WinPing
* Method:    native_icmp_WinPing4Async
* Signature: (IILjava/util/function/Consumer;)I
*/
JNIEXPORT jint JNICALL Java_at_spindi_WinPing_native_1icmp_1WinPing4Async
(JNIEnv *env, jclass clazz, jint bigEndianv4Address, jint timeoutMs, jobject Consumer) {
	//
	// Do something! Do something! (C) by Schurl
	//
	MY_PING_CTX *pingCtx = (MY_PING_CTX*)HeapAlloc(GetProcessHeap(), 0, sizeof(MY_PING_CTX));
	pingCtx->jniCtx.Consumer = Consumer;
	pingCtx->jniCtx.env = env;
	pingCtx->ip = bigEndianv4Address;
	pingCtx->timeoutMs = timeoutMs;

	const DWORD enqueueRc = gWinPing->apcWorker->enqueue(pingCtx);
	if (enqueueRc != 0) {
		HeapFree(GetProcessHeap(), 0, pingCtx);
	}

	return enqueueRc;
}

void jniCallback(IPAddr ip, DWORD roundtrip, int pingStatus, int LastError, void* ctx) {
	MY_JNI_CONTEXT *jnictx = (MY_JNI_CONTEXT*)ctx;
	
	JNIEnv *env = jnictx->env;
	jobject	Consumer = jnictx->Consumer;

	HeapFree(GetProcessHeap(), 0, ctx);

	JNIEnv* myNewEnv;
	JavaVMAttachArgs args;
	args.version = JNI_VERSION_1_6; // choose your JNI version
	args.name = NULL; // you might want to give the java thread a name
	args.group = NULL; // you might want to assign the java thread to a ThreadGroup
	gWinPing->vm->AttachCurrentThread((void**)(&myNewEnv), &args);


	//
	// create return object
	//            
	const jclass	WinPingResultClazz = env->FindClass("at/spindi/WinPingResult");
	const jmethodID WinPingResultCtor = env->GetMethodID(WinPingResultClazz, "<init>", "(III)V");
	const jobject   WinPingResultObj = env->NewObject(WinPingResultClazz, WinPingResultCtor, LastError, pingStatus, roundtrip);
	//
	// calling the "consumer" callback
	//
	const jclass	consumerClass = env->GetObjectClass(Consumer);
	const jmethodID	acceptMethod = env->GetMethodID(consumerClass, "accept", "(Ljava/lang/Object;)V");
	env->CallVoidMethod(Consumer, acceptMethod, WinPingResultObj);

}






