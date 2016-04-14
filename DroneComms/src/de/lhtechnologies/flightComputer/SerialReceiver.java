package de.lhtechnologies.flightComputer;

import com.google.protobuf.InvalidProtocolBufferException;
import gnu.io.*;

import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;
import java.util.Observable;

import static de.lhtechnologies.CommunicationProtocol.*;

/**
 * Created by ludger on 18.01.16.
 */
public class SerialReceiver extends Observable implements Runnable {
    public static byte[] startMarker = "start".getBytes();

    InputStream in;

    protected byte[] inputBuffer = new byte[257];
    protected int payloadLength = 0;
    private int inByte = 0;
    private int index = 0;

    private int receivedMsg = 0;
    long millis = 0;

    public SerialReceiver(InputStream in) {
        this.in = in;
    }

    @Override
    public void run() {
        try {
            while (true) {
                //if the index is 0, read byte-by-byte until we have index and length
                if(index < 5) {
                    inByte = in.read();
                    if (inByte == startMarker[index]) {
                        index++;
                    } else {
                        index = 0;
                    }
                }
                if(index == 5) {
                    inByte = in.read();
                    if(inByte == -1) {
                        index = 0;
                    } else {
                        payloadLength = inByte;
                        index++;
                    }
                }
                if(index >= (startMarker.length + 1)) {
                    //We are in the message body, wait for the avalable size to be correct
                    if(in.available() >= payloadLength + 1) {
                        index = 0;

                        int read = in.read(inputBuffer, 0, payloadLength + 1);
                        assert(read == payloadLength + 1);

                        int receivedChecksum = inputBuffer[payloadLength];
                        int calculatedChecksum = calculateChecksum();

                        //Reception complete. Verify checksum, create Protobuf object and notify the observers
                        if (receivedChecksum == calculatedChecksum) {
                            byte[] rawMessage = Arrays.copyOfRange(inputBuffer, 0, payloadLength);
                            try {
                                System.out.format("complete msg %d, Backlog %d, time since last %d %n", receivedMsg++, in.available(), System.currentTimeMillis() - millis);
                                millis = System.currentTimeMillis();
                                DroneMessage msg = DroneMessage.parseFrom(rawMessage);
                                setChanged();
                                notifyObservers(msg);
                            } catch (InvalidProtocolBufferException e) {
                                System.out.println("WARNING: Invalid protobuf received!");
                            }
                        }
                    } else {
                        try {
                            Thread.sleep(1);
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                }
            }
        } catch (IOException e) {
            System.out.println("Error encountered. Exiting…");
            e.printStackTrace();
            System.exit(-1);
        }
    }

    public int calculateChecksum() {
        return SerialPortManager.calculateChecksum(payloadLength, inputBuffer);
    }
}
