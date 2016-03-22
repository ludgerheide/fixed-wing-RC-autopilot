package de.lhtechnologies.flightComputer;

import com.google.protobuf.InvalidProtocolBufferException;
import de.lhtechnologies.CommunicationProtocol;
import de.lhtechnologies.RouteManager.Waypoint;

import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;

import static de.lhtechnologies.CommunicationProtocol.*;

/**
 * Created by ludger on 19.03.16.
 */
public class MockSerialReceiver extends SerialReceiver implements Runnable {
    private Waypoint position = new Waypoint(51.117, 10.3343, 100);
    private double currentHeading = 0;
    private int timestamp = 0;

    private final double climbRate = 2; //m/s
    private final double speed = 150; // m/s
    private final double interval = 0.1; //seconds

    public MockSerialReceiver(InputStream in) {
        super(in);
    }

    @Override
    public void run() {
        while (true) {
            int inByte, checksum;
            int index = 0;
            try {
                while ((in.available() > 0) && (inByte = in.read()) > -1) {
                    //First, synchronize on the start marker
                    if (index < 5) {
                        if (inByte == startMarker[index]) {
                            index++;
                        } else {
                            index = 0;
                        }
                    } else if (index == 5) {
                        //If we reached this part, we have received a valid start marker.
                        //The next byte is the length, add 5 + 1 + 1 to it to get the index of the final byte
                        payloadLength = inByte;
                        index++;
                    } else if (index >= startMarker.length && index < (payloadLength + startMarker.length + 1)) {
                        //Now we are receiving the payload
                        inputBuffer[index - (startMarker.length + 1)] = (byte) inByte;
                        index++;
                    } else {
                        //Reception complete. Verify checksum, create Protobuf object and notify the observers
                        int receivedChecksum = inByte;
                        int calculatedChecksum = calculateChecksum();
                        if (receivedChecksum == calculatedChecksum) {
                            byte[] rawMessage = Arrays.copyOfRange(inputBuffer, 0, payloadLength);
                            try {
                                DroneMessage msg = DroneMessage.parseFrom(rawMessage);
                                processMessage(msg);
                            } catch (InvalidProtocolBufferException e) {
                                System.out.println("WARNING: Invalid protobuf received!");
                            }
                        }
                        break;
                    }
                }
            } catch (IOException e) {
                System.out.println("Error encountered. Exiting…");
                e.printStackTrace();
                System.exit(-1);
            }

            buildOutMsg();

            try {
                Thread.sleep((long) (interval * 1000));
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    private void processMessage(DroneMessage msg) {
        assert(msg.getNewMode() == DroneMessage.FlightMode.m_autonomous);
        double targetHeading = msg.getAutonomousUpdate().getHeading() / (double)64;
        double targetAltitude = msg.getAutonomousUpdate().getAltitude() / (double)100;

        //Create an updated position;
        double newAltitude;
        if(targetAltitude - position.altitude > 0) {
            newAltitude = position.altitude + interval * climbRate;
        } else {
            newAltitude = position.altitude - interval * climbRate;
        }

        position = position.waypointWithDistanceAndBearing(interval * speed, targetHeading);
        position.altitude = newAltitude;
    }

    private void buildOutMsg() {
        timestamp = timestamp + 1;
        DroneMessage outMsg = DroneMessage.newBuilder()
                .setTimestamp(timestamp)
                .setCurrentPosition(DroneMessage.Position.newBuilder()
                        .setTimestamp(timestamp)
                        .setLatitude((float)position.latitude)
                        .setLongitude((float)position.longitude)
                        .build())
                .setCurrentAltitude((int)(position.altitude * 100))
                .build();
        setChanged();
        notifyObservers(outMsg);
    }
}
