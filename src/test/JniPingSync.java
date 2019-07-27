package test;

import static org.junit.Assert.*;

import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.UnknownHostException;

import at.spindi.WinPing;
import at.spindi.WinPingResult;
import org.junit.Assert;
import org.junit.Test;


public class JniPingSync {

	@Test
	public void testOnePing() throws UnknownHostException {
		
		Assert.assertEquals(0, at.spindi.WinPing.Startup());
		
		int rc = at.spindi.WinPing.ping4((Inet4Address)Inet4Address.getByName("127.0.0.1"), 1000);
		Assert.assertEquals(0, rc);
		
		Assert.assertEquals(0, at.spindi.WinPing.Cleanup());
	}
	@Test
	public void testTwoPingsWithExtraStartupAndCleanup() throws UnknownHostException {

		Assert.assertEquals(0, at.spindi.WinPing.Startup());

		int rc = at.spindi.WinPing.ping4((Inet4Address)Inet4Address.getByName("127.0.0.1"), 1000);
		Assert.assertEquals(0, rc);

		Assert.assertEquals(0, at.spindi.WinPing.Cleanup());

		Assert.assertEquals(0, at.spindi.WinPing.Startup());

		rc = at.spindi.WinPing.ping4((Inet4Address)Inet4Address.getByName("127.0.0.1"), 1000);
		Assert.assertEquals(0, rc);

		Assert.assertEquals(0, at.spindi.WinPing.Cleanup());

	}

	@Test
	public void testOnev6LocalHost() throws UnknownHostException {

		Assert.assertEquals(0, at.spindi.WinPing.Startup());

		WinPingResult res = WinPing.ping6(
			(Inet6Address) Inet6Address.getByName("::0"),
			(Inet6Address) Inet6Address.getByName("::1"),
			1000);

		Assert.assertEquals(0, res.LastError);

		Assert.assertEquals(0, at.spindi.WinPing.Cleanup());
	}
	@Test
	public void testIPv6Google() throws UnknownHostException {

		Assert.assertEquals(0, at.spindi.WinPing.Startup());

		WinPingResult res = WinPing.ping6(
			(Inet6Address) Inet6Address.getByName("::0"),
			(Inet6Address) Inet6Address.getByName("ipv6.google.com"),
			1000);

		Assert.assertEquals(0, res.LastError);

		Assert.assertEquals(0, at.spindi.WinPing.Cleanup());
	}
	@Test
	public void testIPv6GoogleWithoutSourceAddress() throws UnknownHostException {

		Assert.assertEquals(0, at.spindi.WinPing.Startup());

		WinPingResult res = WinPing.ping6((Inet6Address) Inet6Address.getByName("ipv6.google.com"),1000);
		Assert.assertEquals(0, res.LastError);

		Assert.assertEquals(0, at.spindi.WinPing.Cleanup());

	}

}
