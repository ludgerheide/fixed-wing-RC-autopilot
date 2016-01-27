package de.lhtechnologies.flightComputer;

import de.lhtechnologies.CommunicationProtocol;

import java.io.IOException;
import java.io.OutputStream;
import java.util.Arrays;

import static de.lhtechnologies.CommunicationProtocol.*;

/**
 * Created by ludger on 18.01.16.
 */
public class SerialTransmitter {
    public OutputStream out;

    public SerialTransmitter(OutputStream out) {
        this.out = out;
    }

    public void transmit(DroneMessage msg) throws IOException {
        //TODO: Validate message before sending it out
        byte[] compiledMessageAndChecksum = Arrays.copyOf(msg.toByteArray() , msg.toByteArray().length + 1);
        compiledMessageAndChecksum[compiledMessageAndChecksum.length] = (byte) SerialPortManager.calculateChecksum(msg.toByteArray().length, compiledMessageAndChecksum);

        out.write(compiledMessageAndChecksum);
    }
}
