//
//  MapSettingsViewController.swift
//  Drone Viewer
//
//  Created by Ludger Heide on 31.01.16.
//  Copyright © 2016 Ludger Heide. All rights reserved.
//

import UIKit

class MapSettingsViewController: UIViewController {
    @IBOutlet var mapSatelliteControl: UISegmentedControl!
    var mapViewController: MapViewController!
    
    @IBAction func segmentedControlChanged(sender: AnyObject) {
        switch mapSatelliteControl.selectedSegmentIndex {
        case 0:
            mapViewController.changeToMap()
        case 1:
            mapViewController.changeToSatellite()
        default:
            print("Invalid status for degmented control!")
        }
    }
}
