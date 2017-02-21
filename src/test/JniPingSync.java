package test;

import static org.junit.Assert.*;

import java.net.Inet4Address;
import java.net.UnknownHostException;

import org.junit.Assert;
import org.junit.Test;


public class JniPingSync {

	@Test
	public void test() throws UnknownHostException {
		int rc = at.spindi.WinPing.ping4((Inet4Address)Inet4Address.getByName("127.0.0.1"), 1000);
		Assert.assertEquals(0, rc);
	}

}
