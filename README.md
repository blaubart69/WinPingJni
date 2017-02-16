# WinPingJni
pinging with Java on Windows. Only IPv4 right now.

since there is no Icmp-Ping in Java, we use the native Windows functions via JNI.
IcmpSendEcho(), ... from the Iphlpapi.dll

We try to keep the DLL as small as possible so we do not link against the Visual C runtime.
