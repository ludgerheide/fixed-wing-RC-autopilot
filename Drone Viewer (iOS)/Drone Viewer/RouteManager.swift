//
//  RouteManager.swift
//  Drone Viewer
//
//  Created by Ludger Heide on 14.02.16.
//  Copyright © 2016 Ludger Heide. All rights reserved.
//

import MapKit

class RouteManager: NSObject {
    private static let defaultRadius: Double = Waypoint.defaultRadius
    private static let defaultNumberOfPoints: UInt = 30
    private static let countKey = "countKey"
    
    class WaypointWithAnnotations: NSObject, MKAnnotation {
        @objc var coordinate: CLLocationCoordinate2D {
            set {
                waypoint.point = Waypoint.Point(latitude: newValue.latitude, longitude: newValue.longitude, altitude: waypoint.point.altitude)
            }
            get {
                return CLLocationCoordinate2D(latitude: waypoint.point.latitude, longitude: waypoint.point.longitude)
            }
        }
        
        @objc var subtitle: String? {
            get {
                let newSubtitle = NSString(format: "Altitude: %.0fm", waypoint.point.altitude)
                return newSubtitle as String
            }
        }
        
        @objc var title: String?
        
        override func isEqual(object: AnyObject?) -> Bool {
            if let rhs = object as? WaypointWithAnnotations {
                if(self.waypoint == rhs.waypoint) {
                    return true
                } else {
                    return false
                }
            } else {
                return false
            }
        }
        
        var waypoint: Waypoint!
        var radius: Double?
        var orbitUntilAltitude: Bool?
        
        //Only for a standalone orbit point (last waypoint or only waypoint)
        var initialBearing: Double?
        var clockwise: Bool?
        
        //Required values
        private let latKey = "lat"
        private let lonKey = "lon"
        private let altKey = "alt"
        
        //Optional values
        private let radiusPresentKey = "radPresent"
        private let radiusKey = "rad"
        
        private let orbitUntilAltitudePresentKey = "orbUntilAltPresent"
        private let orbitUntilAltitudeKey = "orbUntilAlt"
        
        private let initialBearingPresentKey = "initBearPresent"
        private let initialBearingKey = "initBear"
        
        private let clockwisePresentKey = "clockwisePresent"
        private let clockwiseKey = "clockwise"
        
        required convenience init(coder aDecoder: NSCoder) {
            self.init()
            
            //Init the reuqired values
            let latitude = aDecoder.decodeDoubleForKey(latKey)
            let longitude = aDecoder.decodeDoubleForKey(lonKey)
            let altitude = aDecoder.decodeDoubleForKey(altKey)
            
            let thePoint = Waypoint.Point(latitude: latitude, longitude: longitude, altitude: altitude)
            waypoint = Waypoint(thePoint: thePoint, theOrbit: nil)
            
            //The optional values
            if(aDecoder.decodeBoolForKey(radiusPresentKey) == true) {
                radius = aDecoder.decodeDoubleForKey(radiusKey)
            }
            
            if(aDecoder.decodeBoolForKey(orbitUntilAltitudePresentKey) == true) {
                orbitUntilAltitude = aDecoder.decodeBoolForKey(orbitUntilAltitudeKey)
            }
            
            if(aDecoder.decodeBoolForKey(initialBearingPresentKey) == true) {
                initialBearing = aDecoder.decodeDoubleForKey(initialBearingKey)
            }
            
            if(aDecoder.decodeBoolForKey(clockwisePresentKey) == true) {
                clockwise = aDecoder.decodeBoolForKey(clockwiseKey)
            }

        }
        
        func encodeWithCoder(aCoder: NSCoder) {
            //Code the required values
            aCoder.encodeDouble(waypoint.point.latitude, forKey: latKey)
            aCoder.encodeDouble(waypoint.point.longitude, forKey: lonKey)
            aCoder.encodeDouble(waypoint.point.altitude, forKey: altKey)
            
            
            //Code thje optional values if applicable
            if(radius != nil) {
                aCoder.encodeBool(true, forKey: radiusPresentKey)
                aCoder.encodeDouble(radius!, forKey: radiusKey)
            } else {
                aCoder.encodeBool(false, forKey: radiusPresentKey)
            }
            
            if(orbitUntilAltitude != nil) {
                aCoder.encodeBool(true, forKey: orbitUntilAltitudePresentKey)
                aCoder.encodeBool(orbitUntilAltitude!, forKey: orbitUntilAltitudeKey)
            } else {
                aCoder.encodeBool(false, forKey: orbitUntilAltitudePresentKey)
            }
            
            if(initialBearing != nil) {
                aCoder.encodeBool(true, forKey: initialBearingPresentKey)
                aCoder.encodeDouble(initialBearing!, forKey: initialBearingKey)
            } else {
                aCoder.encodeBool(false, forKey: initialBearingPresentKey)
            }
            
            if(clockwise != nil) {
                aCoder.encodeBool(true, forKey: clockwisePresentKey)
                aCoder.encodeBool(clockwise!, forKey: clockwiseKey)
            } else {
                aCoder.encodeBool(false, forKey: clockwisePresentKey)
            }
        }
    }
    
    private var rawRoute: Array<WaypointWithAnnotations!>!
    
    override init() {
        rawRoute = Array<WaypointWithAnnotations!>()
        super.init()
    }
    
    required convenience init(coder aDecoder: NSCoder) {
        self.init()
        
        let nbCounter = aDecoder.decodeIntegerForKey(RouteManager.countKey)
        
        for _ in 0 ..< nbCounter {
            if let wp = aDecoder.decodeObject() as? WaypointWithAnnotations {
                rawRoute.append(wp)
            }
        }
    }
    
    func encodeWithCoder(aCoder: NSCoder) {
        aCoder.encodeInteger(rawRoute.count, forKey: RouteManager.countKey)
        
        for wp in rawRoute {
            aCoder.encodeObject(wp)
        }
    }
    
    func clearRoute() {
        rawRoute.removeAll(keepCapacity: true)
    }
    
    func addPoint(thePoint: WaypointWithAnnotations!) {
        //Make sure this point is an orbit waypoint
        thePoint.orbitUntilAltitude = nil
        if(thePoint.radius == nil) {
            thePoint.radius = RouteManager.defaultRadius
        }
        if(thePoint.clockwise == nil) {
            thePoint.clockwise = true
        }
        
        //If it is the only point, make sure it has an initial bearing
        if(rawRoute.count == 0 && thePoint.initialBearing == nil) {
            thePoint.initialBearing = 0
        } else if(rawRoute.count != 0) {
            thePoint.initialBearing = nil
        }
        
        rawRoute.append(thePoint)
        
        //Turn the previous last waypoint into a non-orbit waypoint
        if (rawRoute.count > 1) {
            let oldLastPoint = rawRoute[rawRoute.count - 2]
            oldLastPoint.orbitUntilAltitude = false
            
            oldLastPoint.clockwise = nil
            oldLastPoint.initialBearing = nil
        }
    }
    
    func removePoint(thePoint: WaypointWithAnnotations!) -> Bool! {
        if let index = rawRoute.indexOf({$0.isEqual(thePoint)}) {
            rawRoute.removeAtIndex(index)
            //Change the new last waypoint into an orbit waypoint, if it exists
            if(rawRoute.count > 0) {
                let newLastPoint = rawRoute[rawRoute.count - 1]
                newLastPoint.orbitUntilAltitude = nil
                if(newLastPoint.radius == nil) {
                    newLastPoint.radius = RouteManager.defaultRadius
                }
                if(newLastPoint.clockwise == nil) {
                    newLastPoint.clockwise = true
                }
            }
            //And give it an initial bearing if it is the only point
            if(rawRoute.count == 1) {
                let newLastPoint = rawRoute[rawRoute.count - 1]
                newLastPoint.initialBearing = 0
            }
            return true
        } else {
            return false
        }
    }
    
    func getAnnotatedWaypoints() -> Array<WaypointWithAnnotations!>? {
        if(rawRoute.count == 0) {
            return nil
        } else {
            for(var i = 0; i < rawRoute.count; i++) {
                let currentWp = rawRoute[i]
                
                if(i == (rawRoute.count - 1)) {
                    currentWp.title = "Last Waypoint"
                } else {
                    currentWp.title = "Waypoint " + String(i+1)
                }
            }
            return rawRoute
        }
    }
    
    func createSmoothedPolyLines() -> Array<MKPolyline!>? {
        if(rawRoute.count == 0) {
            return nil
        } else {
            var returnArray = Array<MKPolyline!>()
            returnArray.append(createUnSmoothPolyLine()!)
            
            //Now create a circle for each bend and one after the last point
            //Create a circle section for "orbit until alt = false" and a circle for "orbit until alt = true"
            if(rawRoute.count >= 3) {
                for(var i = 1; i < (rawRoute.count - 1); i++) {
                    let annotatedWp = rawRoute[i]
                    let wpA = rawRoute[i-1].waypoint
                    let wpB = rawRoute[i].waypoint
                    let wpC = rawRoute[i+1].waypoint
                    
                    let (firstPoint, orbitPoint, lastPoint) = wpB.tangentialSegment(wpA, pointC: wpC, radius: annotatedWp.radius!)
                    if(firstPoint != nil && orbitPoint != nil && lastPoint != nil) {
                        let curvePoints: Array<Waypoint.Point!>?
                        if(annotatedWp.orbitUntilAltitude == false) {
                            let fromBearing = orbitPoint!.bearingTo(firstPoint!)
                            let toBearing = orbitPoint!.bearingTo(lastPoint!)
                            curvePoints = orbitPoint!.descriptionLineForOrbitSegment(RouteManager.defaultNumberOfPoints, fromBearing: fromBearing, toBearing: toBearing)
                            
                            curvePoints!.append(lastPoint!.point)
                            curvePoints!.append(orbitPoint!.point)
                            curvePoints!.append(firstPoint!.point)
                        } else {
                            curvePoints = orbitPoint!.descriptionLineForOrbit(RouteManager.defaultNumberOfPoints)
                        }
                        
                        let polyline = createCirclePolyline(curvePoints)
                        
                        returnArray.append(polyline)
                    }
                }
            }
            
            //Add the last point (the standalone orbit point)
            //Construct imaginary points a and c in initialBearing and 180-initialBearing
            let standalonePoint = rawRoute[rawRoute.count - 1]
            
            let imagA: Waypoint!
            if (rawRoute.count == 1) {
                let imagAPoint = standalonePoint.waypoint.waypointWithDistanceAndBearing(10000, bearing: standalonePoint.initialBearing! - 180)
                imagA = Waypoint(thePoint: imagAPoint, theOrbit: nil)
            } else {
                imagA = rawRoute[rawRoute.count - 2].waypoint
            }
            //Go +90 degrees for the center of a clockwise turn, -90 for a countercockwise
            let thePoint: Waypoint.Point
            if(standalonePoint.clockwise == true) {
                thePoint = standalonePoint.waypoint.waypointWithDistanceAndBearing(standalonePoint.radius!, bearing: imagA.bearingTo(standalonePoint.waypoint) + 90)
            } else {
                thePoint = standalonePoint.waypoint.waypointWithDistanceAndBearing(standalonePoint.radius!, bearing: imagA.bearingTo(standalonePoint.waypoint) - 90)
            }
            
            //Turn it into an orbit Waypoint
            let theOrbit = Waypoint.Orbit(radius: standalonePoint.radius, orbitUntilAltitude: true, clockwise: standalonePoint.clockwise!)
            let orbitPoint = Waypoint(thePoint: thePoint, theOrbit: theOrbit)
            
            //Construct its polyline and add it
            let curvePoints = orbitPoint!.descriptionLineForOrbit(RouteManager.defaultNumberOfPoints)
            
            returnArray.append(createCirclePolyline(curvePoints))
            
            return returnArray
        }
    }
    
    func createRouteToUpload() -> Array<Waypoint!>? {
        if(rawRoute.count == 0) {
            return nil
        } else {
            var returnArray = Array<Waypoint!>()
            
            //Add the middle waypoints
            if(rawRoute.count >= 2) {
                let firstWaypoint = Waypoint(thePoint: rawRoute[0].waypoint.point, theOrbit: nil)
                returnArray.append(firstWaypoint!)
            }
            
            if(rawRoute.count >= 3) {
                
                for(var i = 1; i < (rawRoute.count - 1); i++) {
                    let annotatedWp = rawRoute[i]
                    let wpA = returnArray[returnArray.count - 1]
                    let wpB = rawRoute[i].waypoint
                    let wpC = rawRoute[i+1].waypoint
                    
                    let (firstPoint, orbitPoint, lastPoint) = wpB.tangentialSegment(wpA, pointC: wpC, radius: annotatedWp.radius!)
                    if(firstPoint != nil && orbitPoint != nil && lastPoint != nil) {
                        firstPoint!.orbit = nil
                        returnArray.append(firstPoint!)
                        
                        if(annotatedWp.orbitUntilAltitude == false) {
                            orbitPoint!.orbit = Waypoint.Orbit(radius: orbitPoint!.orbit!.radius!, orbitUntilAltitude: false, clockwise: nil)
                        } else {
                            orbitPoint!.orbit = Waypoint.Orbit(radius: orbitPoint!.orbit!.radius!, orbitUntilAltitude: true, clockwise: nil)
                        }
                        returnArray.append(orbitPoint!)
                        
                        lastPoint?.orbit = nil
                        returnArray.append(lastPoint!)
                    } else {
                        return nil
                    }
                }
            }
            
            //Add the last point (the standalone orbit point)
            //Construct imaginary points a and c in initialBearing and 180-initialBearing
            let standalonePoint = rawRoute[rawRoute.count - 1]
            
            let imagA: Waypoint!
            if (rawRoute.count == 1) {
                let imagAPoint = standalonePoint.waypoint.waypointWithDistanceAndBearing(10000, bearing: standalonePoint.initialBearing! - 180)
                imagA = Waypoint(thePoint: imagAPoint, theOrbit: nil)
            } else {
                imagA = rawRoute[rawRoute.count - 2].waypoint
            }
            //Go +90 degrees for the center of a clockwise turn, -90 for a countercockwise
            let thePoint: Waypoint.Point
            if(standalonePoint.clockwise == true) {
                thePoint = standalonePoint.waypoint.waypointWithDistanceAndBearing(standalonePoint.radius!, bearing: imagA.bearingTo(standalonePoint.waypoint) + 90)
            } else {
                thePoint = standalonePoint.waypoint.waypointWithDistanceAndBearing(standalonePoint.radius!, bearing: imagA.bearingTo(standalonePoint.waypoint) - 90)
            }
            
            //Turn it into an orbit Waypoint
            let theOrbit = Waypoint.Orbit(radius: standalonePoint.radius, orbitUntilAltitude: nil, clockwise: standalonePoint.clockwise!)
            let orbitPoint = Waypoint(thePoint: thePoint, theOrbit: theOrbit)
            
            //Append it
            returnArray.append(orbitPoint!)
            
            return returnArray
        }
    }
    
    func createCirclePolyline(curvePoints: Array<Waypoint.Point!>?) ->MKPolyline! {
        var coords = Array<CLLocationCoordinate2D>()
        
        for p in curvePoints! {
            let coordinate = CLLocationCoordinate2D(latitude: p.latitude, longitude: p.longitude)
            coords.append(coordinate)
        }
        
        let polyline = MKPolyline(coordinates: &coords, count: coords.count)
        
        return polyline
    }
    
    func createUnSmoothPolyLine() -> MKPolyline? {
        if(rawRoute.count == 0) {
            return nil
        }
        
        var coords = Array<CLLocationCoordinate2D>()
        
        for wp in rawRoute {
            let point = wp.waypoint.point
            let coordinate = CLLocationCoordinate2D(latitude: point.latitude, longitude: point.longitude)
            coords.append(coordinate)
        }
        
        let polyline = MKPolyline(coordinates: &coords, count: coords.count)
        return polyline
    }
}