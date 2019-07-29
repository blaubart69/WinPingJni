package at.spindi;

import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.util.function.Consumer;

public class WinPing {
	
	static {
	      System.loadLibrary("WinPingJni"); // Load native library at runtime
	                                   // hello.dll (Windows) or libhello.so (Unixes)
	}
	
	private static native int native_WinPing_Startup();
	private static native int native_WinPing_Shutdown();

	private static native WinPingResult native_icmp_WinPing4     (final int IpAdress, final int timeout);
	private static native int 			native_icmp_WinPing4Async(final int IpAdress, final int timeout, final Consumer<WinPingResult> callback);

	private static native WinPingResult native_icmp_WinPing6     (final byte[] SourceAddress, final byte[] DestinationAddress, final int timeout);

	// ---------------------------------------------------------------------------

	private static final byte[] IPv6ZERO = new byte[16];

	public static int Startup() {
		return native_WinPing_Startup();
	}
	public static int Shutdown() {
		return native_WinPing_Shutdown();
	}

	public static WinPingResult ping(final InetAddress ip, final int timeoutMs) throws UnsupportedOperationException {

		WinPingResult result;

		if ( ip instanceof Inet4Address ) {
			result = native_icmp_WinPing4(
				IPv4ToNetworkByteOrder( (Inet4Address)ip ),
				timeoutMs);
		}
		else if ( ip instanceof Inet6Address ) {
			result = native_icmp_WinPing6(
				IPv6ZERO,
				ip.getAddress(),
				timeoutMs);
		}
		else {
			throw new UnsupportedOperationException("only IPv4 and IPv6 addresses are supported");
		}

		return result;
	}

	public static WinPingResult ping6(final Inet6Address SourceAddress, final Inet6Address DestinationAddress, int timeoutMs) {
		return native_icmp_WinPing6(
			SourceAddress.getAddress(),
			DestinationAddress.getAddress(),
			timeoutMs);
	}

	public static int ping4Async(final Inet4Address v4ToPing, final int timeoutMs, final Consumer<WinPingResult> callback) {
		return native_icmp_WinPing4Async(
			IPv4ToNetworkByteOrder(v4ToPing),
			timeoutMs,
			callback);
	}

	// ---------------------------------------------------------------------------

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
