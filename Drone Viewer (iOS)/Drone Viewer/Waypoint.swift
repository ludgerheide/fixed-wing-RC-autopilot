//
//  Waypoint.swift
//  Drone Viewer
//
//  Created by Ludger Heide on 06.02.16.
//  Copyright © 2016 Ludger Heide. All rights reserved.
//

import Foundation

class Waypoint {
    private static let earthRadius: Double = 6371000 //Meters
    
    struct Point {
        let latitude: Double!
        let longitude: Double!
        let altitude: Double!
    }
    
    struct Orbit {
        let radius: Double!
        let orbitUntilAltitude: Bool!
        let clockwise: Bool!
    }
    
    private let point: Point!
    private let orbit: Orbit?
    
    init?(thePoint: Point!, theOrbit: Orbit?) {
        self.point = thePoint
        self.orbit = theOrbit
        
        //Verify the point
        if((thePoint.latitude > 90) || (thePoint.latitude < -90) || (thePoint.longitude > 180) || (thePoint.longitude < -180)) {
            return nil
        }
        
        //Verify the orbit
        if((theOrbit != nil) && (theOrbit?.radius < 0)) {
            return nil
        }
    }
    
    convenience init?(latitude: Double!, longitude: Double!, altitude: Double!, orbitRadius: Double?, orbitUntilAltitude: Bool?, clockwise: Bool?) {
        let thePoint = Point(latitude: latitude, longitude: longitude, altitude: altitude)
        
        let theOrbit: Orbit?
        if(orbitRadius != nil) {
            theOrbit = Orbit(radius: orbitRadius!, orbitUntilAltitude: orbitUntilAltitude!, clockwise: clockwise!)
        } else {
            theOrbit = nil
        }
        
        self.init(thePoint: thePoint, theOrbit: theOrbit)
    }
    
    convenience init?(latitude: Double!, longitude: Double!, altitude: Double!) {
        self.init(latitude: latitude, longitude: longitude, altitude: altitude, orbitRadius: nil, orbitUntilAltitude: nil, clockwise: nil)
    }
    
    convenience init?(wp: DroneMessage_Waypoint) {
        let thePoint = Point(latitude: Double(wp.latitude), longitude: Double(wp.longitude), altitude: Double(wp.altitude) / Double(100))
        
        let theOrbit: Orbit?
        if(wp.hasOrbitRadius) {
            theOrbit = Orbit(radius: Double(wp.orbitRadius), orbitUntilAltitude: wp.orbitUntilTargetAltitude, clockwise: wp.orbitClockwise)
        } else {
            theOrbit = nil
        }
        
        self.init(thePoint: thePoint, theOrbit: theOrbit)
    }
    
    func toProtobuf() -> DroneMessage_Waypoint! {
        let wp: DroneMessage_Waypoint = DroneMessage_Waypoint();
        wp.latitude = Float(point.latitude)
        wp.longitude = Float(point.longitude)
        wp.altitude = Int32(round(point.altitude * 100))
        
        if(orbit != nil) {
            wp.orbitRadius = UInt32(round(orbit!.radius))
            wp.orbitUntilTargetAltitude = orbit!.orbitUntilAltitude
            wp.orbitClockwise = orbit!.clockwise
        }
        
        return wp
    }
    
    /**
     Returns an array of waypoints to draw this waypoint as polyline
     
     Parameters:
     - maxNumberOfPoints: The m
     
     - Returns: an array of waypoints to draw this waypoint as polyline or nil, if this was requested for a non-orbit waypoint
     */
    func descriptionLine(maxNumberOfPoints: Int) -> Array<Point!>? {
        
        if(self.orbit == nil) {
            return nil
        }
        
        let degreesPerPoint = 360 / Double(maxNumberOfPoints)
        var returnArray = Array<Point!>(count: maxNumberOfPoints, repeatedValue: nil)
        
        for(var i = 0; i < maxNumberOfPoints; i++) {
            returnArray[i] = self.waypointWithDistanceAndBearing(self.orbit!.radius, bearing: degreesPerPoint * Double(i))
        }
        return returnArray
    }
    
    /**
     Returns an array of waypoints to draw this waypoint as polyline
     
     Parameters:
     - other: the waypoint the line goes to
     
     - Returns: an array of waypoints to draw this waypoint as polyline or nil, if this was requested for a orbit waypoint
     */
    func descriptionLine(other: Waypoint) -> Array<Point!>? {
        
        if(self.orbit != nil) {
            return nil
        }
        
        var returnArray = Array<Point!>(count: 2, repeatedValue: nil)
        returnArray[0] = self.point
        returnArray[1] = other.point
        
        return returnArray
    }
    
    /**
     Calculates the distance to another waypoint. If these are orbit waypoints, it calculates distance between centers.
     
     Parameters:
     - other: The other waypoint
     
     - Returns: A Double indicating the dietance in meters.
     */
    func distanceTo(other: Waypoint) -> Double! {
        let φ1 = point.latitude.toRadians
        let φ2 = other.point.latitude.toRadians
        let Δφ = (other.point.latitude - point.latitude).toRadians
        let Δλ = (other.point.longitude - point.longitude).toRadians
        
        let a = sin(Δφ/2) * sin(Δφ/2) +
            cos(φ1) * cos(φ2) *
            sin(Δλ/2) * sin(Δλ/2);
        let c = 2 * atan2(sqrt(a), sqrt(1-a));
        
        return Waypoint.earthRadius * c;
    }
    
    
    /**
    Calculates the bearing to another waypoint from this one
    
    Parameters:
    - other: The other waypoint
    
    - Returns: A Double indicating the bearing in degrees.
    */
    func bearingTo(other: Waypoint) -> Double! {
        let φ1 = point.latitude.toRadians
        let φ2 = other.point.latitude.toRadians
        
        let λ1 = point.longitude.toRadians
        let λ2 = other.point.longitude.toRadians
        
        let y = sin(λ2-λ1) * cos(φ2)
        let x = cos(φ1) * sin(φ2) - sin(φ1) * cos(φ2) * cos(λ2-λ1);
        return atan2(y, x).toDegrees
    }
    
    
    /**
     Calculates an waypoint given another waypoint and distance+bearing
     
     Parameters:
     - distance: The distance from this point in meters
     - bearing: The direction in whioch we go away from the start point in degrees
     
     - Returns: An waypoint with a given distance and bearing to another point
     */
    func waypointWithDistanceAndBearing(distance: Double, bearing: Double) -> Point! {
        let φ1 = point.latitude.toRadians
        let λ1 = point.longitude.toRadians
        
        let φ2 = asin(sin(φ1) * cos(distance/Waypoint.earthRadius) + cos(φ1) * sin(distance/Waypoint.earthRadius) * cos(bearing.toRadians))
        let λ2 = λ1 + atan2(sin(bearing.toRadians) * sin(distance/Waypoint.earthRadius) * cos(φ1), cos(distance/Waypoint.earthRadius) - sin(φ1) * sin(φ2))
        
        return Point(latitude: φ2, longitude: λ2, altitude: self.point.altitude)
    }
    
    
    /**
     Calculates an orbit waypoint which has an orbit that tangentially intercepts the lines from this point to points a and b
     
     Parameters:
     - pointA: The first line waypoint
     - pointC: The second line waypoint
     - radius:
     
     - Returns: An orbit waypoint that fulfills the conditions or nil if no such waypoint exists.
     */
    func orbitWaypointBetween(pointA: Waypoint, pointC: Waypoint, radius: Double) -> Waypoint! {
        let firstBearing = pointA.bearingTo(self)
        let secondBearing = pointC.bearingTo(self)
        let difference = (firstBearing - secondBearing) % 360
        
        let clockwise: Bool
        if(difference > 0) {
            clockwise = false
        } else {
            clockwise = true
        }
        
        let middleBearing = (firstBearing + difference/2) % 360
        let distance = radius + Waypoint.earthRadius * tan(difference.toRadians)
        
        let orbitPoint = waypointWithDistanceAndBearing(distance, bearing: middleBearing)
        let desiredOrbit = Orbit(radius: radius, orbitUntilAltitude: false, clockwise: clockwise)
        
        return Waypoint(thePoint: orbitPoint, theOrbit: desiredOrbit)
    }
    
    
    /**
    Calculates the cross track error between a line between two points and a third point
    
    Parameters:
    - position: The other waypoint
    - linePointA: The first line waypoint
    - linePointB: The second line waypoint
    
    - Returns: A Double indicatingthe cross-track error in meters. The sign indicates the side we are on. HOW?
    */
    static func lineCrossTrackError(position: Waypoint, linePointA: Waypoint, linePointB: Waypoint) -> Double! {
        let d13 = linePointA.distanceTo(position)
        let θ13 = linePointA.bearingTo(position).toRadians
        let θ12 = linePointA.bearingTo(linePointB).toRadians
        return asin(sin(d13 / Waypoint.earthRadius) * sin(θ13 - θ12)) * Waypoint.earthRadius;
    }
}

//Utility fuctions
extension Double {
    var toRadians: Double {
        return self * (Double(M_PI) / Double(180))
    }
    
    var toDegrees: Double {
        return self * (Double(180) / Double(M_PI))
    }
}