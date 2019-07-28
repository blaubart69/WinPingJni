# WinPingJni
pinging with Java on Windows. ~~Only IPv4 right now.~~

Since there is no Icmp-Ping in Java, we use the native Windows functions via JNI.
IcmpSendEcho(), IcmpSendEcho2(), Icmp6SendEcho2() from the Iphlpapi.dll.

Also implement the APC modes of the functions IcmpSendEcho2(), Icmp6SendEcho2(). (tricky!)

2019-07-28 first version of IPv6 syncron ping is available

