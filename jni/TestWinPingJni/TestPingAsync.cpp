#include "stdafx.h"
#include "CppUnitTest.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestWinPingJni
{
	TEST_CLASS(WinPingJniAsync)
	{
	public:

		TEST_METHOD(AsyncPingLocalHost)
		{
			IN_ADDR ip;
			ip.S_un.S_un_b.s_b1 = 127;
			ip.S_un.S_un_b.s_b2 = 0;
			ip.S_un.S_un_b.s_b3 = 0;
			ip.S_un.S_un_b.s_b4 = 1;

			//(JNIEnv *env, jclass clazz, jint bigEndianv4Address, jint timeoutMs, jobject Consumer)
			//int rc = Java_at_spindi_WinPing_native_1icmp_1WinPing4Async(NULL, NULL, ip, 1000, )

			//Assert::AreEqual(0, rc);
		}
	};
}