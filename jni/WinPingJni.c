#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>
#include <IPHlpApi.h>
#include <IcmpAPI.h>

// Need to link with Iplhlapi.lib
#pragma comment(lib, "IPHLPAPI.lib")

#include "at_spindi_WinPing.h"


typedef struct {
	char data[64];
} MY_DATA;


typedef struct {
#if _WIN64	
	ICMP_ECHO_REPLY32	reply;
#else
	ICMP_ECHO_REPLY		reply;
#endif
	MY_DATA				data;
	BYTE				extra_data[9];
} MY_ICMP_REPLY;


JNIEXPORT jint JNICALL 
Java_at_spindi_WinPing_native_1icmp_1WinPing4 (JNIEnv *env, jclass cl, jint IpAdress, jint TimeoutMs) {
		
	const HANDLE hIcmpFile = IcmpCreateFile();
	if (hIcmpFile == INVALID_HANDLE_VALUE) {
		return GetLastError();
	}

	MY_DATA SendData = { .data = "WinPingJni Send Buffer Data" };
	MY_ICMP_REPLY ReplyBuffer = { 0 };

	int sizeSendData = sizeof(SendData);
	int sizeReplyBuffer = sizeof(ReplyBuffer);
	int sizeIcmpReply = sizeof(ICMP_ECHO_REPLY32);

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
