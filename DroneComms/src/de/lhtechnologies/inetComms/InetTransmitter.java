package de.lhtechnologies.inetComms;

import de.lhtechnologies.SignVerify;

import java.io.IOException;
import java.io.OutputStream;

/**
 * Created by ludger on 28.01.16.
 */
public class InetTransmitter {
    private OutputStream out;
    private SignVerify sv;

    public InetTransmitter(OutputStream out, SignVerify sv) {
        this.out = out;
        this.sv = sv;
    }

    public void transmit(byte[] message) throws Exception {
        String signedMessage = sv.createSignedMessage(message);
        out.write(signedMessage.getBytes());
    }
}
