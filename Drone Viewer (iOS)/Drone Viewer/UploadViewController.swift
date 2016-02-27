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
    
    override func viewWillAppear(animated: Bool) {
        if(routeManager != nil) {
            //Get the route as an array
            let routeToUpload: Array<Waypoint!>? = routeManager!.createRouteToUpload()
            
            
            //Create the description
            if(routeToUpload == nil) {
                buUpload.enabled = false
                buUpload.highlighted = true
                tfRouteDescription.text = "Could not create route. Please verify this route does not contain any bends without corresponding circle segements in the map view"
            } else {
                var message: String = NSString(format: "Route contains %i waypoints.\n", routeToUpload!.count) as String
                for (var i = 0; i < routeToUpload!.count; i++) {
                    let appendString = NSString(format: "Waypoint %i: Altitude %.0f", i+1, routeToUpload![i].point.altitude) as String
                    message.appendContentsOf(appendString)
                    
                    let secondAppendString: String!
                    if(routeToUpload![i].orbit != nil) {
                        secondAppendString = NSString(format: ", Orbit radius: %.0f\n", routeToUpload![i].orbit!.radius) as String
                    } else {
                        secondAppendString = "\n"
                    }
                    message.appendContentsOf(secondAppendString)
                }
                tfRouteDescription.text = message
            }
            
            
            if(routeToUpload != nil) {
                //Create the drone message
                messageToSend = DroneMessage.init()
                
                for wp in routeToUpload! {
                    messageToSend!.routeArray.addObject(wp.toProtobuf())
                }
            }
            
        } else {
            buUpload.enabled = false
            buUpload.highlighted = true
            tfRouteDescription.text = "Soething went wrong (routeManager nil)"
        }
    }
    
    @IBAction func buUploadPressed(sender: AnyObject) {
        //Get teh Inet Manager
        if let appDelegate = UIApplication.sharedApplication().delegate as? AppDelegate {
            let inetComms = appDelegate.inetComms
            inetComms!.sendMessage(messageToSend!)
        }
    }
    
    @IBAction func buCancelPressed(sender: AnyObject) {
        self.dismissViewControllerAnimated(true, completion: nil)
    }
}
