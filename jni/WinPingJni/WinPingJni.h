#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif


#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#define PIO_APC_ROUTINE_DEFINED		// before IcmpAPI.h

#include <winternl.h>				// PIO_STATUS_BLOCK, PIO_APC_ROUTINE

#include <IPHlpApi.h>
#include <IcmpAPI.h>

#include <jni.h>

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
#if _WIN64	
	ICMPV6_ECHO_REPLY			reply[2];
	IP_OPTION_INFORMATION32		options;	
#else
	ICMP_ECHO_REPLY		reply;
#endif
	MY_DATA				data;
	BYTE				extra_data[8];
	IO_STATUS_BLOCK		io_block;
} MY_ICMPV6_REPLY;



typedef struct {
	IPAddr			ip;
	DWORD			timeoutMs;
	MY_ICMP_REPLY	icmpReply;
	jobject			globalRefobjConsumer;
} PING_CTX;

typedef struct {
	//JavaVM*				vm;
	HANDLE				hIcmpFile;
	HANDLE				hIcmp6File;
	HANDLE				shutdownEvent;
	HANDLE				hTread;
	JNIEnv*				ApcJniEnv;
} WIN_PING_GLOBAL;


jobject newWinPingResult(JNIEnv* env, jint lastError, jlong status, jint roundtrip);