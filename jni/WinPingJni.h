#pragma once

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>
#include <IPHlpApi.h>
//#include <IcmpAPI.h>


typedef struct my_data {
	char data[32];
} MY_DATA;


typedef struct {
#if _WIN64	
	ICMP_ECHO_REPLY		reply;
#else
	ICMP_ECHO_REPLY		reply;
#endif
	MY_DATA				data;
	BYTE				extra_data[9];
} MY_ICMP_REPLY;

