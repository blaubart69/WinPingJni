package test;

import static org.junit.Assert.*;

import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.UnknownHostException;

import at.spindi.WinPing;
import at.spindi.WinPingResult;
import org.junit.After;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;


public class JniPingSync {

	@Before
	public void WinPingStartup() {
		Assert.assertEquals(0, at.spindi.WinPing.Startup());
	}
	@After
	public void WinPingCleanup() {
		Assert.assertEquals(0, at.spindi.WinPing.Shutdown());
	}
	@Test
	public void testOnePing() throws UnknownHostException {
		
		WinPingResult rc = WinPing.ping((Inet4Address)Inet4Address.getByName("127.0.0.1"), 1000);
		Assert.assertEquals(0, rc.LastError);
	}
	@Test
	public void testTwoPingsWithExtraStartupAndCleanup() throws UnknownHostException {

		WinPingResult rc = WinPing.ping((Inet4Address)Inet4Address.getByName("127.0.0.1"), 1000);
		Assert.assertEquals(0, rc.LastError);

		Assert.assertEquals(0, at.spindi.WinPing.Shutdown());
		Assert.assertEquals(0, at.spindi.WinPing.Startup());

		rc = at.spindi.WinPing.ping((Inet4Address)Inet4Address.getByName("127.0.0.1"), 1000);
		Assert.assertEquals(0, rc.LastError);
	}

	@Test
	public void testOnev6LocalHost() throws UnknownHostException {

		WinPingResult res = WinPing.ping6(
			(Inet6Address) Inet6Address.getByName("::0"),
			(Inet6Address) Inet6Address.getByName("::1"),
			1000);

		Assert.assertEquals(0, res.LastError);
	}
	@Test
	public void testIPv6Google() throws UnknownHostException {

		WinPingResult res = WinPing.ping6(
			(Inet6Address) Inet6Address.getByName("::0"),
			(Inet6Address) Inet6Address.getByName("ipv6.google.com"),
			1000);

		Assert.assertEquals(0, res.LastError);
	}
	@Test
	public void testIPv6GoogleWithoutSourceAddress() throws UnknownHostException {

		WinPingResult res = WinPing.ping(Inet6Address.getByName("ipv6.google.com"),1000);
		Assert.assertEquals(0, res.LastError);
	}
}
