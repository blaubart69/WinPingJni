package at.spindi;

import java.net.Inet4Address;

public class WinPing {
	
	static {
	      System.loadLibrary("WinPingJni"); // Load native library at runtime
	                                   // hello.dll (Windows) or libhello.so (Unixes)
	}
	
	private static native int native_WinPing_Startup();
	private static native int native_WinPing_Cleanup();
	private static native int native_icmp_WinPing4(int IpAdress, int timeout); 
	
	public static int ping4(Inet4Address v4ToPing, int timeoutMs) {

		final byte[] tmpByteArr = v4ToPing.getAddress();
		
		int 
		address  =   tmpByteArr[0] 	      & 0xFF;
        address |= ((tmpByteArr[1] <<  8) & 0xFF00);
        address |= ((tmpByteArr[2] << 16) & 0xFF0000);
        address |= ((tmpByteArr[3] << 24) & 0xFF000000);
		

        return native_icmp_WinPing4(address, timeoutMs);
	}
}
