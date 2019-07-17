#include "at_spindi_WinPing.h"
#include "WinPingJni.h"

// global structure initialized via "startup" call
extern WIN_PING_GLOBAL* gWinPing;

DWORD	WINAPI	ThreadProc(LPVOID lpThreadParameter);
VOID	NTAPI	ApcSendPingAsync(ULONG_PTR Parameter);
DWORD			enqueue(PING_CTX* pingCtx);

// -----------------------------------------------------------------------------
void jniPingCompletedCallback(IPAddr ip, DWORD roundtrip, int pingStatus, int LastError, jobject globalRefobjConsumer) {
// -----------------------------------------------------------------------------

	// the JNIenv of the APC thread ("AttachToCurrentThread")
	JNIEnv* APCJniEnv = gWinPing->ApcJniEnv;
	//
	// create return object
	//            
	jclass			WinPingResultClazz	= (*APCJniEnv)->FindClass	(APCJniEnv, "at/spindi/WinPingResult");
	jmethodID		WinPingResultCtor	= (*APCJniEnv)->GetMethodID	(APCJniEnv, WinPingResultClazz, "<init>", "(III)V");
	const jobject   WinPingResultObj	= (*APCJniEnv)->NewObject	(APCJniEnv, WinPingResultClazz, WinPingResultCtor, LastError, pingStatus, roundtrip);
	//
	// calling the "consumer" callback
	//
	const jclass	consumerClass = (*APCJniEnv)->GetObjectClass	(APCJniEnv, globalRefobjConsumer);
	const jmethodID acceptMethod  = (*APCJniEnv)->GetMethodID		(APCJniEnv, consumerClass, "accept", "(Ljava/lang/Object;)V");
	//
	//
	//
	(*APCJniEnv)->CallVoidMethod(APCJniEnv, globalRefobjConsumer, acceptMethod, WinPingResultObj);
	//
	// get rid of the global reference
	//
	(*APCJniEnv)->DeleteGlobalRef(APCJniEnv, globalRefobjConsumer);
}
/*
* Class:     at_spindi_WinPing
* Method:    native_icmp_WinPing4Async
* Signature: (IILjava/util/function/Consumer;)I
*/
// -----------------------------------------------------------------------------
JNIEXPORT jint JNICALL Java_at_spindi_WinPing_native_1icmp_1WinPing4Async
(JNIEnv *env, const jclass clazz, const jint bigEndianv4Address, const jint timeoutMs, const jobject objConsumer) {
// -----------------------------------------------------------------------------
	//
	// Do something! Do something! (C) by Schurl
	//
	PING_CTX *pingCtx = (PING_CTX*)HeapAlloc(GetProcessHeap(), 0, sizeof(PING_CTX));
	if (pingCtx == NULL) {
		return ERROR_OUTOFMEMORY;
	}

	pingCtx->ip						= bigEndianv4Address;
	pingCtx->timeoutMs				= timeoutMs;
	pingCtx->globalRefobjConsumer	= (*env)->NewGlobalRef(env, objConsumer);

	DWORD rc = 0;
	if (QueueUserAPC(ApcSendPingAsync, gWinPing->hTread, (ULONG_PTR)pingCtx) == 0) {
		rc = GetLastError();
		(*env)->DeleteGlobalRef(env, pingCtx->globalRefobjConsumer);
		HeapFree(GetProcessHeap(), 0, pingCtx);
	}

	return rc;
}
// -----------------------------------------------------------------------------
// APC worker functions
// -----------------------------------------------------------------------------
DWORD WINAPI ThreadProc(LPVOID lpThreadParameter) {
// -----------------------------------------------------------------------------
	JavaVMAttachArgs args;
	args.version = JNI_VERSION_1_6; // choose your JNI version
	args.name = "WinPingApcPing"; // you might want to give the java thread a name
	args.group = NULL; // you might want to assign the java thread to a ThreadGroup

	JNIEnv* jniEnv;

	//(*gWinPing->vm)->AttachCurrentThread(gWinPing->vm, (void**)(&myNewEnv), &args);
	const jint attachRc =
		(*gWinPing->vm)->AttachCurrentThread(
			gWinPing->vm,
			(void**)(&(gWinPing->ApcJniEnv)),
			&args);

	while (WaitForSingleObjectEx(gWinPing->shutdownEvent, INFINITE, TRUE) == WAIT_IO_COMPLETION)
		;

	const jint detachRc = (*gWinPing->vm)->DetachCurrentThread(gWinPing->vm);
	CloseHandle(gWinPing->shutdownEvent);
	IcmpCloseHandle(gWinPing->hIcmpFile);
	HeapFree(GetProcessHeap(), 0, gWinPing);

	return 0;
}
// -----------------------------------------------------------------------------
VOID NTAPI ApcOnPingCompleted(IN PVOID ApcContext, IN PIO_STATUS_BLOCK IoStatusBlock, IN ULONG Reserved) {
// -----------------------------------------------------------------------------
	PING_CTX* pingCtx = (PING_CTX*)ApcContext;

	DWORD replies = IcmpParseReplies(&pingCtx->icmpReply, sizeof(MY_ICMP_REPLY));

	int ipStatus = -1;
	DWORD roundtrip = 0;
	if (replies > 0) {
		ipStatus = pingCtx->icmpReply.reply.Status;
		roundtrip = pingCtx->icmpReply.reply.RoundTripTime;
	}

	jniPingCompletedCallback(pingCtx->ip, roundtrip, ipStatus, 0, pingCtx->globalRefobjConsumer);

	HeapFree(GetProcessHeap(), 0, pingCtx);
}
// -----------------------------------------------------------------------------
VOID NTAPI ApcSendPingAsync(ULONG_PTR Parameter) {
// -----------------------------------------------------------------------------
	PING_CTX* pingCtx = (PING_CTX*)Parameter;
	//
	// we are now within the "ping thread"
	// let's send an async ping...
	//
	const DWORD rcSendecho = IcmpSendEcho2(
		gWinPing->hIcmpFile,		
		NULL,						// event
		ApcOnPingCompleted,
		pingCtx,					// Apc context
		pingCtx->ip,
		(LPVOID)&(pingCtx->icmpReply.data),
		sizeof(MY_DATA),
		NULL,						//PIP_OPTION_INFORMATION
		(LPVOID)&(pingCtx->icmpReply.reply),
		sizeof(MY_ICMP_REPLY),
		pingCtx->timeoutMs);
	/*
		When called asynchronously, the IcmpSendEcho2 function returns ERROR_IO_PENDING to indicate the operation is in progress. 
		The results can be retrieved later when the event specified in the Event parameter signals or the callback function 
		in the ApcRoutine parameter is called.
		If the return value is zero, call GetLastError for extended error information.
		If the function fails, the extended error code returned by GetLastError can be one of the following values.
		...
		ERROR_IO_PENDING
		...

		--> this all means, WHEN we get an RC != ERROR_IO_PENDING
			THEN the GetLastError() could also be ERROR_IO_PENDING and does mean NO_ERROR!!! (grrrr!)
	*/
	if (rcSendecho != ERROR_IO_PENDING) {
		// MAYBE an error. Check again.
		const int send2LastError = GetLastError();

		//  check again if IO_PENDING
		if (send2LastError != ERROR_IO_PENDING) {
			//
			// some error occured. APC will not be called.
			// clean up here
			//
			jniPingCompletedCallback(pingCtx->ip, 0, -1, send2LastError, pingCtx->globalRefobjConsumer);
			HeapFree(GetProcessHeap(), 0, pingCtx);
		}
	}
}
