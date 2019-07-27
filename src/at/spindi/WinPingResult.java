package at.spindi;

public class WinPingResult {
	public final int  LastError;
	public final long IpStatus;
	public final int  RoundTripTime;
	
	public WinPingResult(int lastError, long ipStatus, int roundTripTime) {
		super();
		LastError = lastError;
		IpStatus = ipStatus;
		RoundTripTime = roundTripTime;
	}
}
