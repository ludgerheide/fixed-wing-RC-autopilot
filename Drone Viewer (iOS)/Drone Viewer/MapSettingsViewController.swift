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
    var mapViewController: MapViewController!
    var selectedMapType: MKMapType?
    
    override func viewWillAppear(animated: Bool) {
        if(selectedMapType == MKMapType.HybridFlyover) {
            mapSatelliteControl.selectedSegmentIndex = 1
        } else {
            mapSatelliteControl.selectedSegmentIndex = 0
        }
    }
    
    @IBAction func segmentedControlChanged(sender: AnyObject) {
        switch mapSatelliteControl.selectedSegmentIndex {
        case 0:
            mapViewController.changeToMap()
        case 1:
            mapViewController.changeToSatellite()
        default:
            print("Invalid status for degmented control!")
        }
        self.dismissViewControllerAnimated(true, completion: nil)
    }
}
