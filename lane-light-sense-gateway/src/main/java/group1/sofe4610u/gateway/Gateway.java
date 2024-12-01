package group1.sofe4610u.gateway;

import java.util.Properties;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.io.FileInputStream;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

public class Gateway {

    public static DatagramSocket socket;
    public static int port;
    public static int bufferSize;
    public static ConcurrentHashMap<Integer, InetAddress> clientAddresses = new ConcurrentHashMap<>();
    public static ConcurrentHashMap<Integer, Integer> clientPorts = new ConcurrentHashMap<>();
    public static ConcurrentLinkedQueue<StationMacRSSI> signalQueue = new ConcurrentLinkedQueue<>();
    public static DBRetriever dbRetriever;
    public static DBSubmitter dbSubmitter;

    public static void main(String[] args) {
        Properties properties = new Properties();
        try {
            properties.load(new FileInputStream("./lane-light-sense-gateway.conf"));
        } catch (IOException e) {
            System.err.println("Failed to read config file, exiting: " + e.getMessage());
            e.printStackTrace();
            System.exit(1);
        }
        port = Integer.parseInt(properties.getProperty("port", "5683"));
        bufferSize = Integer.parseInt(properties.getProperty("bufferSize", "1024"));
        try {
            Class.forName("com.mysql.cj.jdbc.Driver");
            Connection dbConnection = DriverManager.getConnection(properties.getProperty("dbURI", "jdbc:mysql://localhost:3306"), properties.getProperty("dbUser"), properties.getProperty("dbPass"));
            dbRetriever = new DBRetriever(dbConnection, Integer.parseInt(properties.getProperty("dbRetrieverSleep", "60")));
            dbConnection = DriverManager.getConnection(properties.getProperty("dbURI", "jdbc:mysql://localhost:3306"), properties.getProperty("dbUser"), properties.getProperty("dbPass"));
            dbSubmitter = new DBSubmitter(dbConnection);
            dbRetriever.start();
            dbSubmitter.start();
        } catch (ClassNotFoundException | SQLException e) {
            System.err.println("Failure in MySQL database connection: " + e.getMessage());
            e.printStackTrace();
            System.exit(2);
        }
        try {
            socket = new DatagramSocket(port);
            while (true) {
                byte[] buffer = new byte[bufferSize]; // this is larger than the 
                DatagramPacket pdu = new DatagramPacket(buffer, bufferSize);
                socket.receive(pdu);
                ClientHandler clientHandler = new ClientHandler(pdu);
                clientHandler.start();
            }
        } catch (IOException e) {
            System.err.println("Failure in creating/receiving pdu: " + e.getMessage());
            e.printStackTrace();
        }
    }
}
