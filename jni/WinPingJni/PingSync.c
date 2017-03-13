#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>
#include <IPHlpApi.h>
#include <IcmpAPI.h>

// Need to link with Iplhlapi.lib
//#pragma comment(lib, "IPHLPAPI.lib")

#include "at_spindi_WinPing.h"
#include "WinPingJni.h"


// global structure initialized via "startup" call
extern WIN_PING_GLOBAL* gWinPing;

JNIEXPORT jint JNICALL 
Java_at_spindi_WinPing_native_1icmp_1WinPing4 (const JNIEnv *env, const jclass cl, const jint IpAdress, const jint TimeoutMs) {
		
	MY_DATA			SendData;		// = { .data = "WinPingJni Send Buffer Data" };
	MY_ICMP_REPLY	ReplyBuffer;	// = { 0 };

	const DWORD ReplysReceived = IcmpSendEcho(
		gWinPing->hIcmpFile,
		IpAdress,
		(LPVOID)&SendData,
		sizeof(SendData),
		NULL,				// option information
		(LPVOID)&ReplyBuffer,
		sizeof(ReplyBuffer),
		TimeoutMs);

	if (ReplysReceived == 0) {
#ifdef _DEBUG
		WCHAR IpErrorMsg[1024];
		DWORD ErrSize = sizeof(IpErrorMsg);
		GetIpErrorString(ReplyBuffer.reply.Status, IpErrorMsg, &ErrSize);
#endif
		DWORD replyLastError = GetLastError();
		return replyLastError;
	}

	const ULONG IpStatus = ReplyBuffer.reply.Status;

	return IpStatus;
}
