package de.lhtechnologies.RouteManager;

import de.lhtechnologies.CommunicationProtocol;

import java.util.ArrayList;

import static de.lhtechnologies.CommunicationProtocol.*;

/**
 * Created by ludger on 28.02.16.
 */
public class RouteManager {
    public static double epsilonAltitude = 10; //Meters
    public static double switchToNextWaypointRange = 25;



    ArrayList<Waypoint> smoothRoute;
    private int currentTargetIndex;

    public RouteManager() {
        smoothRoute = new ArrayList<>();
    }

    public void clearRoute() {
        if(smoothRoute != null) {
            smoothRoute.clear();
        }
    }

    public void addPoint(Waypoint waypoint) {
        smoothRoute.add(waypoint);
    }

    public void replaceRoute(Waypoint[] newRoute) {
        clearRoute();
        for(Waypoint wp: newRoute) {
            smoothRoute.add(wp);
        }
        currentTargetIndex = -1;
    }

    public DroneMessage getAutonomousUpdate(double lat, double lon, double heading, Double altitude) {
        //If we do not have a route, return nil
        if(smoothRoute.size() == 0) {
            return null;
        }

        //If we do not know which waypoint is the current target, find the next waypoint and set its index
        if(currentTargetIndex == -1) {
            //TODO: Resume route?
            //TODO: Crash/reboot survival?
            currentTargetIndex = 0;
        }

        /*Check if the next waypoint exists and we should switch to the next waypoint
        Rules for switching to next WP
         - Next WP must exist (otherwise orbit until directive changes)
         - If we are orbiting an "orbit until target altitude" *orbit* waypoint, we must be at target altitude (within 10m)
         - If we are orbiting, we must be withtin 25m of the *next* waypoint and witch to the next *plus one*
         - If we are not orbiting, we switch to the orbit waypoint if we are within 25m od the *next one*
         */

        if(currentTargetIndex < (smoothRoute.size() - 1)) {
            Waypoint currentTarget = smoothRoute.get(currentTargetIndex);
            Waypoint ownPosition = new Waypoint(lat, lon, altitude);
            if(currentTarget.hasOrbit()) {
                Waypoint endOfOrbit = smoothRoute.get(currentTargetIndex + 1);
                double distanceToEnd = ownPosition.distance(endOfOrbit);
                if(distanceToEnd < switchToNextWaypointRange) {
                    if(currentTarget.orbitUntilTargetAltitude == true) {
                        if (Math.abs(altitude - currentTarget.altitude) < epsilonAltitude) {
                            currentTargetIndex = currentTargetIndex + 2;
                        }
                    } else {
                        currentTargetIndex = currentTargetIndex + 2;
                    }
                }
            } else {
                //If we are approaching anything beyond the second WP, Check if we have overshot the target,
                // e.g the difference in bearing start - end and pos - end is greater than 90°
                double difference;
                if(currentTargetIndex > 0) {
                    Waypoint start = smoothRoute.get(currentTargetIndex - 1);
                    double startEnd = start.bearingTo(currentTarget);
                    double posEnd = ownPosition.bearingTo(currentTarget);

                    //Wonky angle subtraction
                    difference = startEnd - posEnd;
                    if (Math.abs(difference) > 180) {
                        if (startEnd - posEnd < 0) {
                            startEnd += 360;
                        } else {
                            posEnd += 360;
                        }
                    }
                    difference = startEnd - posEnd;
                } else {
                    //Difference is irrelevant otherwise, set it to 0
                    difference = 0;
                }

                if(Math.abs(difference) > 90) {
                    System.out.println("Warning: overshot!");
                    currentTargetIndex = currentTargetIndex + 1;
                } else {
                    Waypoint nextOrbitStart = smoothRoute.get(currentTargetIndex);
                    double distanceToNext = ownPosition.distance(nextOrbitStart);
                    if (distanceToNext < switchToNextWaypointRange) {
                        currentTargetIndex = currentTargetIndex + 1;
                    }
                }
            }
        }

        //This is the current Target *after* updating
        Waypoint currentTarget = smoothRoute.get(currentTargetIndex);
        Waypoint ownPosition = new Waypoint(lat, lon, altitude);

        double desiredHeading;
        if(currentTarget.hasOrbit()) {
            //Calculate how far we are from the intended track
            //>0 menas we are outside, <0 menas we are inside
            double error = ownPosition.distance(currentTarget) - currentTarget.orbitRadius;
            double courseCorrection = bearingCorrectionForError(error);

            if(currentTarget.orbitClockwise) {
                //If we are orbiting clockwise, the tangential heading is the bearing to the middle *-* 90
                double tangentialHeading = ownPosition.bearingTo(currentTarget) - 90;

                //If we are orbiting clockwise, we need to apply the coursecorrection by addition
                desiredHeading = tangentialHeading + courseCorrection;
            } else {
                //Counterclockwise, add 90 to bearing towards middle
                double tangentialHeading = ownPosition.bearingTo(currentTarget) + 90;

                //And subtract the correnction
                desiredHeading = tangentialHeading - courseCorrection;
            }
        } else {
            if(currentTargetIndex > 0) {
                Waypoint lineStart = smoothRoute.get(currentTargetIndex - 1);

                double error = Waypoint.crossTrackError(lineStart, currentTarget, ownPosition);
                double courseCorrection = bearingCorrectionForError(error);

                double targetHeading = lineStart.bearingTo(currentTarget);
                desiredHeading = targetHeading + courseCorrection;
            } else {
                //We are still approaching the start waypoint
                desiredHeading = ownPosition.bearingTo(currentTarget);
            }
        }

        System.out.format("Lat: %f, Lon: %f, Alt: %.1f, new Course %.0f, new Alt %.0f %n", lat, lon, altitude, desiredHeading, currentTarget.altitude);

        //Create the update message
        DroneMessage update = DroneMessage.newBuilder()
                .setAutonomousUpdate(DroneMessage.AutonomousUpdate.newBuilder()
                    .setAltitude((int)Math.round(currentTarget.altitude * 100))
                    .setHeading((int)Math.round(desiredHeading * 64))
                    .build())
                .build();
        return update;
    }

    /**
     * Calculates the bearing correction for a given error in meters
     */
    private static double maximalError = 50; //If the error is bigger than this, we just give max correction
    private static double maximalCorrection = 90; //The maximal correction angle we fly. Must be < 90

    double bearingCorrectionForError(double error) {
        if(error < maximalError) {
            return maximalCorrection * (error/maximalError);
        } else {
            if(error < 0) {
                return -maximalCorrection;
            } else {
                return maximalCorrection;
            }
        }
    }
}
