package de.lhtechnologies.inetComms;

import de.lhtechnologies.SignVerify;

import java.io.IOException;
import java.io.OutputStream;
import java.net.SocketException;

/**
 * Created by ludger on 28.01.16.
 */
public class InetTransmitter {
    private OutputStream out;
    private SignVerify sv;
    private InetManager manager;

    public InetTransmitter(OutputStream out, SignVerify sv, InetManager manager) {
        this.out = out;
        this.sv = sv;
        this.manager = manager;
    }

    public void newOutputStream(OutputStream out) {
        this.out = out;
    }

    public void transmit(byte[] message) {
        try {
            String signedMessage = sv.createSignedMessage(message);
            out.write(signedMessage.getBytes());
        } catch (SocketException e) {
            manager.resetConnection();
        } catch (Exception e) {

        }
    }
}
