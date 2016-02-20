//
//  MapOverlayController.swift
//  Drone Viewer
//
//  Created by Ludger Heide on 03.02.16.
//  Copyright © 2016 Ludger Heide. All rights reserved.
//

import UIKit
import MapKit

class MapOverlayController: NSObject, MKMapViewDelegate {
    private static let DocumentsDirectory = NSFileManager().URLsForDirectory(.DocumentDirectory, inDomains: .UserDomainMask).first!
    private static let routeURL = DocumentsDirectory.URLByAppendingPathComponent("route")
    
    private var parentViewController: UIViewController!
    
    private var mapView: MKMapView!
    
    private var trackPolyLine: MKPolyline?
    internal var centerOnCurrentPosition: Bool! = false
    
    private var routeManager: RouteManager!
    private var routePolylines: [MKOverlay]?
    private var routePins: [MKAnnotation]?
    
    override init() {
        if let rm = NSKeyedUnarchiver.unarchiveObjectWithFile(MapOverlayController.routeURL.path!) as? RouteManager {
            routeManager = rm
        } else {
            routeManager = RouteManager()
        }
        
        super.init()
        
        NSNotificationCenter.defaultCenter().addObserver(
            self,
            selector: "newMapUpdateReady:",
            name: TrackCreator.notificationName,
            object: nil)
        
        //Register entering backgroudn to save track
        NSNotificationCenter.defaultCenter().addObserver(
            self,
            selector: "saveRoute",
            name: UIApplicationDidEnterBackgroundNotification,
            object: nil)
    }
    
    convenience init(mv: MKMapView, vc: UIViewController) {
        self.init()
        mapView = mv
        mapView.delegate = self
        
        parentViewController = vc
        
        addTestPoints()
    }
    
    func saveRoute() {
        let isSuccessfulSave = NSKeyedArchiver.archiveRootObject(routeManager, toFile: MapOverlayController.routeURL.path!)
        if !isSuccessfulSave {
            Logger.log("Failed to save route…")
        } else {
            Logger.log("Saved route!")
        }
    }
    
    func redrawRouteComplete() {
        redrawRouteAnnotations()
        redrawRoutePolylines()
    }
    
    func redrawRouteAnnotations() {
        if(routePins != nil) {
            mapView.removeAnnotations(routePins!)
        }
        
        if let pointsArray = routeManager.getAnnotatedWaypoints() {
            routePins = [MKAnnotation]()
            for point in pointsArray {
                routePins!.append(point)
            }
            
            mapView.addAnnotations(routePins!)
        }
    }
    
    func redrawRoutePolylines() {
        if(routePolylines != nil) {
            mapView.removeOverlays(routePolylines!)
        }
        
        if let polylines = routeManager.createSmoothedPolyLines() {
            routePolylines = [MKOverlay]()
            for line in polylines {
                routePolylines!.append(line)
            }
            
            mapView.addOverlays(routePolylines!)
        }
    }
    
    @objc func newMapUpdateReady(notification: NSNotification) {
        assert(notification.object == nil)
        
        if let tc = (UIApplication.sharedApplication().delegate as? AppDelegate)?.trackCreator {
            //Delete the old polyline, if it exists
            if(trackPolyLine != nil) {
                mapView.removeOverlay(trackPolyLine!)
            }
            
            trackPolyLine = tc.getPolyLine()
            if(trackPolyLine != nil) {
                mapView.addOverlay(trackPolyLine!)
            }
            
            //Center on thelatest coordinate if enabled
            if(centerOnCurrentPosition == true) {
                if let currentLocation = tc.getLatestCoordinates() {
                    if(!MKMapRectContainsPoint(mapView.visibleMapRect, MKMapPointForCoordinate(currentLocation))) {
                        mapView.setCenterCoordinate(currentLocation, animated: true)
                    }
                }
            }
        } else {
            Logger.log("getting the track creator failed!")
        }
    }
    
    func mapView(mapView: MKMapView, rendererForOverlay overlay: MKOverlay) -> MKOverlayRenderer {
        let plRenderer = MKPolylineRenderer(polyline: overlay as! MKPolyline)
        
        //Render the track in red and the route in black
        if(overlay.isEqual(trackPolyLine)) {
            plRenderer.strokeColor = UIColor.redColor()
            plRenderer.lineWidth = 2.0
        } else {
            plRenderer.strokeColor = UIColor.blackColor()
            plRenderer.lineWidth = 2.0
        }
        
        return plRenderer
    }
    
    func mapView(mapView: MKMapView, viewForAnnotation annotation: MKAnnotation) -> MKAnnotationView? {
        // If the annotation is the user location, just return nil.
        if (annotation.isKindOfClass(MKUserLocation)) {
            return nil;
        }
        
        //Check if the annotation is part of the route (only kind for now, but maybe we get a plane icon later ;)
        if let _ = annotation as? RouteManager.WaypointWithAnnotations {
            //Define an identifier fir this kind of annotation
            let identifier = "route point"
            
            //Try to dequeue an existing view
            var view: MKPinAnnotationView
            if let dequeuedView = mapView.dequeueReusableAnnotationViewWithIdentifier(identifier)
                as? MKPinAnnotationView {
                    dequeuedView.annotation = annotation
                    view = dequeuedView
            } else {
                view = MKPinAnnotationView(annotation: annotation, reuseIdentifier: identifier)
                
                view.animatesDrop = false
                view.canShowCallout = true
                view.draggable = true
                
                //Add a button on the rightt that goes to the "edit" screen
                let rightButton = UIButton(type: UIButtonType.DetailDisclosure)
                rightButton.addTarget(nil, action: nil, forControlEvents: UIControlEvents.TouchUpInside)
                view.rightCalloutAccessoryView = rightButton
            }
            return view
        }
        return nil
    }
    
    func mapView(mapView: MKMapView, annotationView view: MKAnnotationView, calloutAccessoryControlTapped control: UIControl) {
        let storyboard = UIStoryboard(name: "Main", bundle: nil)
        let popoverController = storyboard.instantiateViewControllerWithIdentifier("WaypointPopover") as! WaypointPopOverController
        popoverController.modalPresentationStyle = UIModalPresentationStyle.Popover
        popoverController.popoverPresentationController?.sourceView = mapView
        popoverController.popoverPresentationController?.sourceRect = view.frame
        popoverController.preferredContentSize = CGSize(width: 270, height: 310)
        
        popoverController.waypoint = (view.annotation as! RouteManager.WaypointWithAnnotations)
        popoverController.controller = self
        
        parentViewController.presentViewController(popoverController, animated: true, completion: nil)
    }
    
    func popoverCompleted(wp: RouteManager.WaypointWithAnnotations!) {
        //Update the UI after the popover is done
        redrawRoutePolylines()
        
        mapView.removeAnnotation(wp as MKAnnotation)
        mapView.addAnnotation(wp as MKAnnotation)
    }
    
    func mapView(mapView: MKMapView, annotationView view: MKAnnotationView, didChangeDragState newState: MKAnnotationViewDragState, fromOldState oldState: MKAnnotationViewDragState) {
        if(oldState == MKAnnotationViewDragState.Dragging && newState == MKAnnotationViewDragState.Ending) {
            //Redraw the polyline, the annotation coordinate is synced automatically 
            redrawRoutePolylines()
        }
    }
    
    //TODO: Remove
    func addTestPoints() {
        routeManager = RouteManager() //Clears old points
        let rm = routeManager
        
        let i: Double = 50
        
        let wp1 = Waypoint(latitude: 0, longitude: 0, altitude: 100)
        var aWp = RouteManager.WaypointWithAnnotations()
        aWp.waypoint = wp1
        aWp.radius = 50000
        aWp.orbitUntilAltitude = nil
        aWp.initialBearing = nil
        aWp.clockwise = true
        rm!.addPoint(aWp)
        
        var last = wp1
        let max = 2
        for(var j = 1; j < max; j++) {
            let wp2Point = last!.waypointWithDistanceAndBearing(100000, bearing: i*Double(j) % 150)
            last = Waypoint(thePoint: wp2Point, theOrbit: nil)
            var bWp = RouteManager.WaypointWithAnnotations()
            bWp.waypoint = last
            bWp.radius = 50000
            bWp.orbitUntilAltitude = false
            bWp.initialBearing = nil
            bWp.clockwise = nil
            rm!.addPoint(bWp)
        }
        
        let wp4Point = last!.waypointWithDistanceAndBearing(100000, bearing: i*Double(max) % 150)
        let wp4 = Waypoint(thePoint: wp4Point, theOrbit: nil)
        var bWp = RouteManager.WaypointWithAnnotations()
        bWp.waypoint = wp4
        bWp.radius = 50000
        bWp.orbitUntilAltitude = true
        bWp.initialBearing = i
        bWp.clockwise = false
        rm!.addPoint(bWp)
    }
}