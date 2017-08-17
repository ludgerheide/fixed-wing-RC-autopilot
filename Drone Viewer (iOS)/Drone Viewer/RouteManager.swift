//
//  RouteManager.swift
//  Drone Viewer
//
//  Created by Ludger Heide on 14.02.16.
//  Copyright © 2016 Ludger Heide. All rights reserved.
//

import MapKit

class RouteManager: NSObject {
    fileprivate static let defaultRadius: Double = Waypoint.defaultRadius
    fileprivate static let defaultNumberOfPoints: UInt = 30
    fileprivate static let countKey = "countKey"
    
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
        
        override func isEqual(_ object: Any?) -> Bool {
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
        fileprivate let latKey = "lat"
        fileprivate let lonKey = "lon"
        fileprivate let altKey = "alt"
        
        //Optional values
        fileprivate let radiusPresentKey = "radPresent"
        fileprivate let radiusKey = "rad"
        
        fileprivate let orbitUntilAltitudePresentKey = "orbUntilAltPresent"
        fileprivate let orbitUntilAltitudeKey = "orbUntilAlt"
        
        fileprivate let initialBearingPresentKey = "initBearPresent"
        fileprivate let initialBearingKey = "initBear"
        
        fileprivate let clockwisePresentKey = "clockwisePresent"
        fileprivate let clockwiseKey = "clockwise"
        
        required convenience init(coder aDecoder: NSCoder) {
            self.init()
            
            //Init the reuqired values
            let latitude = aDecoder.decodeDouble(forKey: latKey)
            let longitude = aDecoder.decodeDouble(forKey: lonKey)
            let altitude = aDecoder.decodeDouble(forKey: altKey)
            
            let thePoint = Waypoint.Point(latitude: latitude, longitude: longitude, altitude: altitude)
            waypoint = Waypoint(thePoint: thePoint, theOrbit: nil)
            
            //The optional values
            if(aDecoder.decodeBool(forKey: radiusPresentKey) == true) {
                radius = aDecoder.decodeDouble(forKey: radiusKey)
            }
            
            if(aDecoder.decodeBool(forKey: orbitUntilAltitudePresentKey) == true) {
                orbitUntilAltitude = aDecoder.decodeBool(forKey: orbitUntilAltitudeKey)
            }
            
            if(aDecoder.decodeBool(forKey: initialBearingPresentKey) == true) {
                initialBearing = aDecoder.decodeDouble(forKey: initialBearingKey)
            }
            
            if(aDecoder.decodeBool(forKey: clockwisePresentKey) == true) {
                clockwise = aDecoder.decodeBool(forKey: clockwiseKey)
            }

        }
        
        func encodeWithCoder(_ aCoder: NSCoder) {
            //Code the required values
            aCoder.encode(waypoint.point.latitude, forKey: latKey)
            aCoder.encode(waypoint.point.longitude, forKey: lonKey)
            aCoder.encode(waypoint.point.altitude, forKey: altKey)
            
            
            //Code thje optional values if applicable
            if(radius != nil) {
                aCoder.encode(true, forKey: radiusPresentKey)
                aCoder.encode(radius!, forKey: radiusKey)
            } else {
                aCoder.encode(false, forKey: radiusPresentKey)
            }
            
            if(orbitUntilAltitude != nil) {
                aCoder.encode(true, forKey: orbitUntilAltitudePresentKey)
                aCoder.encode(orbitUntilAltitude!, forKey: orbitUntilAltitudeKey)
            } else {
                aCoder.encode(false, forKey: orbitUntilAltitudePresentKey)
            }
            
            if(initialBearing != nil) {
                aCoder.encode(true, forKey: initialBearingPresentKey)
                aCoder.encode(initialBearing!, forKey: initialBearingKey)
            } else {
                aCoder.encode(false, forKey: initialBearingPresentKey)
            }
            
            if(clockwise != nil) {
                aCoder.encode(true, forKey: clockwisePresentKey)
                aCoder.encode(clockwise!, forKey: clockwiseKey)
            } else {
                aCoder.encode(false, forKey: clockwisePresentKey)
            }
        }
    }
    
    fileprivate var rawRoute: Array<WaypointWithAnnotations?>!
    
    override init() {
        rawRoute = Array<WaypointWithAnnotations!>()
        super.init()
    }
    
    required convenience init(coder aDecoder: NSCoder) {
        self.init()
        
        let nbCounter = aDecoder.decodeInteger(forKey: RouteManager.countKey)
        
        for _ in 0 ..< nbCounter {
            if let wp = aDecoder.decodeObject() as? WaypointWithAnnotations {
                rawRoute.append(wp)
            }
        }
    }
    
    func encodeWithCoder(_ aCoder: NSCoder) {
        aCoder.encode(rawRoute.count, forKey: RouteManager.countKey)
        
        for wp in rawRoute {
            aCoder.encode(wp)
        }
    }
    
    func clearRoute() {
        rawRoute.removeAll(keepingCapacity: true)
    }
    
    func addPoint(_ thePoint: WaypointWithAnnotations!) {
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
            oldLastPoint?.orbitUntilAltitude = false
            
            oldLastPoint?.clockwise = nil
            oldLastPoint?.initialBearing = nil
        }
    }
    
    func removePoint(_ thePoint: WaypointWithAnnotations!) -> Bool! {
        if let index = rawRoute.index(where: {($0?.isEqual(thePoint))!}) {
            rawRoute.remove(at: index)
            //Change the new last waypoint into an orbit waypoint, if it exists
            if(rawRoute.count > 0) {
                let newLastPoint = rawRoute[rawRoute.count - 1]
                newLastPoint?.orbitUntilAltitude = nil
                if(newLastPoint?.radius == nil) {
                    newLastPoint?.radius = RouteManager.defaultRadius
                }
                if(newLastPoint?.clockwise == nil) {
                    newLastPoint?.clockwise = true
                }
            }
            //And give it an initial bearing if it is the only point
            if(rawRoute.count == 1) {
                let newLastPoint = rawRoute[rawRoute.count - 1]
                newLastPoint?.initialBearing = 0
            }
            return true
        } else {
            return false
        }
    }
    
    func getAnnotatedWaypoints() -> Array<WaypointWithAnnotations?>? {
        if(rawRoute.count == 0) {
            return nil
        } else {
            for i in 0 ..< rawRoute.count {
                let currentWp = rawRoute[i]
                
                if(i == (rawRoute.count - 1)) {
                    currentWp?.title = "Last Waypoint"
                } else {
                    currentWp?.title = "Waypoint " + String(i+1)
                }
            }
            return rawRoute
        }
    }
    
    func createSmoothedPolyLines() -> Array<MKPolyline?>? {
        if(rawRoute.count == 0) {
            return nil
        } else {
            var returnArray = Array<MKPolyline!>()
            returnArray.append(createUnSmoothPolyLine()!)
            
            //Now create a circle for each bend and one after the last point
            //Create a circle section for "orbit until alt = false" and a circle for "orbit until alt = true"
            if(rawRoute.count >= 3) {
                for i in 1 ..< (rawRoute.count - 1) {
                    let annotatedWp = rawRoute[i]
                    let wpA = rawRoute[i-1]?.waypoint
                    let wpB = rawRoute[i]?.waypoint
                    let wpC = rawRoute[i+1]?.waypoint
                    
                    let (firstPoint, orbitPoint, lastPoint) = (wpB?.tangentialSegment(wpA!, pointC: wpC!, radius: annotatedWp!.radius!))!
                    if(firstPoint != nil && orbitPoint != nil && lastPoint != nil) {
                        let curvePoints: Array<Waypoint.Point?>?
                        if(annotatedWp?.orbitUntilAltitude == false) {
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
                let imagAPoint = standalonePoint?.waypoint.waypointWithDistanceAndBearing(10000, bearing: (standalonePoint?.initialBearing!)! - 180)
                imagA = Waypoint(thePoint: imagAPoint, theOrbit: nil)
            } else {
                imagA = rawRoute[rawRoute.count - 2]?.waypoint
            }
            //Go +90 degrees for the center of a clockwise turn, -90 for a countercockwise
            let thePoint: Waypoint.Point
            if(standalonePoint?.clockwise == true) {
                thePoint = standalonePoint!.waypoint.waypointWithDistanceAndBearing(standalonePoint!.radius!, bearing: imagA.bearingTo(standalonePoint!.waypoint) + 90)
            } else {
                thePoint = standalonePoint!.waypoint.waypointWithDistanceAndBearing(standalonePoint!.radius!, bearing: imagA.bearingTo(standalonePoint!.waypoint) - 90)
            }
            
            //Turn it into an orbit Waypoint
            let theOrbit = Waypoint.Orbit(radius: standalonePoint?.radius, orbitUntilAltitude: true, clockwise: standalonePoint?.clockwise!)
            let orbitPoint = Waypoint(thePoint: thePoint, theOrbit: theOrbit)
            
            //Construct its polyline and add it
            let curvePoints = orbitPoint!.descriptionLineForOrbit(RouteManager.defaultNumberOfPoints)
            
            returnArray.append(createCirclePolyline(curvePoints))
            
            return returnArray
        }
    }
    
    func createRouteToUpload() -> Array<Waypoint?>? {
        if(rawRoute.count == 0) {
            return nil
        } else {
            var returnArray = Array<Waypoint!>()
            
            //Add the middle waypoints
            if(rawRoute.count >= 2) {
                let firstWaypoint = Waypoint(thePoint: rawRoute[0]?.waypoint.point, theOrbit: nil)
                returnArray.append(firstWaypoint!)
            }
            
            if(rawRoute.count >= 3) {
                
                for i in 1 ..< (rawRoute.count - 1) {
                    let annotatedWp = rawRoute[i]
                    let wpA = returnArray[returnArray.count - 1]
                    let wpB = rawRoute[i]?.waypoint
                    let wpC = rawRoute[i+1]?.waypoint
                    
                    let (firstPoint, orbitPoint, lastPoint) = wpB!.tangentialSegment(wpA!, pointC: wpC!, radius: annotatedWp!.radius!)
                    if(firstPoint != nil && orbitPoint != nil && lastPoint != nil) {
                        firstPoint!.orbit = nil
                        returnArray.append(firstPoint!)
                        
                        if(annotatedWp?.orbitUntilAltitude == false) {
                            orbitPoint!.orbit = Waypoint.Orbit(radius: orbitPoint!.orbit!.radius!, orbitUntilAltitude: false, clockwise: orbitPoint!.orbit!.clockwise)
                        } else {
                            orbitPoint!.orbit = Waypoint.Orbit(radius: orbitPoint!.orbit!.radius!, orbitUntilAltitude: true, clockwise: orbitPoint!.orbit!.clockwise)
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
                let imagAPoint = standalonePoint!.waypoint.waypointWithDistanceAndBearing(10000, bearing: standalonePoint!.initialBearing! - 180)
                imagA = Waypoint(thePoint: imagAPoint, theOrbit: nil)
            } else {
                imagA = rawRoute[rawRoute.count - 2]?.waypoint
            }
            //Go +90 degrees for the center of a clockwise turn, -90 for a countercockwise
            let thePoint: Waypoint.Point
            if(standalonePoint?.clockwise == true) {
                thePoint = standalonePoint!.waypoint.waypointWithDistanceAndBearing(standalonePoint!.radius!, bearing: imagA.bearingTo(standalonePoint!.waypoint) + 90)
            } else {
                thePoint = standalonePoint!.waypoint.waypointWithDistanceAndBearing(standalonePoint!.radius!, bearing: imagA.bearingTo(standalonePoint!.waypoint) - 90)
            }
            
            //Turn it into an orbit Waypoint
            let theOrbit = Waypoint.Orbit(radius: standalonePoint?.radius, orbitUntilAltitude: nil, clockwise: standalonePoint?.clockwise!)
            let orbitPoint = Waypoint(thePoint: thePoint, theOrbit: theOrbit)
            
            //If the route has more than one point, we need to append the "start" point of the final orbit as well so we don't go straight into orbit
            if(rawRoute.count > 1) {
                returnArray.append(standalonePoint?.waypoint)
            }
            
            //Append it
            returnArray.append(orbitPoint!)
            
            return returnArray
        }
    }
    
    func createCirclePolyline(_ curvePoints: Array<Waypoint.Point?>?) ->MKPolyline! {
        var coords = Array<CLLocationCoordinate2D>()
        
        for p in curvePoints! {
            let coordinate = CLLocationCoordinate2D(latitude: (p?.latitude)!, longitude: (p?.longitude)!)
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
            let point = wp?.waypoint.point
            let coordinate = CLLocationCoordinate2D(latitude: (point?.latitude)!, longitude: (point?.longitude)!)
            coords.append(coordinate)
        }
        
        let polyline = MKPolyline(coordinates: &coords, count: coords.count)
        return polyline
    }
}
