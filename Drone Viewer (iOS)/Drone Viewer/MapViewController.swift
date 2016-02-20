//
//  MapViewController.swift
//  Drone Viewer
//
//  Created by Ludger Heide on 13.01.16.
//  Copyright © 2016 Ludger Heide. All rights reserved.
//

import UIKit
import MapKit

class MapViewController: UIViewController, UIPopoverPresentationControllerDelegate {
    @IBOutlet var mapView: MKMapView!
    var overlayController: MapOverlayController!
    
    @IBOutlet var laConnection: UILabel!
    @IBOutlet var laAltitude: UILabel!
    @IBOutlet var laBattery: UILabel!
    var labelTimeoutTimer: NSTimer?
    let labelTimeOut: NSTimeInterval = 5
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Do any additional setup after loading the view.
        NSNotificationCenter.defaultCenter().addObserver(
            self,
            selector: "newDroneMessageReceived:",
            name: InetInterface.notificationName,
            object: nil)
        updateLabels(nil)
        
        //Initialize the overlay handler
        overlayController = MapOverlayController.init(mv: mapView, vc: self)
    }
    
    override func viewWillAppear(animated: Bool) {
        //Make the overly controller plot the route and track
        overlayController.newMapUpdateReady(NSNotification(name: "dummy", object: nil))
        overlayController.redrawRouteComplete()
    }
    
    @objc func newDroneMessageReceived(notification: NSNotification){
        if let theMessage: DroneMessage = notification.object as? DroneMessage {
            Logger.log(Double(theMessage.timestamp) / 1000)
            updateLabels(theMessage)
        } else {
            Logger.log("Received a message that is not a DroneMessage!");
        }
    }
    
    func updateLabels(msg: DroneMessage?) {
        if let localMsg = msg {
            if(localMsg.hasCurrentPosition) {
                laConnection.text = "Connected"
                laConnection.textColor = UIColor.greenColor()
            } else {
                laConnection.text = "No position"
                laConnection.textColor = UIColor.orangeColor()
            }
            
            let altitude = Double(localMsg.currentAltitude)/100
            laAltitude.text = String(format: "%3.1f m", arguments: [altitude])
            laAltitude.textColor = UIColor.blackColor()
            
            let voltage = Double(localMsg.currentBatteryData.voltage) / 1000
            let current = Double(localMsg.currentBatteryData.current) / 1000
            laBattery.text = String(format: "%2.1fV %2.1fA", arguments: [voltage, current])
            laBattery.textColor = UIColor.blackColor()
            
            labelTimeoutTimer?.invalidate()
            labelTimeoutTimer = NSTimer.scheduledTimerWithTimeInterval(labelTimeOut, target: self, selector: "clearLabels", userInfo: nil, repeats: false)
        } else {
            laConnection.text = "No Data!"
            laConnection.textColor = UIColor.redColor()
            
            laAltitude.text = "N/A"
            laAltitude.textColor = UIColor.redColor()
            
            laBattery.text = "N/A"
            laBattery.textColor = UIColor.redColor()
        }
    }
    
    func clearLabels() {
        updateLabels(nil)
    }
    
    func changeToMap() {
        mapView.mapType = MKMapType.Standard
        
    }
    
    func changeToSatellite() {
        mapView.mapType = MKMapType.HybridFlyover
    }
    
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        //segue for the popover configuration window
        if segue.identifier == "MapSettingsPopoverSegue" {
            if let controller = segue.destinationViewController as? MapSettingsViewController {
                controller.popoverPresentationController!.delegate = self
                controller.preferredContentSize = controller.view.systemLayoutSizeFittingSize(CGSizeMake(0, 0))
                controller.mapViewController = self
                controller.selectedMapType = mapView.mapType
                controller.overlayController = self.overlayController
            }
        }
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
}
