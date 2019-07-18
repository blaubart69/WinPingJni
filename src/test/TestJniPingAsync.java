package test;

import static org.junit.Assert.*;

import java.net.Inet4Address;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.atomic.AtomicInteger;

import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.BeforeClass;
import org.junit.Test;

import at.spindi.WinPingResult;

public class TestJniPingAsync {

	private class TestResult<T> {
		public T result;
		public TestResult(T ResultToSet) {
			result = ResultToSet;
		}
	}
	@BeforeClass
	public static void runBeforeClass() {
		System.out.println("starting up");
		Assert.assertEquals(0, at.spindi.WinPing.Startup());		
	}
	@AfterClass
	public static void afterClass() {
		System.out.println("cleaning up");
		Assert.assertEquals(0, at.spindi.WinPing.Cleanup());
	}
	@Test
	public void testAsyncLocalhost() throws UnknownHostException, InterruptedException {
		
		Object waitObj = new Object();
		final TestResult<WinPingResult> res = new TestResult<WinPingResult>(null);
		
		at.spindi.WinPing.ping4Async(
				(Inet4Address)Inet4Address.getByName("127.0.0.1"),
				1000,
				(result) -> {
					System.out.printf("callback! %d\n", result.IpStatus);
					synchronized (waitObj) {
						res.result = result;
						waitObj.notify();
					}
					
				});
		
		synchronized (waitObj) {
			waitObj.wait();	
		}
		
		Assert.assertTrue( res.result.RoundTripTime >= 0);
		Assert.assertEquals(0, res.result.IpStatus);
		Assert.assertEquals(0, res.result.LastError);
	}
	@Test
	public void testAsync8888() throws UnknownHostException, InterruptedException {
		
		Object waitObj = new Object();
		final TestResult<WinPingResult> res = new TestResult<WinPingResult>(null);
		
		at.spindi.WinPing.ping4Async(
				(Inet4Address)Inet4Address.getByName("8.8.8.8"),
				1000,
				(result) -> {
					res.result = result;
					synchronized (waitObj) {
						waitObj.notify();
					}
					
				});
		
		synchronized (waitObj) {
			waitObj.wait();	
		}
		
		Assert.assertTrue( res.result.RoundTripTime >= 0);
		Assert.assertEquals(0, res.result.IpStatus);
		Assert.assertEquals(0, res.result.LastError);
		
		System.out.printf("8.8.8.8 roundtrip %d\n", res.result.RoundTripTime);
	}

	@Test
	public void test2hosts() throws UnknownHostException, InterruptedException {
		String[] hosts = new String[] {"8.8.8.8", "mail.utanet.at", "1.1.1.1", "test.schrimpe.de", "llkjdfslögjklösdfkg.sdfgsdfg.sdf"};
		List<WinPingResult> results = doAsyncPing(hosts);
		Assert.assertEquals(hosts.length, results.size());
	}
	public List<WinPingResult> doAsyncPing(final String[] hostnames) throws UnknownHostException, InterruptedException {

		final CountDownLatch numberPings = new CountDownLatch(hostnames.length);
		final List<WinPingResult> results = new ArrayList<>(hostnames.length);
		
		for ( String host : hostnames ) {

			Inet4Address ipv4;
			try {
				ipv4 = (Inet4Address) Inet4Address.getByName(host);
			}
			catch (Exception ex) {
				System.out.printf("X: %s - %s\n",host, ex.toString());
				results.add(new WinPingResult(-1,-1,-1));
				numberPings.countDown();
				continue;
			}

			System.out.println("->" + ipv4.toString());
			at.spindi.WinPing.ping4Async(
				ipv4,
				1000,
				(winPingResult) -> {
					System.out.printf("<- %4dms %s\n", winPingResult.RoundTripTime, ipv4.toString());
					results.add(winPingResult);
					numberPings.countDown();
				});
		}

		numberPings.await();
		
		return results;
	}
}
