package de.lhtechnologies;

import com.google.protobuf.InvalidProtocolBufferException;
import de.lhtechnologies.flightComputer.SerialReceiver;
import de.lhtechnologies.flightComputer.SerialTransmitter;
import de.lhtechnologies.ircComms.IrcReceiver;
import de.lhtechnologies.ircComms.IrcTransmitter;
import org.pircbotx.PircBotX;

import java.util.Base64;
import java.util.Observable;
import java.util.Observer;

import static de.lhtechnologies.CommunicationProtocol.*;

/**
 * Created by ludger on 20.01.16.
 */
public class IrcSerialConnection implements Observer {
    public SerialTransmitter serialTransmitter;
    public IrcTransmitter ircTransmitter;

    public IrcSerialConnection(SerialTransmitter serialTransmitter, IrcTransmitter ircTransmitter) {
        this.ircTransmitter = ircTransmitter;
        this.serialTransmitter = serialTransmitter;
    }

    @Override
    public void update(Observable o, Object arg) {
        if(o instanceof SerialReceiver) {
            assert(arg instanceof DroneMessage);
            handleDroneMessage((DroneMessage) arg);
        } else {
            assert(o instanceof IrcReceiver);
            assert(arg instanceof String);
            handleIrcMessage((String) arg);
        }
    }

    public void handleIrcMessage(String message) {
        Base64.Decoder dec = Base64.getDecoder();
        byte[] decodedMessage = dec.decode(message);
        try {
            DroneMessage parsedMessage = DroneMessage.parseFrom(decodedMessage);
            serialTransmitter.transmit(parsedMessage);
            ircTransmitter.transmitString("Message transmitted successfully.");
        } catch (Exception e) {
            ircTransmitter.transmitString("WARNING: Transmission to device failed! Reason: " + e.getMessage());
        }
    }

    public void handleDroneMessage(DroneMessage message) {
        if(ircTransmitter.bot.getState() == PircBotX.State.CONNECTED) {
            ircTransmitter.transmit(message.toByteArray());
        }
    }
}
