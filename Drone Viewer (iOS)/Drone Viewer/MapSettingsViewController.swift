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
    
    override func viewWillAppear(_ animated: Bool) {
        if(selectedMapType == MKMapType.hybridFlyover) {
            mapSatelliteControl.selectedSegmentIndex = 1
        } else {
            mapSatelliteControl.selectedSegmentIndex = 0
        }
        
        swCenteringMode.setOn(overlayController.centerOnCurrentPosition, animated: false)
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        //segue for the popover configuration window
        if segue.identifier == "UploadControllerSegue" {
            if let controller = segue.destination as? UploadViewController {
                controller.routeManager = overlayController.routeManager!
            }
        }
    }
    
    @IBAction func segmentedControlChanged(_ sender: AnyObject) {
        switch mapSatelliteControl.selectedSegmentIndex {
        case 0:
            mapViewController.changeToMap()
        case 1:
            mapViewController.changeToSatellite()
        default:
            Logger.log("Invalid status for degmented control!")
        }
        self.dismiss(animated: true, completion: nil)
    }
    
    
    @IBAction func centeringModeSwitchChanged(_ sender: AnyObject) {
        overlayController.centerOnCurrentPosition = swCenteringMode.isOn
        self.dismiss(animated: true, completion: nil)
    }
    
    @IBAction func buClearRotePressed(_ sender: AnyObject) {
        mapViewController.clearRoute()
        self.dismiss(animated: true, completion: nil)
    }
    
    @IBAction func clearTrack(_ sender: AnyObject) {
        if let tc = (UIApplication.shared.delegate as? AppDelegate)?.trackCreator {
            tc.clearTrack()
            self.dismiss(animated: true, completion: nil)
        } else {
            Logger.log("getting the track creator failed!")
        }
    }
}
