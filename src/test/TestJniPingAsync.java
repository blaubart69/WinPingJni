package test;

import static org.junit.Assert.*;

import java.net.Inet4Address;
import java.net.UnknownHostException;

import org.junit.Test;

import at.spindi.WinPingResult;

public class TestJniPingAsync {

	@Test
	public void testSimpleAsync() throws UnknownHostException {
		
		WinPingResult res = null;
		final Object waitObj = new Object();
		boolean success = false;
		
		at.spindi.WinPing.ping4Async(
				(Inet4Address)Inet4Address.getByName("127.0.0.1"),
				1000,
				(result) -> {
					System.out.printf("callback!");
				});
		
	}

}
