#pragma once

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>
#include <IPHlpApi.h>
#include <IcmpAPI.h>


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

