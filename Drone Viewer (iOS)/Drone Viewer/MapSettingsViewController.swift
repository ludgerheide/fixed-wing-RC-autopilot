//
//  MapSettingsViewController.swift
//  Drone Viewer
//
//  Created by Ludger Heide on 31.01.16.
//  Copyright © 2016 Ludger Heide. All rights reserved.
//

import UIKit
import MapKit

class MapSettingsViewController: UIViewController {
    @IBOutlet var mapSatelliteControl: UISegmentedControl!
    var selectedMapType: MKMapType?
    
    var mapViewController: MapViewController!
    
    var overlayController: MapOverlayController!
    @IBOutlet var swCenteringMode: UISwitch!
    
    override func viewWillAppear(animated: Bool) {
        if(selectedMapType == MKMapType.HybridFlyover) {
            mapSatelliteControl.selectedSegmentIndex = 1
        } else {
            mapSatelliteControl.selectedSegmentIndex = 0
        }
        
        swCenteringMode.setOn(overlayController.centerOnCurrentPosition, animated: false)
    }
    
    @IBAction func segmentedControlChanged(sender: AnyObject) {
        switch mapSatelliteControl.selectedSegmentIndex {
        case 0:
            mapViewController.changeToMap()
        case 1:
            mapViewController.changeToSatellite()
        default:
            Logger.log("Invalid status for degmented control!")
        }
        self.dismissViewControllerAnimated(true, completion: nil)
    }
    
    
    @IBAction func centeringModeSwitchChanged(sender: AnyObject) {
        overlayController.centerOnCurrentPosition = swCenteringMode.on
        self.dismissViewControllerAnimated(true, completion: nil)
    }
    
    @IBAction func clearTrack(sender: AnyObject) {
        if let tc = (UIApplication.sharedApplication().delegate as? AppDelegate)?.trackCreator {
            tc.clearTrack()
            self.dismissViewControllerAnimated(true, completion: nil)
        } else {
            Logger.log("getting the track creator failed!")
        }
        
    }
}
