package de.lhtechnologies.flightComputer;

import java.io.IOException;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;

/**
 * Created by ludger on 19.03.16.
 */
public class MockSerialPortManager extends SerialPortManager {
    public MockSerialPortManager() throws IOException {
        PipedInputStream in = new PipedInputStream();
        final PipedOutputStream out = new PipedOutputStream(in);

        serialReceiver = new MockSerialReceiver(in);
        serialTransmitter = new SerialTransmitter(out);

        MockSerialReceiver mock =  (MockSerialReceiver) serialReceiver;
        new Thread(mock).start();
    }
}
