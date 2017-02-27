#pragma once

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>

#define PIO_APC_ROUTINE_DEFINED		// before IcmpAPI.h

#include <winternl.h>				// PIO_STATUS_BLOCK, PIO_APC_ROUTINE

#include <IPHlpApi.h>
#include <IcmpAPI.h>

/*
typedef
VOID
(WINAPI *PIO_APC_ROUTINE) (
	IN PVOID ApcContext,
	IN PIO_STATUS_BLOCK IoStatusBlock,
	IN ULONG Reserved
	);
*/

typedef struct my_data {
	char data[32];
} MY_DATA;


typedef struct {
#if _WIN64	
	ICMP_ECHO_REPLY32			reply;
	IP_OPTION_INFORMATION32		options;	// as MSDN say so: On a 64-bit platform, upon return the buffer contains an array of ICMP_ECHO_REPLY32 structures followed by the options and data for the replies.
#else
	ICMP_ECHO_REPLY		reply;
#endif
	MY_DATA				data;
	BYTE				extra_data[8];
} MY_ICMP_REPLY;


typedef struct {
	HANDLE hIcmpFile;
	HANDLE hThread;

} WIN_PING_GLOBAL;

typedef void(*pingCallback)(IPAddr ip, DWORD roundtrip, int pingStatus, int LastError, void* ctx);

DWORD enqueuePing(const IPAddr ipToPing, const DWORD timeoutMs, const pingCallback callback, const LPVOID callbackContext);