#include "at_spindi_WinPing.h"
#include "WinPingJni.h"

// global structure initialized via "startup" call
extern WIN_PING_GLOBAL* gWinPing;

typedef struct {
	JNIEnv		*env;
	jobject		Consumer;
} MY_JNI_CONTEXT;

void jniCallback(IPAddr ip, DWORD roundtrip, int pingStatus, int LastError, void* ctx) {
	MY_JNI_CONTEXT *jnictx = (MY_JNI_CONTEXT*)ctx;
	
	JNIEnv *env = jnictx->env;
	jobject	Consumer = jnictx->Consumer;

	HeapFree(GetProcessHeap(), 0, ctx);

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

/*
* Class:     at_spindi_WinPing
* Method:    native_icmp_WinPing4Async
* Signature: (IILjava/util/function/Consumer;)I
*/
JNIEXPORT jint JNICALL Java_at_spindi_WinPing_native_1icmp_1WinPing4Async
(JNIEnv *env, jclass clazz, jint bigEndianv4Address, jint timeoutMs, jobject Consumer) {
	//
	// Do something! Do something!
	//
	MY_JNI_CONTEXT *jniCtx = (MY_JNI_CONTEXT*) HeapAlloc(GetProcessHeap(), 0, sizeof(MY_JNI_CONTEXT));
	jniCtx->Consumer = Consumer;
	jniCtx->env = env;

	const DWORD enqueueRc = enqueuePing(bigEndianv4Address, timeoutMs, jniCallback, jniCtx);
	if (enqueueRc != 0) {
		HeapFree(GetProcessHeap(), 0, jniCtx);
	}

	return enqueueRc;
}

// ----------------------------------------------------------------------------
//	native functions
// ----------------------------------------------------------------------------

typedef struct {
	IPAddr			ip;
	DWORD			timeoutMs;
	pingCallback	callback;
	LPVOID			callbackContext;
	MY_DATA			requestbuffer;
	MY_ICMP_REPLY	replybuffer;
} APC_PING_WORKITEM;

// ----------------------------------------------------------------------------

void WINAPI ApcCallback_IcmpSendEcho(IN PVOID ApcContext, IN PIO_STATUS_BLOCK IoStatusBlock, IN ULONG Reserved) {
	APC_PING_WORKITEM* context = (APC_PING_WORKITEM*)ApcContext;

	DWORD replies = IcmpParseReplies(&(context->replybuffer), sizeof(MY_ICMP_REPLY));

	int ipStatus = -1;
	DWORD roundtrip = 0;
	if (replies > 0) {
		ipStatus  = context->replybuffer.reply.Status;
		roundtrip = context->replybuffer.reply.RoundTripTime;
	}

	pingCallback	pfnCallback			= context->callback;
	LPVOID			lpCallbackContext	= context->callbackContext;
	IPAddr			ip					= context->ip;

	//
	// free resources BEFORE callback
	//
	HeapFree(GetProcessHeap(), 0, context);

	pfnCallback(
		ip,
		roundtrip,
		ipStatus,
		0,
		lpCallbackContext);
}

void NTAPI ApcCallback_ProcessWorkitem(ULONG_PTR Parameter) {
	APC_PING_WORKITEM* context = (APC_PING_WORKITEM*)Parameter;
	//
	// we are now within the "ping thread"
	// let's send an async ping...
	//

	DWORD rcSendecho = IcmpSendEcho2(
		gWinPing->hIcmpFile,
		NULL,						// event
		ApcCallback_IcmpSendEcho,
		context,					// Apc context
		context->ip,
		(LPVOID)&(context->requestbuffer),
		sizeof(MY_DATA),
		NULL,						//PIP_OPTION_INFORMATION
		(LPVOID)&(context->replybuffer),
		sizeof(MY_ICMP_REPLY),
		context->timeoutMs);

	if ( rcSendecho != ERROR_IO_PENDING ) {
		int sendError = GetLastError();

		auto fnCallback		   = context->callback;
		auto lpCallbackContext = context->callbackContext;
		auto ip				   = context->ip;

		//
		// clean up BEFORE calling the callback
		//
		HeapFree(GetProcessHeap(), 0, context);

		fnCallback(ip, 0, -1, sendError, lpCallbackContext);
	}
}

DWORD WINAPI ThreadProc(LPVOID lpThreadParameter) {

	DWORD sleepRc;
	while ( (sleepRc=SleepEx(10 * 1000, TRUE)) == WAIT_IO_COMPLETION) {
	}
	if (sleepRc == 0) {
		// timeout!
		//
		// try to write -1 to the global counter
		//
		//_InterlockedCompareExchange(&(gWinPing->asyncCounter), -1, 
	}
	return 0;
}

DWORD enqueuePing(const IPAddr ipToPing, const DWORD timeoutMs, const pingCallback callback, const LPVOID callbackContext) {
	
	if (_InterlockedIncrement(&(gWinPing->asyncCounter)) == 0L) {
		// means: APC-thread has stoppped working or isn't started yet
		// start it
		gWinPing->hThread = CreateThread(NULL, 1, (LPTHREAD_START_ROUTINE)ThreadProc, NULL, 0, NULL);
		if (gWinPing->hThread == NULL) {
			return GetLastError();
		}
	}

	APC_PING_WORKITEM* context = (APC_PING_WORKITEM*) HeapAlloc(GetProcessHeap(), 0, sizeof(APC_PING_WORKITEM));
	context->ip = ipToPing;
	context->timeoutMs = timeoutMs;
	context->callback = callback;
	context->callbackContext = callbackContext;

	if (QueueUserAPC(ApcCallback_ProcessWorkitem, gWinPing->hThread, (ULONG_PTR)context) != 0) {
		return GetLastError();
	}

	return 0;
}



