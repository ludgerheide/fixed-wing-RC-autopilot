//
//  RouteManager.swift
//  Drone Viewer
//
//  Created by Ludger Heide on 14.02.16.
//  Copyright © 2016 Ludger Heide. All rights reserved.
//

import MapKit

class RouteManager: NSObject, NSCoding {
    private static let defaultRadius: Double = 50
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
        
        @objc var title: String?
        @objc var subtitle: String?
        
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
        
        //WARING: NSCoding only PARTIALLY IMPLEMENTED
        private let latKey = "lat"
        private let lonKey = "lon"
        private let altKey = "alt"
        
        private let orbitKey = "orbit"
        private let radiusKey = "rad"
        private let initialBearingKey = "initBear"
        private let clockwiseKey = "clockwise"
        
        required convenience init(coder aDecoder: NSCoder) {
            self.init()
            
            let latitude = aDecoder.decodeDoubleForKey(latKey)
            let longitude = aDecoder.decodeDoubleForKey(lonKey)
            let altitude = aDecoder.decodeDoubleForKey(altKey)
            
            if(aDecoder.decodeBoolForKey(orbitKey) == true) {
                radius = aDecoder.decodeDoubleForKey(radiusKey)
                initialBearing = aDecoder.decodeDoubleForKey(initialBearingKey)
                clockwise = aDecoder.decodeBoolForKey(clockwiseKey)
            }
            
            let thePoint = Waypoint.Point(latitude: latitude, longitude: longitude, altitude: altitude)
            waypoint = Waypoint(thePoint: thePoint, theOrbit: nil)
        }
        
        func encodeWithCoder(aCoder: NSCoder) {
            aCoder.encodeDouble(waypoint.point.latitude, forKey: latKey)
            aCoder.encodeDouble(waypoint.point.longitude, forKey: lonKey)
            aCoder.encodeDouble(waypoint.point.altitude, forKey: altKey)
            
            if(radius != nil && initialBearing != nil && clockwise != nil) {
                aCoder.encodeBool(true, forKey: orbitKey)
                
                aCoder.encodeDouble(radius!, forKey: radiusKey)
                aCoder.encodeDouble(initialBearing!, forKey: initialBearingKey)
                aCoder.encodeBool(clockwise!, forKey: clockwiseKey)
            } else {
                aCoder.encodeBool(false, forKey: orbitKey)
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
    
    func addPoint(thePoint: WaypointWithAnnotations!) {
        rawRoute.append(thePoint)
    }
    
    func removePoint(thePoint: WaypointWithAnnotations!) -> Bool! {
        if let index = rawRoute.indexOf({$0.isEqual(thePoint)}) {
            rawRoute.removeAtIndex(index)
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
                    
                    let clockwiseString: String
                    if(currentWp.clockwise! == true) {
                        clockwiseString = "clockwise"
                    } else {
                        clockwiseString = "counter-clockwise"
                    }
                    let newSubtitle = NSString(format: "Altitude %.2f m\nBearing: %.0f° Direction: %s", currentWp.waypoint.point.altitude, currentWp.initialBearing!, clockwiseString)
                    currentWp.subtitle = newSubtitle as String
                } else {
                    currentWp.title = "Waypoint " + String(i+1)
                    let newSubtitle = NSString(format: "Altitude %.2f m", currentWp.waypoint.point.altitude)
                    currentWp.subtitle = newSubtitle as String
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
                    let curvePoints: Array<Waypoint.Point!>?
                    if(annotatedWp.orbitUntilAltitude == false) {
                        let fromBearing = orbitPoint!.bearingTo(firstPoint!)
                        let toBearing = orbitPoint!.bearingTo(lastPoint!)
                        curvePoints = orbitPoint!.descriptionLineForOrbitSegment(RouteManager.defaultNumberOfPoints, fromBearing: fromBearing, toBearing: toBearing)
                        
                        curvePoints!.append(firstPoint!.point)
                        curvePoints!.append(orbitPoint!.point)
                        curvePoints!.append(lastPoint!.point)
                    } else {
                        curvePoints = orbitPoint!.descriptionLineForOrbit(RouteManager.defaultNumberOfPoints)
                    }
                    
                    let polyline = createCirclePolyline(curvePoints)
                    
                    returnArray.append(polyline)
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