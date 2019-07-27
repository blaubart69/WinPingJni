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
	jbyte* javaSrcIPv6 = (*env)->GetByteArrayElements(env, SourceAddress, NULL);
	jbyte* javaDstIPv6 = (*env)->GetByteArrayElements(env, DestinationAddress, NULL);

	jsize lengthSrc = (*env)->GetArrayLength(env, SourceAddress);
	jsize lengthDst = (*env)->GetArrayLength(env, DestinationAddress);

	MY_DATA			RequestData;		// = { .data = "WinPingJni Send Buffer Data" };
	MY_ICMPV6_REPLY	ReplyBuffer;	// = { 0 };

	struct sockaddr_in6 srcIPv6 = { 0 };
	struct sockaddr_in6 dstIPv6 = { 0 };

	//srcIPv6.sin6_family = AF_INET6;
	//dstIPv6.sin6_family = AF_INET6;

	memcpy(&(srcIPv6.sin6_addr), javaSrcIPv6, 16);
	memcpy(&(dstIPv6.sin6_addr), javaDstIPv6, 16);

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

	jint LastError;
	jlong pingStatus;
	jint roundtrip;

	if (ReplysReceived == 0) {
		LastError = GetLastError();
		pingStatus = -1;
		roundtrip = -1;
	}
	else
	{
		LastError = 0;
		pingStatus = ReplyBuffer.reply->Status;
		roundtrip = ReplyBuffer.reply->RoundTripTime;
	}

	jobject    WinPingResultObj = newWinPingResult(env, LastError, pingStatus, roundtrip);

	return WinPingResultObj;
}