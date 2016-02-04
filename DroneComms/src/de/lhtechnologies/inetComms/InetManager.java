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
    private static int maxRetryCount = 10;
    private static int sleepDuration = 1000;

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
        for(int i = 0; i < maxRetryCount; i++) {
            try {
                connect();
                return;
            } catch (IOException e) {
                e.printStackTrace();
                i++;
                try {
                    Thread.sleep(sleepDuration);
                } catch (InterruptedException e1) {
                    e1.printStackTrace();
                }
            }
        }

        //If we came here, connecting failed 10 times
        System.out.println("Tried resetting for 10 times, giving up!");
        System.exit(-1);
    }

    private void connect() throws IOException {
        if(sock != null) {
            try {
                sock.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        sock = null;

        sock = new Socket(address, port);

        if(receiver == null) {
            receiver = new InetReceiver(sock.getInputStream(), sv);
        } else {
            receiver.newInputStream(sock.getInputStream());
        }

        if(transmitter == null) {
            transmitter = new InetTransmitter(sock.getOutputStream(), sv, this);
        } else {
            transmitter.newOutputStream(sock.getOutputStream());
        }
    }
}
