#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>
#include <IPHlpApi.h>
#include <IcmpAPI.h>

#include <Ws2tcpip.h>

#include "WinPingJni.h"
extern WIN_PING_GLOBAL* gWinPing;	// global structure initialized via "startup" call

// -----------------------------------------------------------------------------
JNIEXPORT jobject JNICALL
Java_at_spindi_WinPing_native_1icmp_1WinPing6(JNIEnv* env, jclass cl, jbyteArray SourceAddress, jbyteArray DestinationAddress, jint timeoutMs) {
// -----------------------------------------------------------------------------

	struct sockaddr_in6 srcIPv6 = { 0 };
	struct sockaddr_in6 dstIPv6 = { 0 };

	(*env)->GetByteArrayRegion(env, SourceAddress,      0, 16, (jbyte*)(&srcIPv6.sin6_addr));
	(*env)->GetByteArrayRegion(env, DestinationAddress, 0, 16, (jbyte*)(&dstIPv6.sin6_addr));

	MY_DATA			RequestData;	// = { .data = "WinPingJni Send Buffer Data" };
	MY_ICMPV6_REPLY	ReplyBuffer;	// = { 0 };

	const DWORD ReplysReceived = Icmp6SendEcho2(
		gWinPing->hIcmp6File
		, NULL	// event
		, NULL	// ApcRoutine
		, NULL	// ApcContext
		, &srcIPv6
		, &dstIPv6
		, (LPVOID)& RequestData
		, sizeof(RequestData)
		, NULL		// option information
		, (LPVOID)& ReplyBuffer
		, sizeof(ReplyBuffer)
		, timeoutMs);

	jobject    WinPingResultObj;

	if (ReplysReceived == 0) {
		WinPingResultObj = newWinPingResult(env, GetLastError(), -1, -1);
	}
	else
	{
		WinPingResultObj = newWinPingResult(env, GetLastError(), ReplyBuffer.reply->Status, ReplyBuffer.reply->RoundTripTime);
	}

	return WinPingResultObj;
}