package de.lhtechnologies;

import com.google.protobuf.Internal;
import com.google.protobuf.InvalidProtocolBufferException;
import de.lhtechnologies.RouteManager.RouteManager;
import de.lhtechnologies.RouteManager.Waypoint;
import de.lhtechnologies.RouteManager.WaypointFromProtobuf;
import de.lhtechnologies.flightComputer.SerialReceiver;
import de.lhtechnologies.flightComputer.SerialTransmitter;
import de.lhtechnologies.inetComms.InetReceiver;
import de.lhtechnologies.inetComms.InetTransmitter;

import java.io.IOException;
import java.util.*;

import static de.lhtechnologies.CommunicationProtocol.*;

/**
 * Created by ludger on 20.01.16.
 */
public class InetSerialConnection implements Observer {
    public SerialTransmitter serialTransmitter;
    public InetTransmitter inetTransmitter;

    private RouteManager routeManager;

    DroneMessage.Builder storedMessage;
    long lastMessageSent; //In seconds
    public static long messageInterval = 1000; //In milliseconds
    public static long maxAltitudeAgeDifference = 100; // In Millisencods

    public InetSerialConnection(SerialTransmitter serialTransmitter, InetTransmitter inetTransmitter) {
        this.inetTransmitter = inetTransmitter;
        this.serialTransmitter = serialTransmitter;

        this.routeManager = new RouteManager();

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
        //Decode the message
        try {
            DroneMessage msg = DroneMessage.parseFrom(message);

            //If the message has a sea level pressure or flight mode, forward it directly
            if(msg.hasSeaLevelPressure() || msg.hasNewMode()) {
                DroneMessage.Builder toFlightControl = DroneMessage.newBuilder();
                if(msg.hasSeaLevelPressure()) {
                    toFlightControl.setSeaLevelPressure(msg.getSeaLevelPressure());
                }
                if(msg.hasNewMode()) {
                    toFlightControl.setNewMode(msg.getNewMode());
                }
                DroneMessage outMsg = toFlightControl.build();
                try {
                    serialTransmitter.transmit(outMsg);
                } catch (IOException e) {
                    System.out.println("Sending flightmode/SLP update failed!");
                }
            }

            //If not, the message should contain a route
            if(msg.getRouteCount() > 0) {
                List<DroneMessage.Waypoint> dmRoute = msg.getRouteList();
                Waypoint[] realRoute = new Waypoint[msg.getRouteCount()];

                for(int i = 0; i < msg.getRouteCount(); i++) {
                    WaypointFromProtobuf wp = new WaypointFromProtobuf(dmRoute.get(i));
                    realRoute[i] = wp;
                }
                routeManager.replaceRoute(realRoute);

            } else {
                System.out.println("Message without route received!");
            }

        } catch (InvalidProtocolBufferException e) {
            System.out.println("Failed to parse protobuf from network!");
        }
    }

    public void handleDroneMessage(DroneMessage receivedMessage) {
        updateStoredMessageAndSend(receivedMessage);

        //If the message contains a position, update the heading, switch waypoints if applicable and send it to the device
        if(receivedMessage.hasCurrentPosition() && (!storedMessage.hasCurrentPosition() || storedMessage.getCurrentPosition().getTimestamp() < receivedMessage.getCurrentPosition().getTimestamp())) {
            double lat = receivedMessage.getCurrentPosition().getLatitude();
            double lon = receivedMessage.getCurrentPosition().getLongitude();

            //There should always be a new sped when there is a new position
            assert(receivedMessage.hasCurrentSpeed() && (!storedMessage.hasCurrentSpeed() || storedMessage.getCurrentSpeed().getTimestamp() < receivedMessage.getCurrentSpeed().getTimestamp()));
            double hdg = receivedMessage.getCurrentSpeed().getCourseOverGround() / (double)64; //Return to float from fixed-point

            //A recent altitude should always be present, but we check anyway
            Double alt;
            if((storedMessage.getTimestamp() - receivedMessage.getCurrentPosition().getTimestamp()) < maxAltitudeAgeDifference) {
                alt = storedMessage.getCurrentAltitude() / (double)100; //Convert cm to m
            } else {
                alt = null;
            }

            DroneMessage update = routeManager.getAutonomousUpdate(lat, lon, hdg, alt);
            if(update != null) {
                try {
                    serialTransmitter.transmit(update);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

    }

    public void updateStoredMessageAndSend(DroneMessage receivedMessage) {
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
        if(receivedMessage.hasHomeBase() && (!storedMessage.hasHomeBase() || storedMessage.getHomeBase().getTimestamp() < receivedMessage.getHomeBase().getTimestamp())) {
            storedMessage.setHomeBase(receivedMessage.getHomeBase());
        }

        try {
            if(millis() - lastMessageSent > messageInterval) {
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
