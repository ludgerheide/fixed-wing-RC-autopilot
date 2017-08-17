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
    var labelTimeoutTimer: Timer?
    let labelTimeOut: TimeInterval = 5
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Do any additional setup after loading the view.
        NotificationCenter.default.addObserver(
            self,
            selector: #selector(MapViewController.newDroneMessageReceived(_:)),
            name: NSNotification.Name(rawValue: InetInterface.notificationName),
            object: nil)
        updateLabels(nil)
        
        //Initialize the overlay handler
        overlayController = MapOverlayController.init(mv: mapView, vc: self)
    }
    
    override func viewWillAppear(_ animated: Bool) {
        //Make the overly controller plot the route and track
        overlayController.newMapUpdateReady(Notification(name: Notification.Name(rawValue: "dummy"), object: nil))
        overlayController.redrawRouteComplete()
    }
    
    @objc func newDroneMessageReceived(_ notification: Notification){
        if let theMessage: DroneMessage = notification.object as? DroneMessage {
            Logger.log(Double(theMessage.timestamp) / 1000)
            updateLabels(theMessage)
        } else {
            Logger.log("Received a message that is not a DroneMessage!");
        }
    }
    
    func updateLabels(_ msg: DroneMessage?) {
        if let localMsg = msg {
            if(localMsg.hasCurrentPosition) {
                laConnection.text = "Connected"
                laConnection.textColor = UIColor.green
            } else {
                laConnection.text = "No position"
                laConnection.textColor = UIColor.orange
            }
            
            let altitude = Double(localMsg.currentAltitude)/100
            laAltitude.text = String(format: "%3.1f m", arguments: [altitude])
            laAltitude.textColor = UIColor.black
            
            let voltage = Double(localMsg.currentBatteryData.voltage) / 1000
            let current = Double(localMsg.currentBatteryData.current) / 1000
            laBattery.text = String(format: "%2.1fV %2.1fA", arguments: [voltage, current])
            laBattery.textColor = UIColor.black
            
            labelTimeoutTimer?.invalidate()
            labelTimeoutTimer = Timer.scheduledTimer(timeInterval: labelTimeOut, target: self, selector: #selector(MapViewController.clearLabels), userInfo: nil, repeats: false)
        } else {
            laConnection.text = "No Data!"
            laConnection.textColor = UIColor.red
            
            laAltitude.text = "N/A"
            laAltitude.textColor = UIColor.red
            
            laBattery.text = "N/A"
            laBattery.textColor = UIColor.red
        }
    }
    
    @IBAction func buAddWaypointPressed(_ sender: AnyObject) {
        //Find the center of the map window
        let centerOfScreen = mapView.centerCoordinate
        let newWaypoint = Waypoint(latitude: centerOfScreen.latitude, longitude: centerOfScreen.longitude, altitude: Waypoint.defaultAltitude)
        let newAnnotatedWaypoint = RouteManager.WaypointWithAnnotations()
        newAnnotatedWaypoint.waypoint = newWaypoint
        newAnnotatedWaypoint.radius = Waypoint.defaultRadius
        newAnnotatedWaypoint.clockwise = true
        
        overlayController.routeManager.addPoint(newAnnotatedWaypoint)
        overlayController.redrawRouteComplete()
    }
    
    func clearRoute() {
        overlayController.routeManager.clearRoute()
        overlayController.redrawRouteComplete()
    }
    
    func clearLabels() {
        updateLabels(nil)
    }
    
    func changeToMap() {
        mapView.mapType = MKMapType.standard
        
    }
    
    func changeToSatellite() {
        mapView.mapType = MKMapType.hybridFlyover
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        //segue for the popover configuration window
        if segue.identifier == "MapSettingsPopoverSegue" {
            if let controller = segue.destination as? MapSettingsViewController {
                controller.popoverPresentationController!.delegate = self
                controller.preferredContentSize = controller.view.systemLayoutSizeFitting(CGSize(width: 0, height: 0))
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
