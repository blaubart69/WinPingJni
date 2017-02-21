package test;

import static org.junit.Assert.*;

import java.net.Inet4Address;
import java.net.UnknownHostException;

import org.junit.Assert;
import org.junit.Test;

import at.spindi.WinPingResult;

public class TestJniPingAsync {

	private class TestResult<T> {
		public T result;
	}
	
	@Test
	public void testSimpleAsync() throws UnknownHostException {
		
		TestResult<WinPingResult> res = new TestResult<>();
		
		at.spindi.WinPing.ping4Async(
				(Inet4Address)Inet4Address.getByName("127.0.0.1"),
				1000,
				(result) -> {
					//System.out.printf("callback!");
					res.result = result;
				});
		Assert.assertNotNull(res.result);
		Assert.assertEquals(123, res.result.RoundTripTime);	
	}

}
