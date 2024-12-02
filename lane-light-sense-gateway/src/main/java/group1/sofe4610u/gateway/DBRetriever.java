package group1.sofe4610u.gateway;

import java.net.DatagramPacket;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.HashMap;

public class DBRetriever extends Thread {
    private Connection dbConnection;
    private static ArrayList<String> targetMACs = new ArrayList<>();
    private int sleep;
    private String timescale;

    public DBRetriever(Connection dbConnection, int sleep, String timescale) {
        this.dbConnection = dbConnection;
        this.sleep = sleep;
        this.timescale = timescale;
    }

    @Override
    public void run() {
        while (true) {
            // this section updates the target MACs
            try {
                Statement statement = dbConnection.createStatement();
                ResultSet results = statement.executeQuery("SELECT `mac` FROM `target_macs`;");
                targetMACs.clear();
                while (results.next()) {
                    targetMACs.add(results.getString(1));
                }
            } catch (SQLException e) {
                System.err.println("Failed to retrieve target MACs: " + e.getMessage());
                e.printStackTrace();
            }

            // this section sends signals to the sections
            HashMap<Integer, Boolean> status = new HashMap<>();
            for (Integer stationID : Gateway.clientAddresses.keySet()) {
                status.put(stationID, false);
            }
            for (String mac : targetMACs) {
                try {
                    PreparedStatement statement = dbConnection.prepareStatement("SELECT `station_id`" + 
                            "FROM `signal_log` " +
                            "WHERE `mac` = ? AND `time` >= NOW() - INTERVAL ? SECOND " +
                            "ORDER BY `rssi` DESC " +
                            "LIMIT 1;");
                    statement.setString(1, mac);
                    statement.setString(2, timescale);
                    ResultSet results = statement.executeQuery();
                    results.next();
                    int stationID = results.getInt(1);
                    status.replace(stationID, true);
                } catch (SQLException e) {
                    System.err.println("Failed to retrieve signal readings, not updating lighting: " + e.getMessage());
                    e.printStackTrace();
                }
            }
            for (Integer stationID : status.keySet()) {
                try {
                    StringBuilder msg = new StringBuilder();
                    msg.append(stationID);
                    msg.append(",");
                    msg.append((status.get(stationID)) ? "1" : "0");
                    msg.append(";");
                    byte[] buffer = msg.toString().getBytes();

                    DatagramPacket send_pdu = new DatagramPacket(buffer, buffer.length, Gateway.clientAddresses.get(stationID), Gateway.clientPorts.get(stationID));
                    Gateway.socket.send(send_pdu);
                } catch (Exception e) {
                    System.err.println("Something went wrong sending command to station: " + e.getMessage());
                    e.printStackTrace();
                }
            }

            // sleep for some time
            try {
                Thread.sleep(sleep * 1000l);
            } catch (InterruptedException e) {
                System.err.println("DBRetriever sleep interrupted, continuing: " + e.getMessage());
                e.printStackTrace();
            }
        }
    }
}
