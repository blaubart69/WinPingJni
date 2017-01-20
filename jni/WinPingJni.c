#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>
#include <IPHlpApi.h>
#include <IcmpAPI.h>

// Need to link with Iplhlapi.lib
#pragma comment(lib, "IPHLPAPI.lib")

#include <stdlib.h>

#include "at_spindi_WinPing.h"

JNIEXPORT jint JNICALL 
Java_at_spindi_WinPing_native_1icmp_1WinPing4 (JNIEnv *env, jclass cl, const jint IpAdress, const jint TimeoutMs) {
		
	const HANDLE hIcmpFile = IcmpCreateFile();
	if (hIcmpFile == INVALID_HANDLE_VALUE) {
		return GetLastError();
	}

	const char SendData[32] = "WinPingJni Data Buffer";

#if _WIN32
	const int sizeOfReplyBuffer = sizeof(ICMP_ECHO_REPLY)   + sizeof(SendData);
#else
	const int sizeOfReplyBuffer = sizeof(ICMP_ECHO_REPLY32) + sizeof(SendData);
#endif

	LPVOID ReplyBuffer = malloc(sizeOfReplyBuffer);

	DWORD sendRc = IcmpSendEcho(
		hIcmpFile,
		IpAdress,
		SendData,
		sizeof(SendData),
		NULL,				// option information
		ReplyBuffer,
		sizeOfReplyBuffer,
		TimeoutMs);

	PICMP_ECHO_REPLY reply = (PICMP_ECHO_REPLY)ReplyBuffer;
	const ULONG IpStatus = reply->Status;

	//
	// free up ressources
	//
	free(ReplyBuffer);
	IcmpCloseHandle(hIcmpFile);

	return IpStatus;
}
