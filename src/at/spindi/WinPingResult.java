package at.spindi;

public class WinPingResult {
	public final int LastError;
	public final int IpStatus;
	public final int RoundTripTime;
	
	public WinPingResult(int lastError, int ipStatus, int roundTripTime) {
		super();
		LastError = lastError;
		IpStatus = ipStatus;
		RoundTripTime = roundTripTime;
	}
}
