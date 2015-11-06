package de.lhtechnologies;

import com.google.protobuf.InvalidProtocolBufferException;

import java.io.FileOutputStream;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.nio.charset.Charset;

import static de.lhtechnologies.CommunicationProtocol.*;

/**
 * Created by ludger on 06.11.15.
 */
public abstract class InputReceiver {
    public static String startMarker = "start";

    public FileOutputStream outFile;
    public DatagramSocket outSocket;
    public String outAddress;
    public int outPort;

    public abstract void startReceiving();

    public void processMessage(byte[] protobuf) {
        DroneMessage msg;

        try {
            msg = DroneMessage.parseFrom(protobuf);

            if(outFile != null) {
                //Write start to the ouput file
                try {
                    outFile.write(startMarker.getBytes());
                    outFile.write(protobuf);
                    outFile.write(InputReceiver.calculateChecksum(protobuf));
                    outFile.flush();
                    outFile.getFD().sync();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

            if(outSocket != null) {
                //Send out over network
                InetAddress address = null;
                try {
                    address = InetAddress.getByName(outAddress);
                } catch (UnknownHostException e) {
                    e.printStackTrace();
                }
                DatagramPacket packet = new DatagramPacket(protobuf, protobuf.length, address, outPort);
                try {
                    outSocket.send(packet);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

            //Write to display
            System.out.println(msg);
        } catch (InvalidProtocolBufferException e) {
            e.printStackTrace();
        }
    }

    public static byte[] calculateChecksum (byte[] input) {
        long checksum = 0;
        for(int i = 0; i < input.length; i++) {
            checksum += input[i] & 0xFF; //To see the unsigned value
        }
        byte[] out = new byte[1];
        out[0] = (byte)checksum;
        return out;
    }
}
