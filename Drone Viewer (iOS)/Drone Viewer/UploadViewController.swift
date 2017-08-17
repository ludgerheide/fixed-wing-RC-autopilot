//
//  UploadViewController.swift
//  Drone Viewer
//
//  Created by Ludger Heide on 27.02.16.
//  Copyright © 2016 Ludger Heide. All rights reserved.
//

import UIKit

class UploadViewController: UIViewController{
    @IBOutlet var tfRouteDescription: UITextView!
    @IBOutlet var buUpload: UIButton!
    
    var routeManager: RouteManager?
    var messageToSend: DroneMessage?
    
    override func viewWillAppear(_ animated: Bool) {
        if(routeManager != nil) {
            //Get the route as an array
            let routeToUpload: Array<Waypoint?>? = routeManager!.createRouteToUpload()
            
            
            //Create the description
            if(routeToUpload == nil) {
                buUpload.isEnabled = false
                buUpload.alpha = 0.3
                tfRouteDescription.text = "Could not create route. Please verify this route does not contain any bends without corresponding circle segements in the map view"
            } else {
                var message: String = NSString(format: "Route contains %i waypoints.\n", routeToUpload!.count) as String
                for i in 0  ..< routeToUpload!.count {
                    let appendString = NSString(format: "Waypoint %i: Altitude %.0f", i+1, (routeToUpload![i]?.point.altitude)!) as String
                    message.append(appendString)
                    
                    let secondAppendString: String!
                    if(routeToUpload![i]?.orbit != nil) {
                        secondAppendString = NSString(format: ", Orbit radius: %.0f\n", (routeToUpload![i]?.orbit!.radius)!) as String
                    } else {
                        secondAppendString = "\n"
                    }
                    message.append(secondAppendString)
                }
                tfRouteDescription.text = message
            }
            
            
            if(routeToUpload != nil) {
                //Create the drone message
                messageToSend = DroneMessage.init()
                
                for wp in routeToUpload! {
                    messageToSend!.routeArray.add(wp!.toProtobuf()!)
                }
            }
            
        } else {
            buUpload.isEnabled = false
            buUpload.alpha = 0.3
            tfRouteDescription.text = "Soething went wrong (routeManager nil)"
        }
    }
    
    @IBAction func buUploadPressed(_ sender: AnyObject) {
        //Register for status update notifications
        NotificationCenter.default.addObserver(
            self,
            selector: #selector(UploadViewController.statusUpdateReceived),
            name: NSNotification.Name(rawValue: InetInterface.statusNotificationName),
            object: nil)
        
        //Get teh Inet Manager
        if let appDelegate = UIApplication.shared.delegate as? AppDelegate {
            let inetComms = appDelegate.inetComms
            inetComms!.sendMessage(messageToSend!)
        }
    }
    
    func statusUpdateReceived(_ notification: Notification) {
        if let statusCode = notification.object as? UInt {
            let event = Stream.Event(rawValue: statusCode)
            
            let alert: UIAlertController
            if(event == Stream.Event.hasSpaceAvailable) {
                alert = UIAlertController(title: "Success!", message: "Message sent successfully", preferredStyle: UIAlertControllerStyle.alert)
            } else {
                alert = UIAlertController(title: "Failure!", message: "Sending message failed!", preferredStyle: UIAlertControllerStyle.alert)
            }
            let button = UIAlertAction(title: "OK", style: UIAlertActionStyle.cancel, handler: nil)
            alert.addAction(button)
            self.present(alert, animated: true, completion: nil)
            
            NotificationCenter.default.removeObserver(self)
        }
    }
    
    @IBAction func buCancelPressed(_ sender: AnyObject) {
        self.dismiss(animated: true, completion: nil)
    }
}
