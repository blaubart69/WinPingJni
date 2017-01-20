#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>
#include <IPHlpApi.h>
#include <IcmpAPI.h>

// Need to link with Iplhlapi.lib
#pragma comment(lib, "IPHLPAPI.lib")

#include "at_spindi_WinPing.h"


typedef struct {
	char data[32];
} MY_DATA;


typedef struct {
#if _WIN64	
	ICMP_ECHO_REPLY32	reply;
#else
	ICMP_ECHO_REPLY		reply;
#endif
	MY_DATA				data;
	BYTE				extra_data[8];
} MY_ICMP_REPLY;


JNIEXPORT jint JNICALL 
Java_at_spindi_WinPing_native_1icmp_1WinPing4 (JNIEnv *env, jclass cl, jint IpAdress, jint TimeoutMs) {
		
	const HANDLE hIcmpFile = IcmpCreateFile();
	if (hIcmpFile == INVALID_HANDLE_VALUE) {
		return GetLastError();
	}

	MY_DATA SendData = { .data = "WinPingJni Send Buffer Data" };
	MY_ICMP_REPLY ReplyBuffer;

	DWORD ReplyReceived = IcmpSendEcho(
		hIcmpFile,
		IpAdress,
		(LPVOID)&SendData,
		sizeof(SendData),
		NULL,				// option information
		(LPVOID)&ReplyBuffer,
		sizeof(MY_ICMP_REPLY),
		TimeoutMs);

	if (ReplyReceived == 0) {
		return GetLastError();
	}

	const ULONG IpStatus = ReplyBuffer.reply.Status;

	//
	// free up resources
	//
	IcmpCloseHandle(hIcmpFile);

	return IpStatus;
}
