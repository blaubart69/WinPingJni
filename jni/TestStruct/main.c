#include "../WinPingJni.h"

#include <stdio.h>


void print3264(const char* label, const int r, const int r32) {
	printf("%-25s %3d - %3d\n", label, r, r32);
}

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
	print3264("Address",sizeof(r.Address), sizeof(r32.Address));
	print3264("Data", sizeof(r.Data), sizeof(r32.Data));
	print3264("DataSize", sizeof(r.DataSize), sizeof(r32.DataSize));
	print3264("Options", sizeof(r.Options), sizeof(r32.Options));
	print3264("Reserved", sizeof(r.Reserved), sizeof(r32.Reserved));
	print3264("RoundTripTime", sizeof(r.RoundTripTime), sizeof(r32.RoundTripTime));
	print3264("Status", sizeof(r.Status), sizeof(r32.Status));
	printf("\n");
	print3264("Options.Flags", sizeof(r.Options.Flags), sizeof(r32.Options.Flags));
	print3264("Options.OptionsData", sizeof(r.Options.OptionsData), sizeof(r32.Options.OptionsData));
	print3264("Options.OptionsSize", sizeof(r.Options.OptionsSize), sizeof(r32.Options.OptionsSize));
	print3264("Options.Tos", sizeof(r.Options.Tos), sizeof(r32.Options.Tos));
	print3264("Options.Ttl", sizeof(r.Options.Ttl), sizeof(r32.Options.Ttl));


#endif
}

