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
    private let countKey = "countKey"
    
    static let notificationName = "newPolyLine"
    private var coordinates: Array<CLLocationCoordinate2D>
    
    override init() {
        coordinates = Array<CLLocationCoordinate2D>()
        super.init()
        NSNotificationCenter.defaultCenter().addObserver(
            self,
            selector: "newDroneMessageReceived:",
            name: InetInterface.notificationName,
            object: nil)
    }
    
    convenience init(withArray array: Array<CLLocationCoordinate2D>) {
        self.init()
        
        coordinates = array
    }
    
    required convenience init(coder aDecoder: NSCoder) {
        self.init()       
        
        let nbCounter = aDecoder.decodeIntegerForKey(countKey)
        
        for _ in 0 ..< nbCounter {
            if let loc = aDecoder.decodeObject() as? CLLocation {
                coordinates.append(loc.coordinate)
            }
        }
    }
    
    func encodeWithCoder(aCoder: NSCoder) {
        aCoder.encodeInteger(coordinates.count, forKey: countKey)
        
        for loc in coordinates {
            let newCoord = CLLocation.init(latitude: loc.latitude, longitude: loc.longitude)
            aCoder.encodeObject(newCoord)
        }
    }
    
    func getPolyLine() -> MKPolyline {
        return MKPolyline(coordinates: &coordinates, count: coordinates.count)
    }
    
    @objc func newDroneMessageReceived(notification: NSNotification){
        if let theMessage: DroneMessage = notification.object as? DroneMessage {
            if(theMessage.hasCurrentPosition) {
                let newCoord = CLLocationCoordinate2D.init(latitude: CLLocationDegrees(theMessage.currentPosition.latitude), longitude: CLLocationDegrees(theMessage.currentPosition.longitude))
                if(coordinates.last != nil && (newCoord.latitude != coordinates.last!.latitude || newCoord.longitude != coordinates.last?.longitude)) {
                    coordinates.append(newCoord)
                    
                    let notificationCenter = NSNotificationCenter.defaultCenter()
                    let theNotification: NSNotification = NSNotification.init(name: TrackCreator.notificationName, object: nil)
                    notificationCenter.postNotification(theNotification)
                }
                
            } else {
                print("Received a message without position")
            }
        } else {
            print("Received a message that is not a DroneMessage!")
        }
    }
}
