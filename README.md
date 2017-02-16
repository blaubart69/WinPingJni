# WinPingJni
pinging with Java on Windows

since there is no Icmp-Ping in Java, we call the native Windows function via JNI.
IcmpSendEcho()

Also we try to keep the DLL as small as possible.
So we do not link against the Visual C runtime.
