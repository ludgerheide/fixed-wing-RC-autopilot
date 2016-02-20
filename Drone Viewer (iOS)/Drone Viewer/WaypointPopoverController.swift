import UIKit
import MapKit

class WaypointPopOverController: UIViewController {

    @IBOutlet var tfAltitude: UITextField!
    @IBOutlet var tfRadius: UITextField!
    @IBOutlet var svInitialBearing: UIStackView!
    @IBOutlet var tfInitialBearing: UITextField!
    @IBOutlet var laInitialBearing1: UILabel!
    @IBOutlet var laInitialBearing2: UILabel!
    @IBOutlet var svClockwise: UIStackView!
    @IBOutlet var buClockwise: UISegmentedControl!
    @IBOutlet var laClockwise: UILabel!
    
    var waypoint: RouteManager.WaypointWithAnnotations?
    var controller: MapOverlayController?
    
    override func viewWillAppear(animated: Bool) {
        tfAltitude.text = NSString(format: "%.0f", waypoint!.waypoint.point.altitude) as String
        tfRadius.text = NSString(format: "%.0f", waypoint!.radius!) as String

        if let clockwise = waypoint!.clockwise {
            if(clockwise == true) {
                buClockwise.selectedSegmentIndex = 0
            } else {
                buClockwise.selectedSegmentIndex = 1
            }
        } else {
            buClockwise.enabled = false
            laClockwise.textColor = UIColor.grayColor()
        }
        
        if let initialBearing = waypoint!.initialBearing {
            tfInitialBearing.text = NSString(format: "%.0f", initialBearing) as String
        } else {
            tfInitialBearing.enabled = false
            laInitialBearing1.textColor = UIColor.grayColor()
            laInitialBearing2.textColor = UIColor.grayColor()
        }
    }
    
    override func viewWillDisappear(animated: Bool) {
        tfAltitudeChanged(self)
        tfRadiusChanged(self)
        
        if(waypoint!.initialBearing != nil) {
            tfInitialBearingChanged(self)
        }
    }
    
    override func viewDidDisappear(animated: Bool) {
        controller!.popoverCompleted(self.waypoint!)
    }
    
    @IBAction func tfAltitudeChanged(sender: AnyObject) {
        let newAltitude = Double(tfAltitude.text!)
        
        if(newAltitude != nil) {
        waypoint!.waypoint.point.altitude = newAltitude
        } else {
            showInvalidNumberAlert()
            tfAltitude.text = NSString(format: "%.0f", waypoint!.waypoint.point.altitude) as String
        }
    }
    
    @IBAction func tfRadiusChanged(sender: AnyObject) {
        let newRadius = Double(tfRadius.text!)
        
        if(newRadius != nil && newRadius > 0) {
            waypoint!.radius = newRadius
        } else {
            showInvalidNumberAlert()
            tfRadius.text = NSString(format: "%.0f", waypoint!.radius!) as String
        }
    }
    
    @IBAction func buClockwiseChanged(sender: AnyObject) {
        switch buClockwise.selectedSegmentIndex {
        case 0:
            waypoint!.clockwise = true
        case 1:
            waypoint!.clockwise = false
        default:
            Logger.log("Invalid status for degmented control!")
        }
    }
    
    @IBAction func tfInitialBearingChanged(sender: AnyObject) {
        let newInitialBearing = Double(tfInitialBearing.text!)
        
        if(newInitialBearing != nil && newInitialBearing > 0 && newInitialBearing <= 360) {
            waypoint!.initialBearing = newInitialBearing
        } else {
            showInvalidNumberAlert()
            tfInitialBearing.text = NSString(format: "%.0f", waypoint!.initialBearing!) as String
        }
    }
    
    func showInvalidNumberAlert() {
        let alert = UIAlertController(title: "Invalid radius!", message: "Please enter valid value!", preferredStyle: UIAlertControllerStyle.Alert)
        alert.addAction(UIAlertAction(title: "OK", style: UIAlertActionStyle.Default, handler: nil))
        self.presentViewController(alert, animated: true, completion: nil)
    }
}
