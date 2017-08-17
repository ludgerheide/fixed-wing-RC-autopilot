//
//  TrackCreator.swift
//  Drone Viewer
//
//  Created by Ludger Heide on 02.02.16.
//  Copyright © 2016 Ludger Heide. All rights reserved.
//

import UIKit
import MapKit

class TrackCreator: NSObject, NSCoding {
    fileprivate let countKey = "countKey"
    
    static let notificationName = "newPolyLine"
    fileprivate var coordinates: Array<CLLocationCoordinate2D>
    
    override init() {
        coordinates = Array<CLLocationCoordinate2D>()
        super.init()
        NotificationCenter.default.addObserver(
            self,
            selector: #selector(TrackCreator.newDroneMessageReceived(_:)),
            name: NSNotification.Name(rawValue: InetInterface.notificationName),
            object: nil)
    }
    
    convenience init(withArray array: Array<CLLocationCoordinate2D>) {
        self.init()
        
        coordinates = array
    }
    
    required convenience init(coder aDecoder: NSCoder) {
        self.init()
        
        let nbCounter = aDecoder.decodeInteger(forKey: countKey)
        
        for _ in 0 ..< nbCounter {
            if let loc = aDecoder.decodeObject() as? CLLocation {
                coordinates.append(loc.coordinate)
            }
        }
    }
    
    func encode(with aCoder: NSCoder) {
        aCoder.encode(coordinates.count, forKey: countKey)
        
        for loc in coordinates {
            let newCoord = CLLocation.init(latitude: loc.latitude, longitude: loc.longitude)
            aCoder.encode(newCoord)
        }
    }
    
    func getPolyLine() -> MKPolyline? {
        if(coordinates.count > 0) {
            return MKPolyline(coordinates: &coordinates, count: coordinates.count)
        } else {
            return nil
        }
    }
    
    func getLatestCoordinates() -> CLLocationCoordinate2D? {
        return coordinates.last
    }
    
    func postNotification() {
        let notificationCenter = NotificationCenter.default
        let theNotification: Notification = Notification.init(name: Notification.Name(rawValue: TrackCreator.notificationName), object: nil)
        notificationCenter.post(theNotification)
    }
    
    func clearTrack() {
        coordinates.removeAll(keepingCapacity: true)
        postNotification()
    }
    
    @objc func newDroneMessageReceived(_ notification: Notification){
        if let theMessage: DroneMessage = notification.object as? DroneMessage {
            if(theMessage.hasCurrentPosition) {
                let newCoord = CLLocationCoordinate2D.init(latitude: CLLocationDegrees(theMessage.currentPosition.latitude), longitude: CLLocationDegrees(theMessage.currentPosition.longitude))
                if(coordinates.last == nil || (newCoord.latitude != coordinates.last!.latitude || newCoord.longitude != coordinates.last?.longitude)) {
                    coordinates.append(newCoord)
                    
                    postNotification()
                }
                
            } else {
                Logger.log("Received a message without position")
            }
        } else {
            Logger.log("Received a message that is not a DroneMessage!")
        }
    }
}
