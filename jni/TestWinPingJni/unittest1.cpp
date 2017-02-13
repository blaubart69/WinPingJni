#include "stdafx.h"
#include "CppUnitTest.h"



using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestWinPingJni
{		
	TEST_CLASS(WinPingJni)
	{
	public:
		
		TEST_METHOD(PingLocalHost)
		{
			IN_ADDR ip;
			ip.S_un.S_un_b.s_b1 = 127;
			ip.S_un.S_un_b.s_b2 = 0;
			ip.S_un.S_un_b.s_b3 = 0;
			ip.S_un.S_un_b.s_b4 = 1;

			int rc = Java_at_spindi_WinPing_native_1icmp_1WinPing4(NULL, NULL, ip.S_un.S_addr, 1000);

			Assert::AreEqual(0, rc);
		}
		TEST_METHOD(PingUnknown)
		{
			IN_ADDR ip;
			ip.S_un.S_un_b.s_b1 = 126;
			ip.S_un.S_un_b.s_b2 = 0;
			ip.S_un.S_un_b.s_b3 = 0;
			ip.S_un.S_un_b.s_b4 = 1;

			int rc = Java_at_spindi_WinPing_native_1icmp_1WinPing4(NULL, NULL, ip.S_un.S_addr, 100);

			Assert::AreNotEqual(0, rc);
		}
		TEST_METHOD(Ping8888)
		{
			IN_ADDR ip;
			ip.S_un.S_un_b.s_b1 = 8;
			ip.S_un.S_un_b.s_b2 = 8;
			ip.S_un.S_un_b.s_b3 = 8;
			ip.S_un.S_un_b.s_b4 = 8;

			int rc = Java_at_spindi_WinPing_native_1icmp_1WinPing4(NULL, NULL, ip.S_un.S_addr, 1000);

			Assert::AreEqual<int>(0, rc);
		}
	};
}