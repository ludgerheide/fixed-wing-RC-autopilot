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
    
    @IBOutlet var laConnection: UILabel!
    @IBOutlet var laAltitude: UILabel!
    @IBOutlet var laBattery: UILabel!
    
    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
        NSNotificationCenter.defaultCenter().addObserver(
            self,
            selector: "newDroneMessageReceived:",
            name: InetInterface.notificationName,
            object: nil)
    }
    
    @objc func newDroneMessageReceived(notification: NSNotification){
        if let theMessage: DroneMessage = notification.object as? DroneMessage {
            print(round(Double(theMessage.timestamp) / 1000))
        } else {
            print("Received a message that is not a DroneMessage!");
        }
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
            }
        }
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
    }
    */

}
