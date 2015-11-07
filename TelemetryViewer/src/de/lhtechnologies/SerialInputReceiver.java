package de.lhtechnologies;

import com.google.protobuf.InvalidProtocolBufferException;
import gnu.io.CommPortIdentifier;
import gnu.io.SerialPort;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.util.Arrays;

/**
 * Created by ludger on 06.11.15.
 */
public class SerialInputReceiver extends InputReceiver {
    public static int baudRate = 115200;
    public static int dataBits = SerialPort.DATABITS_8;
    public static int stopBits = SerialPort.STOPBITS_1;
    public static int parity = SerialPort.PARITY_NONE;

    private InputStream inStream;
    private ByteBuffer inputBuffer;
    private int datalength, invalidMessageCount;

    public SerialInputReceiver(String serialPort) throws Exception {
        CommPortIdentifier portId = CommPortIdentifier.getPortIdentifier(serialPort);
        SerialPort thePort = (SerialPort) portId.open("TelemetryReceiver", 5000);
        thePort.setSerialPortParams(baudRate, dataBits, stopBits, parity);
        thePort.setFlowControlMode(SerialPort.FLOWCONTROL_NONE);

        inStream = thePort.getInputStream();
        inputBuffer = ByteBuffer.allocate(1024*32);
        datalength = 0;
    }

    @Override
    public void startReceiving() {
        while (true) {
            try {
                while (inStream.available() > 0 && inputBuffer.position() < inputBuffer.limit()) {
                    int bytesToRead = inStream.available();
                    if(bytesToRead > (inputBuffer.limit() - inputBuffer.position())) {
                        bytesToRead = inputBuffer.limit() - inputBuffer.position();
                    }
                    byte[] inBytes = new byte[bytesToRead];
                    inStream.read(inBytes);
                    inputBuffer.put(inBytes);
                    datalength+=bytesToRead;
                }
                findMessage();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public void findMessage() {
        if(datalength > 6) {
            //Check if the first five bytes are the start marker
            //If not dump the first byte and recheck
            boolean haveStart = false;
            byte[] startString = InputReceiver.startMarker.getBytes();
            inputBuffer.position(0);
            do {
                byte[] firstFive = new byte[5];
                inputBuffer.get(firstFive);

                if (Arrays.equals(firstFive, startString)) {
                    haveStart = true;
                    inputBuffer.position(inputBuffer.position() - 5);
                } else {
                    inputBuffer.position(inputBuffer.position() - 4);
                    datalength--;
                }
            } while (!haveStart && datalength > 6);
            inputBuffer.compact();

            //Now get the length (comes after the start sequence)
            //If we have the full packet in our buffer process it
            //Else return
            int length = inputBuffer.get(5) & 0xFF;
            if (datalength >= length + 5 + 1 + 1) {
                //Get the data
                inputBuffer.position(6);
                byte[] payload = new byte[length];
                inputBuffer.get(payload);

                try {
                    super.processMessage(payload);
                } catch (InvalidProtocolBufferException e) {
                    System.out.format("Invalid message %d%n", invalidMessageCount++);
                    length = -6;
                }

                //Reduce the byte buffer
                inputBuffer.position(length + 5 + 1 + 1);
                datalength -= length + 5 + 1 + 1;
                inputBuffer.compact();
            } else {
                inputBuffer.position(datalength);
                return;
            }
        }

        if(datalength > 6) {
            findMessage();
        } else {
            inputBuffer.position(datalength);
            return;
        }
    }
}
