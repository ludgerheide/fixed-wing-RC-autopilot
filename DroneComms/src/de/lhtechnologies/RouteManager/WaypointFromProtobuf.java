package de.lhtechnologies.RouteManager;

import static de.lhtechnologies.CommunicationProtocol.*;

/**
 * Created by ludger on 06.02.16.
 */
public class WaypointFromProtobuf extends Waypoint {
    public WaypointFromProtobuf(DroneMessage.Waypoint wp) throws IllegalArgumentException {
        super(wp.getLatitude(), wp.getLongitude(), (double) wp.getAltitude()/(double) 100);
        if(wp.hasOrbitRadius()) {
            this.orbitRadius = Double.valueOf(wp.getOrbitRadius());
            if(wp.hasOrbitUntilTargetAltitude()) {
                this.orbitUntilTargetAltitude = wp.getOrbitUntilTargetAltitude();
            }

            if(!wp.hasOrbitClockwise()) {
                throw new IllegalArgumentException("Invalid Protobuf!");
            }
            this.orbitClockwise = wp.getOrbitClockwise();
        }
    }

    public DroneMessage.Waypoint toProtobuf() {
        DroneMessage.Waypoint.Builder wpBuilder = DroneMessage.Waypoint.newBuilder();
        wpBuilder.setLatitude((float) latitude)
                .setLongitude((float) longitude)
                .setAltitude((int) Math.round(altitude * 100)); //To convert to centimeters
        if(orbitRadius != null) {
            wpBuilder.setOrbitRadius((int) Math.round(orbitRadius))
                    .setOrbitUntilTargetAltitude(orbitUntilTargetAltitude)
                    .setOrbitClockwise(orbitClockwise);
        }
        return wpBuilder.build();
    }
}