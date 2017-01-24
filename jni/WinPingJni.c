#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>
#include <IPHlpApi.h>
#include <IcmpAPI.h>

#include <stdarg.h>
#include <strsafe.h>
#include <malloc.h>

// Need to link with Iplhlapi.lib
#pragma comment(lib, "IPHLPAPI.lib")

#include "at_spindi_WinPing.h"
#include "WinPingJni.h"


void dbgprint(const wchar_t *func, int line, const wchar_t *fmt, ...) {
	INT cbFormatString = 0;
	va_list args;
	PWCHAR wszDebugString = NULL;
	size_t st_Offset = 0;

	va_start(args, fmt);

	cbFormatString = _scwprintf(L"[%s:%d] ", func, line) * sizeof(WCHAR);
	cbFormatString += _vscwprintf(fmt, args) * sizeof(WCHAR) + 2;

	/* Depending on the size of the format string, allocate space on the stack or the heap. */
	wszDebugString = (PWCHAR)_malloca(cbFormatString);

	/* Populate the buffer with the contents of the format string. */
	StringCbPrintfW(wszDebugString, cbFormatString, L"[%s:%d] ", func, line);
	StringCbLengthW(wszDebugString, cbFormatString, &st_Offset);
	StringCbVPrintfW(&wszDebugString[st_Offset / sizeof(WCHAR)], cbFormatString - st_Offset, fmt, args);

	OutputDebugStringW(wszDebugString);

	_freea(wszDebugString);
	va_end(args);
}
#define DBGPRINT(fmt, ...) dbgprint(L"func", __LINE__, fmt, __VA_ARGS__)


JNIEXPORT jint JNICALL 
Java_at_spindi_WinPing_native_1icmp_1WinPing4 (JNIEnv *env, jclass cl, jint IpAdress, jint TimeoutMs) {
		
	const HANDLE hIcmpFile = IcmpCreateFile();
	if (hIcmpFile == INVALID_HANDLE_VALUE) {
		return GetLastError();
	}

	MY_DATA			SendData	= { .data = "WinPingJni Send Buffer Data" };
	MY_ICMP_REPLY	ReplyBuffer	= { 0 };

	int sizeSendData	= sizeof(SendData);
	int sizeReplyBuffer = sizeof(ReplyBuffer);
	int sizeIcmpReply	= sizeof(ReplyBuffer.reply);

	DBGPRINT(L"sizeof(ReplyBuffer) = %d\n", sizeReplyBuffer);
	DBGPRINT(L"sizeof(IcmpReply)   = %d\n", sizeIcmpReply);

	DWORD ReplysReceived = IcmpSendEcho(
		hIcmpFile,
		IpAdress,
		(LPVOID)&SendData,
		sizeof(SendData),
		NULL,				// option information
		(LPVOID)&ReplyBuffer,
		sizeof(ReplyBuffer),
		TimeoutMs);

	if (ReplysReceived == 0) {

		WCHAR IpErrorMsg[1024];
		DWORD ErrSize = sizeof(IpErrorMsg);
		GetIpErrorString(ReplyBuffer.reply.Status, IpErrorMsg, &ErrSize);

		DWORD replyLastError = GetLastError();
		return replyLastError;
	}

	const ULONG IpStatus = ReplyBuffer.reply.Status;

	//
	// free up resources
	//
	IcmpCloseHandle(hIcmpFile);

	return IpStatus;
}
