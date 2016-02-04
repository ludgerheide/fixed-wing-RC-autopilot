package de.lhtechnologies;

import de.lhtechnologies.flightComputer.SerialReceiver;
import de.lhtechnologies.flightComputer.SerialTransmitter;
import de.lhtechnologies.inetComms.InetReceiver;
import de.lhtechnologies.inetComms.InetTransmitter;

import java.util.Date;
import java.util.Observable;
import java.util.Observer;
import java.util.Random;

import static de.lhtechnologies.CommunicationProtocol.*;

/**
 * Created by ludger on 20.01.16.
 */
public class InetSerialConnection implements Observer {
    public SerialTransmitter serialTransmitter;
    public InetTransmitter inetTransmitter;

    DroneMessage.Builder storedMessage;
    long lastMessageSent; //In seconds
    public static long messageInterval = 1000; //In milliseconds

    public InetSerialConnection(SerialTransmitter serialTransmitter, InetTransmitter inetTransmitter) {
        this.inetTransmitter = inetTransmitter;
        this.serialTransmitter = serialTransmitter;

        storedMessage = DroneMessage.newBuilder();
        lastMessageSent = millis();
    }

    @Override
    public void update(Observable o, Object arg) {
        if(o instanceof SerialReceiver) {
            assert(arg instanceof DroneMessage);
            handleDroneMessage((DroneMessage) arg);
        } else {
            assert(o instanceof InetReceiver);
            assert(arg instanceof byte[]);
            handleInetMessage((byte[]) arg);
        }
    }

    public void handleInetMessage(byte[] message) {
        String msgString = new String(message);
        System.out.println(msgString);
    }

    public void handleDroneMessage(DroneMessage receivedMessage) {
        //Update the stored message with any new content

        //Content that depends on the timestamp of the main message
        if(!storedMessage.hasTimestamp() || storedMessage.getTimestamp() < receivedMessage.getTimestamp()) {
            storedMessage.setTimestamp(receivedMessage.getTimestamp());
            storedMessage.setCurrentMode(receivedMessage.getCurrentMode());
            storedMessage.setSeaLevelPressure(receivedMessage.getSeaLevelPressure());

            if(receivedMessage.hasCurrentAltitude()) {
                storedMessage.setCurrentAltitude(receivedMessage.getCurrentAltitude());
            }

            if(receivedMessage.hasOutputCommandSet()) {
                storedMessage.setOutputCommandSet(receivedMessage.getOutputCommandSet());
            }

            if(receivedMessage.hasInputCommandSet()) {
                storedMessage.setInputCommandSet(receivedMessage.getInputCommandSet());
            }

            if(receivedMessage.hasNewMode()) {
                storedMessage.setNewMode(receivedMessage.getNewMode());
            }
        }

        //Contant that has its own timestamp
        if(receivedMessage.hasCurrentSpeed() && (!storedMessage.hasCurrentSpeed() || storedMessage.getCurrentSpeed().getTimestamp() < receivedMessage.getCurrentSpeed().getTimestamp())) {
            storedMessage.setCurrentSpeed(receivedMessage.getCurrentSpeed());
        }

        if(receivedMessage.hasCurrentPosition() && (!storedMessage.hasCurrentPosition() || storedMessage.getCurrentPosition().getTimestamp() < receivedMessage.getCurrentPosition().getTimestamp())) {
            storedMessage.setCurrentPosition(receivedMessage.getCurrentPosition());
        }

        if(receivedMessage.hasCurrentAttitude() && (!storedMessage.hasCurrentAttitude() || storedMessage.getCurrentAttitude().getTimestamp() < receivedMessage.getCurrentAttitude().getTimestamp())) {
            storedMessage.setCurrentAttitude(receivedMessage.getCurrentAttitude());
        }

        //Do not care about raw sensor data (that would come here) but continue with battery data

        if (receivedMessage.hasCurrentBatteryData() && (!storedMessage.hasCurrentBatteryData() || storedMessage.getCurrentBatteryData().getTimestamp() < receivedMessage.getCurrentBatteryData().getTimestamp())) {
            storedMessage.setCurrentBatteryData(receivedMessage.getCurrentBatteryData());
        }

        //Get waypoint and home base changes
        if(receivedMessage.hasWaypoint() && (!storedMessage.hasWaypoint() || storedMessage.getWaypoint().getTimestamp() < receivedMessage.getWaypoint().getTimestamp())) {
            storedMessage.setWaypoint(receivedMessage.getWaypoint());
        }

        if(receivedMessage.hasHomeBase() && (!storedMessage.hasHomeBase() || storedMessage.getHomeBase().getTimestamp() < receivedMessage.getHomeBase().getTimestamp())) {
            storedMessage.setHomeBase(receivedMessage.getHomeBase());
        }

        try {
            if(millis() - lastMessageSent > messageInterval) {
                //FIXME: Remove after testiung
                storedMessage.setCurrentPosition(DroneMessage.Position.newBuilder()
                        .setLatitude(randInt(0, 900) / (float)10)
                        .setLongitude(randInt(-1800, 1800) / (float)10)
                        .build());
                inetTransmitter.transmit(storedMessage.build().toByteArray());
                storedMessage = DroneMessage.newBuilder();
                lastMessageSent = millis();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private long millis() {
        return  System.currentTimeMillis();
    }

    public static int randInt(int min, int max) {

        // NOTE: This will (intentionally) not run as written so that folks
        // copy-pasting have to think about how to initialize their
        // Random instance.  Initialization of the Random instance is outside
        // the main scope of the question, but some decent options are to have
        // a field that is initialized once and then re-used as needed or to
        // use ThreadLocalRandom (if using at least Java 1.7).
        Random rand = new Random();

        // nextInt is normally exclusive of the top value,
        // so add 1 to make it inclusive
        int randomNum = rand.nextInt((max - min) + 1) + min;

        return randomNum;
    }
}
