package at.spindi;

import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.CountDownLatch;

public class RunPing {
    public static void main(String[] args) {

        Stats stats = new Stats();

        new Thread( () -> {
            while (true) {
                System.out.printf("lastError startup/shutdown ping4 sync/async, ping6 sync/async\t%4d | %5d/%5d | %5d/%5d | %5d/%5d\n",
                    stats.lastError,
                    stats.startup, stats.shutdown,
                    stats.sentPing4sync, stats.sentPing4async,
                    stats.sentPing6sync, stats.sentPing6async);

                stats.lastError = 0;

                try {
                    Thread.sleep(2000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }).start();

        List<String> ipv4sync = Arrays.asList("192.168.178.240","192.168.178.200");
        List<String> ipv4async = new ArrayList<>(254);
        for (int i=1; i<255;++i) {
            ipv4async.add("192.168.178." + i);
        }

        while(true) {

            at.spindi.WinPing.Startup();
            stats.startup++;
            doPings(stats, ipv4sync, ipv4async);
            at.spindi.WinPing.Shutdown();
            stats.shutdown++;

            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
    static void doPings(Stats stats, List<String> ipv4sync, List<String> ipv4async) {
        try {
            ping4(stats, ipv4sync);
            doAsyncPing(stats, ipv4async);
        } catch (UnknownHostException | InterruptedException e) {
            e.printStackTrace();
        }
    }
    static void ping4(Stats stats, List<String> ipv4) throws UnknownHostException {
        for ( String ip : ipv4) {
            WinPingResult result = WinPing.ping(InetAddress.getByName(ip), 300);
            stats.sentPing4sync++;
            if (result.LastError != 0) {
                stats.lastError = result.LastError;
            }
        }
    }
    static void doAsyncPing(Stats stats, final List<String> hostnames) throws UnknownHostException, InterruptedException {

        final CountDownLatch numberPings = new CountDownLatch(hostnames.size());

        for ( String host : hostnames ) {

            Inet4Address ipv4;
            try {
                ipv4 = (Inet4Address) Inet4Address.getByName(host);
            }
            catch (Exception ex) {
                System.out.printf("X: %s - %s\n",host, ex.toString());
                numberPings.countDown();
                continue;
            }

            at.spindi.WinPing.ping4Async(
                ipv4,
                1000,
                (winPingResult) -> {
                    /*
                    System.out.printf("<- LastErr: %5d, status: %5d, %4dms %s\n",
                        winPingResult.LastError,
                        winPingResult.IpStatus,
                        winPingResult.RoundTripTime,
                        ipv4.toString());

                     */
                    numberPings.countDown();
                    stats.sentPing4async++;
                });
        }

        numberPings.await();
    }
}
