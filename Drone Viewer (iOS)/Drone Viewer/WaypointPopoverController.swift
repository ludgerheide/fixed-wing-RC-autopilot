import UIKit
import MapKit
// FIXME: comparison operators with optionals were removed from the Swift Standard Libary.
// Consider refactoring the code to use the non-optional operators.
fileprivate func < <T : Comparable>(lhs: T?, rhs: T?) -> Bool {
  switch (lhs, rhs) {
  case let (l?, r?):
    return l < r
  case (nil, _?):
    return true
  default:
    return false
  }
}

// FIXME: comparison operators with optionals were removed from the Swift Standard Libary.
// Consider refactoring the code to use the non-optional operators.
fileprivate func > <T : Comparable>(lhs: T?, rhs: T?) -> Bool {
  switch (lhs, rhs) {
  case let (l?, r?):
    return l > r
  default:
    return rhs < lhs
  }
}

// FIXME: comparison operators with optionals were removed from the Swift Standard Libary.
// Consider refactoring the code to use the non-optional operators.
fileprivate func <= <T : Comparable>(lhs: T?, rhs: T?) -> Bool {
  switch (lhs, rhs) {
  case let (l?, r?):
    return l <= r
  default:
    return !(rhs < lhs)
  }
}


class WaypointPopOverController: UIViewController {
    
    @IBOutlet var tfAltitude: UITextField!
    @IBOutlet var tfRadius: UITextField!
    @IBOutlet var swOrbitUntilAltitude: UISwitch!
    @IBOutlet var svInitialBearing: UIStackView!
    @IBOutlet var tfInitialBearing: UITextField!
    @IBOutlet var laInitialBearing1: UILabel!
    @IBOutlet var laInitialBearing2: UILabel!
    @IBOutlet var svClockwise: UIStackView!
    @IBOutlet var buClockwise: UISegmentedControl!
    @IBOutlet var laClockwise: UILabel!
    
    var waypoint: RouteManager.WaypointWithAnnotations?
    var controller: MapOverlayController?
    var routeManager: RouteManager?
    
    override func viewWillAppear(_ animated: Bool) {
        tfAltitude.text = NSString(format: "%.0f", waypoint!.waypoint.point.altitude) as String
        tfRadius.text = NSString(format: "%.0f", waypoint!.radius!) as String
        
        if let orbitUntilAltitude = waypoint!.orbitUntilAltitude {
            swOrbitUntilAltitude.setOn(orbitUntilAltitude, animated: false)
        } else {
            swOrbitUntilAltitude.isEnabled = false
        }
        
        if let clockwise = waypoint!.clockwise {
            if(clockwise == true) {
                buClockwise.selectedSegmentIndex = 0
            } else {
                buClockwise.selectedSegmentIndex = 1
            }
        } else {
            buClockwise.isEnabled = false
            laClockwise.textColor = UIColor.gray
        }
        
        if let initialBearing = waypoint!.initialBearing {
            tfInitialBearing.text = NSString(format: "%.0f", initialBearing) as String
        } else {
            tfInitialBearing.isEnabled = false
            laInitialBearing1.textColor = UIColor.gray
            laInitialBearing2.textColor = UIColor.gray
        }
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        if(waypoint != nil) {
            tfAltitudeChanged(self)
            tfRadiusChanged(self)
            
            if(waypoint!.initialBearing != nil) {
                tfInitialBearingChanged(self)
            }
        }
    }
    
    override func viewDidDisappear(_ animated: Bool) {
        controller!.popoverCompleted(self.waypoint)
    }
    
    @IBAction func tfAltitudeChanged(_ sender: AnyObject) {
        let newAltitude = Double(tfAltitude.text!)
        
        if(newAltitude != nil) {
            waypoint!.waypoint.point.altitude = newAltitude
        } else {
            showInvalidNumberAlert()
            tfAltitude.text = NSString(format: "%.0f", waypoint!.waypoint.point.altitude) as String
        }
    }
    
    @IBAction func tfRadiusChanged(_ sender: AnyObject) {
        let newRadius = Double(tfRadius.text!)
        
        if(newRadius != nil && newRadius > 0) {
            waypoint!.radius = newRadius
        } else {
            showInvalidNumberAlert()
            tfRadius.text = NSString(format: "%.0f", waypoint!.radius!) as String
        }
    }
    
    @IBAction func swOrbitUntilAltitudeChanged(_ sender: AnyObject) {
        waypoint!.orbitUntilAltitude = swOrbitUntilAltitude.isOn
    }
    
    @IBAction func buClockwiseChanged(_ sender: AnyObject) {
        switch buClockwise.selectedSegmentIndex {
        case 0:
            waypoint!.clockwise = true
        case 1:
            waypoint!.clockwise = false
        default:
            Logger.log("Invalid status for degmented control!")
        }
    }
    
    @IBAction func tfInitialBearingChanged(_ sender: AnyObject) {
        let newInitialBearing = Double(tfInitialBearing.text!)
        
        if(newInitialBearing != nil && newInitialBearing > 0 && newInitialBearing <= 360) {
            waypoint!.initialBearing = newInitialBearing
        } else {
            showInvalidNumberAlert()
            tfInitialBearing.text = NSString(format: "%.0f", waypoint!.initialBearing!) as String
        }
    }
    
    @IBAction func buDeletePressed(_ sender: AnyObject) {
        _ = routeManager!.removePoint(waypoint)
        waypoint = nil
        controller!.redrawRouteAnnotations()
        self.dismiss(animated: true, completion: nil)
    }
    
    @IBAction func buDismissPressed(_ sender: AnyObject) {
        self.dismiss(animated: true, completion: nil)
    }
    
    
    func showInvalidNumberAlert() {
        let alert = UIAlertController(title: "Invalid radius!", message: "Please enter valid value!", preferredStyle: UIAlertControllerStyle.alert)
        alert.addAction(UIAlertAction(title: "OK", style: UIAlertActionStyle.default, handler: nil))
        self.present(alert, animated: true, completion: nil)
    }
}
