package de.lhtechnologies.flightComputer;

import gnu.io.CommPortIdentifier;
import gnu.io.SerialPort;

/**
 * Created by ludger on 18.01.16.
 */
public class SerialPortManager {
    public static int baudRate = 115200;
    public static int dataBits = SerialPort.DATABITS_8;
    public static int stopBits = SerialPort.STOPBITS_1;
    public static int parity = SerialPort.PARITY_NONE;
    public static String deviceFile = "/dev/ttyAMA0";

    public SerialReceiver serialReceiver;

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
        } catch (Exception e) {
            System.out.println("Error encountered. Exiting…");
            e.printStackTrace();
            System.exit(-1);
        }
    }
}
