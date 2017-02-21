cl ^
/I "%JAVA_HOME%\include" /I "%JAVA_HOME%\include\win32" ^
/GS- ^
..\WinPingJni.c ^
/link /release /entry:DllMain /nodefaultlib /out:WinPingJni.dll /subsystem:console ^
kernel32.lib Iphlpapi.lib