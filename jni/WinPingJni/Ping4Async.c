#include "at_spindi_WinPing.h"
#include "WinPingJni.h"

// global structure initialized via "startup" call
extern WIN_PING_GLOBAL* gWinPing;

//VOID	NTAPI	ApcSendPingAsync(ULONG_PTR Parameter);

// -----------------------------------------------------------------------------
void jniPingCompletedCallback(IPAddr ip, int LastError, jlong pingStatus, jint roundtrip, jobject globalRefobjConsumer) {
// -----------------------------------------------------------------------------

	// the JNIenv of the APC thread ("AttachToCurrentThread")
	JNIEnv* APCJniEnv = gWinPing->ApcJniEnv;
	//
	// create return object
	//
	const jobject   WinPingResultObj = newWinPingResult(APCJniEnv, LastError, pingStatus, roundtrip);
	//
	// calling the "consumer" callback
	//
	const jclass	consumerClass = (*APCJniEnv)->GetObjectClass(APCJniEnv, globalRefobjConsumer);
	const jmethodID acceptMethod = (*APCJniEnv)->GetMethodID(APCJniEnv, consumerClass, "accept", "(Ljava/lang/Object;)V");
	//
	// call the callback consumer/accept with the WinPingResult object
	//
	(*APCJniEnv)->CallVoidMethod(APCJniEnv, globalRefobjConsumer, acceptMethod, WinPingResultObj);
	//
	// get rid of the global reference
	//
	(*APCJniEnv)->DeleteGlobalRef(APCJniEnv, globalRefobjConsumer);
}
// -----------------------------------------------------------------------------
VOID NTAPI ApcOnPingCompleted(IN PVOID ApcContext, IN PIO_STATUS_BLOCK IoStatusBlock, IN ULONG Reserved) {
// -----------------------------------------------------------------------------
	PING4_CTX* pingCtx = (PING4_CTX*)ApcContext;

	DWORD replies = IcmpParseReplies(&pingCtx->icmpReply, sizeof(MY_ICMP_REPLY));

	if (replies > 0) {
		jniPingCompletedCallback(
			pingCtx->ip
			, 0
			, (jlong)(pingCtx->icmpReply.reply.Status)
			, (jint)(pingCtx->icmpReply.reply.RoundTripTime)
			, pingCtx->globalRefobjConsumer);
	}
	else {
		// The IcmpParseReplies function returns the number of ICMP responses found on success. 
		// The function returns zero on error. 
		// Call GetLastError for additional error information.
		jniPingCompletedCallback(
			pingCtx->ip
			, 0							// LastError
			, (jlong)GetLastError()		// IpStatus
			, (jint)-1					// roundtrip
			, pingCtx->globalRefobjConsumer);
	}

	HeapFree(GetProcessHeap(), 0, pingCtx);
}
// -----------------------------------------------------------------------------
VOID NTAPI ApcSendPingAsync(ULONG_PTR Parameter) {
	// -----------------------------------------------------------------------------
	PING4_CTX* pingCtx = (PING4_CTX*)Parameter;
	//
	// we are now on the "APC-ping-thread"
	// let's send an async ping...
	//
	const DWORD rcSendecho = IcmpSendEcho2(
		gWinPing->hIcmpFile,
		NULL,						// event
		ApcOnPingCompleted,
		pingCtx,					// Apc context
		pingCtx->ip,
		(LPVOID) & (pingCtx->icmpReply.data),
		sizeof(MY_DATA),
		NULL,						//PIP_OPTION_INFORMATION
		(LPVOID) & (pingCtx->icmpReply.reply),
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
			jniPingCompletedCallback(pingCtx->ip, send2LastError, -1, 0, pingCtx->globalRefobjConsumer);
			HeapFree(GetProcessHeap(), 0, pingCtx);
		}
	}
}

/*
* Class:     at_spindi_WinPing
* Method:    native_icmp_WinPing4Async
* Signature: (IILjava/util/function/Consumer;)I
*/
// -----------------------------------------------------------------------------
JNIEXPORT jint JNICALL Java_at_spindi_WinPing_native_1icmp_1WinPing4Async
(JNIEnv *env, jclass clazz, jint bigEndianv4Address, jint timeoutMs, jobject objConsumer) {
// -----------------------------------------------------------------------------
	//
	// Do something! Do something! (C) by Schurl
	//
	PING4_CTX *pingCtx = (PING4_CTX*)HeapAlloc(GetProcessHeap(), 0, sizeof(PING4_CTX));
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
