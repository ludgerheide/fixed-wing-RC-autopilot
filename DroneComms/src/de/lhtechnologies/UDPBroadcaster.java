package de.lhtechnologies;

import de.lhtechnologies.flightComputer.SerialReceiver;

import java.io.IOException;
import java.net.*;
import java.util.Enumeration;
import java.util.Observable;
import java.util.Observer;

import static de.lhtechnologies.CommunicationProtocol.*;

/**
 * Created by ludger on 10.04.16.
 */
public class UDPBroadcaster implements Observer {
    private InetAddress broadcastAddress;
    private DatagramSocket sock;
    private final int udpPort = 5050;

    public UDPBroadcaster() {
        try {
            //Get the broadcast address
            Enumeration<NetworkInterface> interfaces = NetworkInterface.getNetworkInterfaces();
            while (interfaces.hasMoreElements()) {
                NetworkInterface networkInterface = interfaces.nextElement();
                if (networkInterface.isLoopback())
                    continue;    // Don't want to broadcast to the loopback interface
                for (InterfaceAddress interfaceAddress :
                        networkInterface.getInterfaceAddresses()) {
                    InetAddress broadcast = interfaceAddress.getBroadcast();
                    if (broadcast == null)
                        continue;
                    this.broadcastAddress = broadcast;
                }
            }
        } catch (SocketException e) {
            System.out.println("This should never happen, exiting…");
            e.printStackTrace();
            System.exit(-1);
        }
        System.out.println("Broadcast address : " + broadcastAddress);

        try {
            sock = new DatagramSocket();
        } catch (SocketException e) {
            System.out.println("This should never happen, exiting…");
            e.printStackTrace();
            System.exit(-1);
        }
    }

    @Override
    public void update(Observable o, Object arg) {
        assert(o instanceof SerialReceiver);
        assert(arg instanceof DroneMessage);

        DroneMessage msg = (DroneMessage) arg;

        DatagramPacket packet = new DatagramPacket(msg.toByteArray(), msg.toByteArray().length, broadcastAddress, 5050);
        try {
            sock.send(packet);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
