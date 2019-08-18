package at.spindi;

import java.net.InetAddress;
import java.net.UnknownHostException;

public class RunPing {
    public static void main(String[] args) {

        while(true) {

            System.out.println("startup");
            at.spindi.WinPing.Startup();
            System.out.println("running pings");
            doPings();
            System.out.println("shutdown");
            at.spindi.WinPing.Shutdown();

            try {
                Thread.sleep(3000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
    static void doPings() {
        try {
            ping4();
        } catch (UnknownHostException e) {
            e.printStackTrace();
        }
    }
    static void ping4() throws UnknownHostException {
        at.spindi.WinPing.ping(InetAddress.getByName("192.168.178.240"),1000);
        at.spindi.WinPing.ping(InetAddress.getByName("192.168.178.200"),1000);
    }
}
