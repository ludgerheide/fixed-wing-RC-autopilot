package de.lhtechnologies.flightComputer;

import gnu.io.CommPortIdentifier;
import gnu.io.SerialPort;

/**
 * Created by ludger on 18.01.16.
 */
public class SerialPortManager {
    private static int baudRate = 115200;
    private static int dataBits = SerialPort.DATABITS_8;
    private static int stopBits = SerialPort.STOPBITS_1;
    private static int parity = SerialPort.PARITY_NONE;
    private static String deviceFile = "/dev/ttyAMA0";

    public SerialReceiver serialReceiver;
    public SerialTransmitter serialTransmitter;

    public SerialPortManager() {
        try {
            CommPortIdentifier portId = CommPortIdentifier.getPortIdentifier(SerialPortManager.deviceFile);
            if (portId.isCurrentlyOwned()) {
                System.out.println("Error: Port is currently in use");
                System.exit(-1);
            }
            SerialPort thePort = (SerialPort) portId.open("TelemetryReceiver", 5000);
            thePort.setSerialPortParams(baudRate, dataBits, stopBits, parity);
            thePort.setFlowControlMode(SerialPort.FLOWCONTROL_NONE);

            serialReceiver = new SerialReceiver(thePort.getInputStream());
            thePort.addEventListener(serialReceiver);
            thePort.notifyOnDataAvailable(true);

            serialTransmitter = new SerialTransmitter(thePort.getOutputStream());
        } catch (Exception e) {
            System.out.println("Error encountered. Exiting…");
            e.printStackTrace();
            System.exit(-1);
        }
    }

    public static int calculateChecksum(int payloadLength, byte[] buffer) {
        long checksum = 0;
        for(int i = 0; i < payloadLength; i++) {
            checksum += buffer[i] & 0xFF; //To see the unsigned value
        }
        return (int) (checksum & 0xFF);
    }
}
