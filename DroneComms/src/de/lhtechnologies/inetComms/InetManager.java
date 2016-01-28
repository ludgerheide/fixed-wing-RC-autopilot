package de.lhtechnologies.inetComms;

import de.lhtechnologies.SignVerify;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.Socket;

/**
 * Created by ludger on 28.01.16.
 */
public class InetManager implements Runnable {
    public InetReceiver receiver;
    public InetTransmitter transmitter;

    private Socket sock;
    private SignVerify sv;

    private static String address = "192.168.15.10";
    private static int port = 5050;

    public InetManager() throws Exception {
        sv = new SignVerify();

        resetConnection();
    }

    @Override
    public void run() {
        while (true) {
            receiver.run();
            resetConnection();
        }
    }

    public void resetConnection() {
        receiver = null;
        transmitter = null;

        if(sock != null) {
            try {
                sock.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        sock = null;

        try {
            sock = new Socket(address, port);
            receiver = new InetReceiver(sock.getInputStream(), sv);
            transmitter = new InetTransmitter(sock.getOutputStream(), sv);
        } catch (IOException e) {
            System.out.println("Resetting connection failed!");
            e.printStackTrace();
            System.exit(-1);
        }
    }
}
