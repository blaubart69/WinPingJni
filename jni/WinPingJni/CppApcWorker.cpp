#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "CppApcWorker.h"
// -----------------------------------------------------------------------------
template <class T>
void ApcWorker<T>::_init(
	const ApcProcessItemCallback*		pfnApcProcessItem,
	const ApcCallAfterThreadStarted*	pfnCallAfterThreadStarted = nullptr,
	const ApcCallBeforeThreadEnd*		pfnCallBeforeThreadEnd = nullptr)
{
	this->_internalThreadCounter = 0;
	this->_itemCounter = 0;
	this->_hTread = NULL;
	this->_pfnApcProcessItemCallback = pfnApcProcessItem;
	this->_pfnCallAfterThreadStarted = pfnCallAfterThreadStarted;
	this->_pfnCallBeforeThreadEnd   = pfnCallBeforeThreadEnd;

	InitializeCriticalSection(&_criticalEnqueue);
}
// -----------------------------------------------------------------------------
template <class T>
ApcWorker<T>::ApcWorker(const ApcProcessItemCallback* pfnApcProcessItem)
{
	_init(pfnApcProcessItem, nullptr, nullptr);
}
template <class T>
ApcWorker<T>::ApcWorker(	
	const ApcProcessItemCallback*		pfnApcProcessItem,
	const ApcCallAfterThreadStarted*	pfnCallAfterThreadStarted,
	const ApcCallBeforeThreadEnd*		pfnCallBeforeThreadEnd) 
{
	_init(pfnApcProcessItem, pfnCallAfterThreadStarted, pfnCallBeforeThreadEnd);
}
// -----------------------------------------------------------------------------
template <class T>
ApcWorker<T>::~ApcWorker()
{
	DeleteCriticalSection(&_criticalEnqueue);
}
// -----------------------------------------------------------------------------
template <class T>
void NTAPI ApcWorker<T>::internal_CallAfterThreadStart(ULONG_PTR Parameter) {
	if (this->_pfnCallAfterThreadStarted != nullptr) {
		_pfnCallAfterThreadStarted();
	}
}
// -----------------------------------------------------------------------------
template <class T>
void NTAPI ApcWorker<T>::internal_ProcessWorkitem(ULONG_PTR Parameter) {

	_internalThreadCounter += 1;

	const T* data = (T*)Parameter;
	_pfnApcProcessItemCallback(data);
}
// -----------------------------------------------------------------------------
template <class T>
DWORD ApcWorker<T>::enqueue(const T* ctx) {

	EnterCriticalSection(&_criticalEnqueue);
	if ( _InterlockedIncrement(&_itemCounter == 0) ) {
		// means: APC-thread has stoppped working or isn't started yet --> start it

		// 
		// why we set this counter to zero at this place,
		// read remark in function ThreadProc() below
		//
		_internalThreadCounter = 0;

		_hThread = CreateThread(NULL, 1, (LPTHREAD_START_ROUTINE)ThreadProc, NULL, 0, NULL);
		if (_hThread == NULL) {
			LeaveCriticalSection(&_criticalEnqueue);
			return GetLastError();
		}
		//
		// queue the "special" first APC user call
		//
		if (this->_pfnCallAfterThreadStarted != nullptr) {
			if (QueueUserAPC(internal_CallAfterThreadStart, _hTread, ctx) != 0) {
				return GetLastError();
			}
		}
	}
	LeaveCriticalSection(&_criticalEnqueue);

	if (QueueUserAPC(internal_ProcessWorkitem, _hTread, ctx) != 0) {
		return GetLastError();
	}

	return 0;
}
// -----------------------------------------------------------------------------
template <class T>
DWORD WINAPI ApcWorker<T>::ThreadProc(LPVOID lpThreadParameter) {

	for (;;) {
		while (SleepEx(10 * 1000, TRUE) == WAIT_IO_COMPLETION) {
		}
		// timeout!
		//
		// try to write -1 to the global counter
		//
		if (_InterlockedCompareExchange(
			&_itemCounter,
			-1,
			_internalThreadCounter) == _internalThreadCounter) {
			//
			// the "global" counter == "internal" counter
			// no more work have been enqued between SleepEx() and the CAS operation
			// we are going down...
			//
			//
			// this is not thread safe here!!
			//	We set this counter back to zero in the enqueue function, because
			//	after the above Interlocked operation it could be that...
			//	1, a new thread is started (via enqueue)
			//	2, a APC is enqueued
			//	3, executed on the "new" thread
			//	4, and this counter is incremented
			//		so this "= 0" is a race condition
			//	we will do it in the enqueue() function
			//	when the new thread is started there are NO MORE APC callbacks!
			//	so no one is incrementing this counter
			//_internalThreadCounter = 0;

			break;
		}
	}

	if (_pfnCallBeforeThreadEnd != nullptr) {
		_pfnCallBeforeThreadEnd();
	}

	return 0;
}
// -----------------------------------------------------------------------------

/*

typedef struct {
	IPAddr			ip;
	DWORD			timeoutMs;
	pingCallback	callback;
	LPVOID			callbackContext;
	MY_DATA			requestbuffer;
	MY_ICMP_REPLY	replybuffer;
} APC_PING_WORKITEM;

UINT64 _internalThreadApcCount;

// ----------------------------------------------------------------------------

void WINAPI ApcCallback_IcmpSendEcho(IN PVOID ApcContext, IN PIO_STATUS_BLOCK IoStatusBlock, IN ULONG Reserved) {
	APC_PING_WORKITEM* context = (APC_PING_WORKITEM*)ApcContext;

	DWORD replies = IcmpParseReplies(&(context->replybuffer), sizeof(MY_ICMP_REPLY));

	int ipStatus = -1;
	DWORD roundtrip = 0;
	if (replies > 0) {
		ipStatus = context->replybuffer.reply.Status;
		roundtrip = context->replybuffer.reply.RoundTripTime;
	}

	pingCallback	pfnCallback = context->callback;
	LPVOID			lpCallbackContext = context->callbackContext;
	IPAddr			ip = context->ip;

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
	_internalThreadApcCount += 1;

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

	if (rcSendecho != ERROR_IO_PENDING) {
		int sendError = GetLastError();

		auto pfnCallback = context->callback;
		auto lpCallbackContext = context->callbackContext;
		auto ip = context->ip;

		//
		// clean up BEFORE calling the callback
		//
		HeapFree(GetProcessHeap(), 0, context);

		pfnCallback(ip, 0, -1, sendError, lpCallbackContext);
	}
}

DWORD WINAPI ThreadProc(LPVOID lpThreadParameter) {

	for (;;) {
		while (SleepEx(10 * 1000, TRUE) == WAIT_IO_COMPLETION) {
		}
		// timeout!
		//
		// try to write -1 to the global counter
		//
		if (_InterlockedCompareExchange(
			&(gWinPing->asyncCounter),
			-1,
			_internalThreadApcCount) == _internalThreadApcCount) {
			//
			// the "global" counter == "internal" counter
			// no more work have been enqued between SleepEx() and the CAS operation
			// we are going down...
			//
			break;
		}
	}
	return 0;
}

DWORD enqueuePing(const IPAddr ipToPing, const DWORD timeoutMs, const pingCallback callback, const LPVOID callbackContext) {

	if (_InterlockedIncrement(&(gWinPing->asyncCounter)) == 0) {
		// means: APC-thread has stoppped working or isn't started yet
		// start it
		_internalThreadApcCount = 0;
		gWinPing->hThread = CreateThread(NULL, 1, (LPTHREAD_START_ROUTINE)ThreadProc, NULL, 0, NULL);
		if (gWinPing->hThread == NULL) {
			return GetLastError();
		}
	}

	APC_PING_WORKITEM* context = (APC_PING_WORKITEM*)HeapAlloc(GetProcessHeap(), 0, sizeof(APC_PING_WORKITEM));
	if (context == NULL) {
		return STATUS_NO_MEMORY;
	}

	context->ip = ipToPing;
	context->timeoutMs = timeoutMs;
	context->callback = callback;
	context->callbackContext = callbackContext;

	if (QueueUserAPC(ApcCallback_ProcessWorkitem, gWinPing->hThread, (ULONG_PTR)context) != 0) {
		HeapFree(GetProcessHeap(), 0, context);
		return GetLastError();
	}

	return 0;
}

*/