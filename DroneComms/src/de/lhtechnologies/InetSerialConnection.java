package de.lhtechnologies;

import de.lhtechnologies.flightComputer.SerialReceiver;
import de.lhtechnologies.flightComputer.SerialTransmitter;
import de.lhtechnologies.inetComms.InetReceiver;
import de.lhtechnologies.inetComms.InetTransmitter;

import java.util.Base64;
import java.util.Observable;
import java.util.Observer;

import static de.lhtechnologies.CommunicationProtocol.*;

/**
 * Created by ludger on 20.01.16.
 */
public class InetSerialConnection implements Observer {
    public SerialTransmitter serialTransmitter;
    public InetTransmitter inetTransmitter;

    public InetSerialConnection(SerialTransmitter serialTransmitter, InetTransmitter inetTransmitter) {
        this.inetTransmitter = inetTransmitter;
        this.serialTransmitter = serialTransmitter;
    }

    @Override
    public void update(Observable o, Object arg) {
        if(o instanceof SerialReceiver) {
            assert(arg instanceof DroneMessage);
            handleDroneMessage((DroneMessage) arg);
        } else {
            assert(o instanceof InetReceiver);
            assert(arg instanceof byte[]);
            handleIrcMessage((byte[]) arg);
        }
    }

    public void handleIrcMessage(byte[] message) {
        String msgString = new String(message);
        System.out.println(msgString);
    }

    public void handleDroneMessage(DroneMessage message) {
        //Update the stored message with any new content
        try {
            inetTransmitter.transmit(message.toByteArray());
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
