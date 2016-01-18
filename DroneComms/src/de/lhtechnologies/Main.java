package de.lhtechnologies;

import de.lhtechnologies.flightComputer.SerialPortManager;
import de.lhtechnologies.flightComputer.SerialReceiver;

public class Main {

    public static void main(String[] args) {
        //Initialize the receiver
        SerialPortManager manager = new SerialPortManager();

        //Initialize the dummy listener
        DataPrinter dp = new DataPrinter();

        manager.serialReceiver.addObserver(dp);

        System.out.println("Initialization complete…");
    }
}
