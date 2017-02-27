#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>
#include <IPHlpApi.h>
#include <IcmpAPI.h>

// Need to link with Iplhlapi.lib
//#pragma comment(lib, "IPHLPAPI.lib")

#include "at_spindi_WinPing.h"
#include "WinPingJni.h"


#ifdef _DEBUG
#include "DebugPrint.h"
#endif 

BOOLEAN WINAPI 
DllMain(IN HINSTANCE hDllHandle, IN DWORD nReason, IN LPVOID Reserved)
{
	BOOLEAN bSuccess = TRUE;
	return bSuccess;
}

extern HANDLE g_hIcmpFile;

JNIEXPORT jint JNICALL 
Java_at_spindi_WinPing_native_1icmp_1WinPing4 (JNIEnv *env, jclass cl, jint IpAdress, jint TimeoutMs) {
		
	MY_DATA			SendData;		// = { .data = "WinPingJni Send Buffer Data" };
	MY_ICMP_REPLY	ReplyBuffer;	// = { 0 };

#ifdef _DEBUG	
	int sizeSendData	= sizeof(SendData);
	int sizeReplyBuffer = sizeof(ReplyBuffer);
	int sizeIcmpReply	= sizeof(ReplyBuffer.reply);
	DBGPRINT(L"sizeof(ReplyBuffer) = %d\n", sizeReplyBuffer);
	DBGPRINT(L"sizeof(IcmpReply)   = %d\n", sizeIcmpReply);
#endif

	DWORD ReplysReceived = IcmpSendEcho(
		g_hIcmpFile,
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
