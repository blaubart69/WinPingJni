java ^
 -cp out\production\WinPingJni ^
 -Djava.library.path=jni\x64\Debug ^
 -XX:+UnlockDiagnosticVMOptions ^
 -XX:NativeMemoryTracking=detail ^
 -XX:+PrintGCDetails ^
 -XX:+PrintGCTimeStamps ^
 -Xmx16M ^
 at.spindi.RunPing