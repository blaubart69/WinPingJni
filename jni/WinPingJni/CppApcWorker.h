#pragma once

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>

template <class T>
class ApcWorker
{
public:
	typedef void(*ApcProcessItemCallback)(const T* ctx);
	typedef void(*ApcCallAfterThreadStarted)();
	typedef void(*ApcCallBeforeThreadEnd)();

	ApcWorker(const ApcProcessItemCallback* pfnApcProcessItem);
	ApcWorker(	const ApcProcessItemCallback*		pfnApcProcessItem, 
				const ApcCallAfterThreadStarted*	pfnCallAfterThreadStarted,
				const ApcCallBeforeThreadEnd*		pfnCallAfterThreadEnded);
	~ApcWorker();

	DWORD enqueue(const T* ctx);

private:
	__declspec(align(64))volatile	INT64 _itemCounter;

	INT64 _internalThreadCounter;
	HANDLE _hTread;

	const ApcProcessItemCallback*		_pfnApcProcessItemCallback;
	const ApcCallAfterThreadStarted*	_pfnCallAfterThreadStarted;
	const ApcCallBeforeThreadEnd*		_pfnCallBeforeThreadEnd;

	const CRITICAL_SECTION _criticalEnqueue;

	DWORD WINAPI ThreadProc(LPVOID lpThreadParameter);
	VOID  NTAPI  internal_ProcessWorkitem(ULONG_PTR Parameter);
	VOID  NTAPI  internal_CallAfterThreadStart(ULONG_PTR Parameter);

	void _init(
		const ApcProcessItemCallback*		pfnApcProcessItem, 
		const ApcCallAfterThreadStarted*	pfnCallAfterThreadStarted = nullptr,
		const ApcCallBeforeThreadEnd*		pfnCallAfterThreadEnded = nullptr);
};
