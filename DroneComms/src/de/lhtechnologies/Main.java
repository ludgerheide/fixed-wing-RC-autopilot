package de.lhtechnologies;

import de.lhtechnologies.flightComputer.SerialPortManager;
import de.lhtechnologies.ircComms.IrcManager;

public class Main {

    public static void main(String[] args) {
        //Initialize the IRC bot
        IrcManager irc = new IrcManager();

        //Initialize the serial port
        SerialPortManager serial = new SerialPortManager();

        //Initialize the glue class
        IrcSerialConnection connection = new IrcSerialConnection(serial.serialTransmitter, irc.transmitter);
        irc.receiver.addObserver(connection);
        serial.serialReceiver.addObserver(connection);

        System.out.println("Initialization complete…");

        //Start the IRC thread
        new Thread(irc).start();
    }
}
