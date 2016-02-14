package de.lhtechnologies.RouteManager;

/**
 * Created by ludger on 06.02.16.
 */
public class Waypoint {
    private static double epsilon = 5; //Unit: meters
    private static double earthRadius = 6371000; //Unit: meters

    //Position of the waypoitn
    protected double latitude; //Degrees with fractions
    protected double longitude; //Degrees with fractions
    protected double altitude; //Meters ASL

    //Orbit parameters
    protected Double orbitRadius; //Meters
    protected Boolean orbitUntilTargetAltitude; //Meters
    protected Boolean orbitClockwise;

    public Waypoint(double latitude, double longitude, double altitude, double orbitRadius, boolean orbitUntilTargetAltitude, boolean orbitClockwise) throws IllegalArgumentException {
        this(latitude, longitude, altitude);

        this.orbitRadius = orbitRadius;
        if(orbitRadius < 0) {
            throw new IllegalArgumentException("orbitRadius Out of bounds!");
        }
        this.orbitUntilTargetAltitude = orbitUntilTargetAltitude;
    }

    public Waypoint(double latitude, double longitude, double altitude) throws IllegalArgumentException {
        this.latitude = latitude;
        if(latitude < -90 || latitude > 90) {
            throw new IllegalArgumentException("Latitude out of bounds!");
        }

        this.longitude = longitude;
        if(longitude < -180 || longitude > 180) {
            throw new IllegalArgumentException("Longitude out of bounds!");
        }

        this.altitude = altitude;

        orbitRadius = null;
        orbitUntilTargetAltitude = null;
        orbitClockwise = null;
    }

    public boolean hasOrbit() {
        if(orbitRadius != null) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * Calculates the distance to another Waypoint (bewtween centers for orbit waypoints)
     * Source: http://www.movable-type.co.uk/scripts/latlong.html
     *
     * @param other the other waypoint (Must be a non-orbit waypoint)
     *
     * @return an orbit waypoint
     */
    public double distance(Waypoint other) {
        double ownLatitude = Math.toRadians(latitude);
        double otherLatitude = Math.toRadians(other.latitude);
        double latDiff = Math.toRadians(other.latitude - latitude);

        double ownLongitude = Math.toRadians(longitude);
        double otherLongitude = Math.toRadians(other.longitude);
        double longDiff = Math.toRadians(other.longitude - longitude);

        double a = (Math.sin(latDiff/2) * Math.sin(latDiff/2)) +
                (Math.cos(ownLatitude) * Math.cos(otherLatitude) * Math.sin(longDiff/2) * Math.sin(longDiff/2));
        double c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a));

        return earthRadius * c;
    }

    /**
     * Creates an orbit waypoint which has a circle that is a tangent to the line between a and b and b and c.
     * Throws an IllegalArgumentException if given orbit waypoints as input.
     * Throws an ArithmeticException if the orbit can't be created
     *
     * @param a the first waypoint (Must be a non-orbit waypoint)
     * @param b the second waypoint (Must be a non-orbit waypoint)
     * @param c the third waypoint (Must be a non-orbit waypoint)
     *
     * @return an orbit waypoint
     */
    public Waypoint tangentialOrbitWaypoint(Waypoint a, Waypoint b, Waypoint c) throws IllegalArgumentException, ArithmeticException {
        if(a.hasOrbit() || b.hasOrbit() || c.hasOrbit()) {
            throw new IllegalArgumentException("Cannot calculate tangents for orbit waypoints");
        }
        return null;
    }

    /**
     * Calculates the intercept point between an orbit waypoint and a line segment (Deutsch: Strecke, nicht Gerade)
     * between two waypoints. Returns null if there is no such intercept point. Throws IllegalargumentException if the
     * orbit waypoint is not one or the
     *
     * @param a non-orbit first waypoint
     * @param b non-orbit second waypoint
     * @param orbit orbit third waypoint
     *
     * @return the intercept point to get to the orbit
     */
    public Waypoint interceptOrbit(Waypoint a, Waypoint b, Waypoint orbit) throws IllegalArgumentException {
        if(a.hasOrbit() || b.hasOrbit() || !orbit.hasOrbit()) {
            throw new IllegalArgumentException("A waypoint has an orbit that it shouldn't have (or the other way round)");
        }
        return null;
    }
}