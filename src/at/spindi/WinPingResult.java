package at.spindi;

public class WinPingResult {
	public int LastError;
	public int IpStatus;
	public int RoundTripTime;
	
	public WinPingResult(int lastError, int ipStatus, int roundTripTime) {
		super();
		LastError = lastError;
		IpStatus = ipStatus;
		RoundTripTime = roundTripTime;
	}
}
