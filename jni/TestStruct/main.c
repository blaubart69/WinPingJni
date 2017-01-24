#include "../WinPingJni.h"

#include <stdio.h>

int main(int argc, char* argv) {

	MY_DATA			SendData = { .data = "WinPingJni Send Buffer Data" };
	MY_ICMP_REPLY	ReplyBuffer = { 0 };

	int sizeSendData = sizeof(SendData);
	int sizeReplyBuffer = sizeof(ReplyBuffer);
	int sizeIcmpReply = sizeof(ReplyBuffer.reply);

	printf("reply.reply      = %d\n", (int)sizeof(ReplyBuffer.reply));
	printf("reply.data       = %d\n", (int)sizeof(ReplyBuffer.data));
	printf("reply.extra_data = %d\n", (int)sizeof(ReplyBuffer.extra_data));
	printf("---------------------\n");
	printf("reply struct     = %d\n", (int)sizeof(ReplyBuffer));

#if _WIN64

	ICMP_ECHO_REPLY		r;
	ICMP_ECHO_REPLY32 r32;

	printf("\n");
	printf("ICMP_ECHO_REPLY     = %d\n", (int)sizeof(ICMP_ECHO_REPLY));
	printf("ICMP_ECHO_REPLY32   = %d\n", (int)sizeof(ICMP_ECHO_REPLY32));
	printf("\n");
	printf("Address:       %d - %d\n",sizeof(r.Address), sizeof(r32.Address));
	printf("Data:          %d - %d\n", sizeof(r.Data), sizeof(r32.Data));
	printf("DataSize:      %d - %d\n", sizeof(r.DataSize), sizeof(r32.DataSize));
	printf("Options:       %d - %d\n", sizeof(r.Options), sizeof(r32.Options));
	printf("Reserved:	   %d - %d\n", sizeof(r.Reserved), sizeof(r32.Reserved));
	printf("RoundTripTime: %d - %d\n", sizeof(r.RoundTripTime), sizeof(r32.RoundTripTime));
	printf("Status:        %d - %d\n", sizeof(r.Status), sizeof(r32.Status));
	printf("\n");
	printf("Options.Flags:        %d - %d\n", sizeof(r.Options.Flags), sizeof(r32.Options.Flags));
	printf("Options.OptionsData:  %d - %d\n", sizeof(r.Options.OptionsData), sizeof(r32.Options.OptionsData));
	printf("Options.OptionsSize:  %d - %d\n", sizeof(r.Options.OptionsSize), sizeof(r32.Options.OptionsSize));
	printf("Options.Tos:          %d - %d\n", sizeof(r.Options.Tos), sizeof(r32.Options.Tos));
	printf("Options.Ttl:          %d - %d\n", sizeof(r.Options.Ttl), sizeof(r32.Options.Ttl));


#endif
}