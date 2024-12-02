package group1.sofe4610u.gateway;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.util.concurrent.ConcurrentLinkedQueue;

public class DBSubmitter extends Thread {
    private Connection dbConnection;
    public static ConcurrentLinkedQueue<StationMacRSSI> signalQueue = new ConcurrentLinkedQueue<>();

    public DBSubmitter(Connection dbConnection) {
        this.dbConnection = dbConnection;
    }

    @Override
    public void run() {
        while (true) {
            StationMacRSSI next = signalQueue.poll();
            if (next != null) {
                try {
                    PreparedStatement statement = dbConnection.prepareStatement("INSERT INTO `signal_log`(`station_id`,`mac`,`rssi`) values (?, ?, ?);");
                    statement.setInt(1, next.stationID);
                    statement.setString(2, next.mac);
                    statement.setInt(3, next.rssi);
                    statement.executeUpdate();
                } catch (SQLException e) {
                    System.err.println("Failed to submit to signal log db: " + e.getMessage());
                    e.printStackTrace();
                }

            }
        }
    }
}
