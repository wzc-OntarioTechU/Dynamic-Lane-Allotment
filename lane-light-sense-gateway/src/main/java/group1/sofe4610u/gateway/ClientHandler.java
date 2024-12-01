package group1.sofe4610u.gateway;

import java.net.DatagramPacket;
import java.net.InetAddress;

public class ClientHandler extends Thread {
    private final DatagramPacket pdu;
    private final InetAddress clientAddress;
    private final int clientPort;

    public ClientHandler(DatagramPacket pdu) {
        this.pdu = pdu;
        this.clientAddress = pdu.getAddress();
        this.clientPort = pdu.getPort();
    }

    @Override
    public void run() {
        String data = new String(pdu.getData(), 0, pdu.getLength());
        String[] parts = data.split(";,");
        int stationID = 0;
        try {
            stationID = Integer.parseInt(parts[0]);
        } catch (NumberFormatException | IndexOutOfBoundsException e) {
            System.err.println("Failed to parse station ID of incoming pdu: " + e.getMessage());
            e.printStackTrace();
            return;
        }
        updateStationAddr(stationID, clientAddress, clientPort);
        for (int i = 1; i < parts.length; i += 2) {
            try {
                DBSubmitter.signalQueue.add(new StationMacRSSI(stationID, parts[i], Integer.parseInt(parts[i + 1])));
            } catch (NumberFormatException | IndexOutOfBoundsException e) {
                System.err.println("Failed to parse mac and rssi from incoming pdu, continuing: " + e.getMessage());
                e.printStackTrace();
            }
        }
    }

    private void updateStationAddr(int stationID, InetAddress address, int port) {
        Gateway.clientAddresses.put(stationID, address);
        Gateway.clientPorts.put(stationID, port);
    }
}
