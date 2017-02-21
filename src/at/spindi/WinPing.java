package at.spindi;

import java.net.Inet4Address;
import java.nio.ByteOrder;
import java.util.function.Consumer;

public class WinPing {
	
	static {
	      System.loadLibrary("WinPingJni"); // Load native library at runtime
	                                   		// hello.dll (Windows) or libhello.so (Unixes)
	   }
	
	private static native int native_icmp_WinPing4(final int IpAdress, final int timeout); 
	private static native int native_icmp_WinPing4Async(final int IpAddress, final int timeoutMs, final Consumer<WinPingResult> callback);
	
	public static int ping4(Inet4Address v4ToPing, int timeoutMs) {

        return native_icmp_WinPing4(
        			makeBigEndian(v4ToPing), 
        			timeoutMs);
	}
	
	public static void ping4Async(final Inet4Address v4ToPing, int timeoutMs, final Consumer<WinPingResult> onCompleted) {
		native_icmp_WinPing4Async(
				makeBigEndian(v4ToPing),
				timeoutMs,
				onCompleted);
	}
	
	private static int makeBigEndian(Inet4Address v4Address) {
		
		final byte[] tmpByteArr = v4Address.getAddress();
		
		int 
		BigEndianAddress  =   tmpByteArr[0] 	   & 0xFF;
        BigEndianAddress |= ((tmpByteArr[1] <<  8) & 0xFF00);
        BigEndianAddress |= ((tmpByteArr[2] << 16) & 0xFF0000);
        BigEndianAddress |= ((tmpByteArr[3] << 24) & 0xFF000000);
        
        return BigEndianAddress;
	}
	
	public static int htonl(int value) {
		if (ByteOrder.nativeOrder().equals(ByteOrder.BIG_ENDIAN)) {
			return value;
		}
		return Integer.reverseBytes(value);
	}
}
