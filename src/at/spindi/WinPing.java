package at.spindi;

import java.net.Inet4Address;
import java.util.function.Consumer;

public class WinPing {
	
	static {
	      System.loadLibrary("WinPingJni"); // Load native library at runtime
	                                   // hello.dll (Windows) or libhello.so (Unixes)
	}
	
	private static native int native_WinPing_Startup();
	private static native int native_WinPing_Cleanup();
	
	private static native int native_icmp_WinPing4     (final int IpAdress, final int timeout); 
	private static native int native_icmp_WinPing4Async(final int IpAdress, final int timeout, final Consumer<WinPingResult> callback);
	
	public static int ping4(final Inet4Address v4ToPing, final int timeoutMs) {
        return native_icmp_WinPing4(
        		IPv4ToNetworkByteOrder(v4ToPing), 
        		timeoutMs);
	}
	
	public static int ping4Async(final Inet4Address v4ToPing, final int timeoutMs, final Consumer<WinPingResult> callback) {
        return native_icmp_WinPing4Async(
        		IPv4ToNetworkByteOrder(v4ToPing), 
        		timeoutMs,
        		callback);
	}
	
	private static int IPv4ToNetworkByteOrder(final Inet4Address ipv4) {
		final byte[] tmpByteArr = ipv4.getAddress();
		
		int 
		address  =   tmpByteArr[0] 	      & 0xFF;
        address |= ((tmpByteArr[1] <<  8) & 0xFF00);
        address |= ((tmpByteArr[2] << 16) & 0xFF0000);
        address |= ((tmpByteArr[3] << 24) & 0xFF000000);
        
        return address;
	}
}
