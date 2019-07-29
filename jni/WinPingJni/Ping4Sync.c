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

// -----------------------------------------------------------------------------
JNIEXPORT jobject JNICALL 
Java_at_spindi_WinPing_native_1icmp_1WinPing4 (JNIEnv *env, jclass cl, jint IpAdress, jint TimeoutMs) {
// -----------------------------------------------------------------------------
		
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

	jobject result;

	if (ReplysReceived == 0) {
#ifdef _DEBUG
		WCHAR IpErrorMsg[1024];
		DWORD ErrSize = sizeof(IpErrorMsg);
		GetIpErrorString(ReplyBuffer.reply.Status, IpErrorMsg, &ErrSize);
#endif
		DWORD LastError = GetLastError();

		if (LastError == 0) {
			// should not be possible
			// >>> If the return value is zero, call GetLastError for additional error information. <<<
			result = newWinPingResult(env, LastError, (jlong)-1, (jint)-1);
		}
		else {
			jlong ipstatus = -1;
			if (LastError >= 11001 && LastError <= 11050)
			{
				ipstatus = (jlong)LastError;
				LastError = 0;
			}

			result = newWinPingResult(env, LastError, ipstatus, (jint)-1);
		}
	}
	else {
		result = newWinPingResult(env, 0, (jlong)ReplyBuffer.reply.Status, (jint)ReplyBuffer.reply.RoundTripTime);
	}

	return result;
}
