package group1.sofe4610u.gateway;

public class StationMacRSSI {
    public final int stationID;
    public final String mac;
    public final int rssi;

    public StationMacRSSI(int stationID, String mac, int rssi) {
        this.stationID = stationID;
        this.mac = mac;
        this.rssi = rssi;
    }
}
