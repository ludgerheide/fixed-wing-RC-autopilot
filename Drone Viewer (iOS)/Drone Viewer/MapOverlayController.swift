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
    fileprivate static let DocumentsDirectory = FileManager().urls(for: .documentDirectory, in: .userDomainMask).first!
    fileprivate static let routeURL = DocumentsDirectory.appendingPathComponent("route")
    
    fileprivate var parentViewController: UIViewController!
    
    fileprivate var mapView: MKMapView!
    
    fileprivate var trackPolyLine: MKPolyline?
    internal var centerOnCurrentPosition: Bool! = false
    
    var routeManager: RouteManager!
    fileprivate var routePolylines: [MKOverlay]?
    fileprivate var routePins: [MKAnnotation]?
    
    override init() {
        if let rm = NSKeyedUnarchiver.unarchiveObject(withFile: MapOverlayController.routeURL.path) as? RouteManager {
            routeManager = rm
        } else {
            routeManager = RouteManager()
        }
        
        super.init()
        
        NotificationCenter.default.addObserver(
            self,
            selector: #selector(MapOverlayController.newMapUpdateReady(_:)),
            name: NSNotification.Name(rawValue: TrackCreator.notificationName),
            object: nil)
        
        //Register entering backgroudn to save track
        NotificationCenter.default.addObserver(
            self,
            selector: #selector(MapOverlayController.saveRoute),
            name: NSNotification.Name.UIApplicationDidEnterBackground,
            object: nil)
    }
    
    convenience init(mv: MKMapView, vc: UIViewController) {
        self.init()
        mapView = mv
        mapView.delegate = self
        
        parentViewController = vc
    }
    
    func saveRoute() {
        let isSuccessfulSave = NSKeyedArchiver.archiveRootObject(routeManager, toFile: MapOverlayController.routeURL.path)
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
                routePins!.append(point!)
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
                routePolylines!.append(line!)
            }
            
            mapView.addOverlays(routePolylines!)
        }
    }
    
    @objc func newMapUpdateReady(_ notification: Notification) {
        assert(notification.object == nil)
        
        if let tc = (UIApplication.shared.delegate as? AppDelegate)?.trackCreator {
            //Delete the old polyline, if it exists
            if(trackPolyLine != nil) {
                mapView.remove(trackPolyLine!)
            }
            
            trackPolyLine = tc.getPolyLine()
            if(trackPolyLine != nil) {
                mapView.add(trackPolyLine!)
            }
            
            //Center on thelatest coordinate if enabled
            if(centerOnCurrentPosition == true) {
                if let currentLocation = tc.getLatestCoordinates() {
                    if(!MKMapRectContainsPoint(mapView.visibleMapRect, MKMapPointForCoordinate(currentLocation))) {
                        mapView.setCenter(currentLocation, animated: true)
                    }
                }
            }
        } else {
            Logger.log("getting the track creator failed!")
        }
    }
    
    func mapView(_ mapView: MKMapView, rendererFor overlay: MKOverlay) -> MKOverlayRenderer {
        let plRenderer = MKPolylineRenderer(polyline: overlay as! MKPolyline)
        
        //Render the track in red and the route in black
        if(overlay.isEqual(trackPolyLine)) {
            plRenderer.strokeColor = UIColor.red
            plRenderer.lineWidth = 2.0
        } else {
            plRenderer.strokeColor = UIColor.black
            plRenderer.lineWidth = 2.0
        }
        
        return plRenderer
    }
    
    func mapView(_ mapView: MKMapView, viewFor annotation: MKAnnotation) -> MKAnnotationView? {
        // If the annotation is the user location, just return nil.
        if (annotation.isKind(of: MKUserLocation.self)) {
            return nil;
        }
        
        //Check if the annotation is part of the route (only kind for now, but maybe we get a plane icon later ;)
        if let _ = annotation as? RouteManager.WaypointWithAnnotations {
            //Define an identifier fir this kind of annotation
            let identifier = "route point"
            
            //Try to dequeue an existing view
            var view: MKPinAnnotationView
            if let dequeuedView = mapView.dequeueReusableAnnotationView(withIdentifier: identifier)
                as? MKPinAnnotationView {
                    dequeuedView.annotation = annotation
                    view = dequeuedView
            } else {
                view = MKPinAnnotationView(annotation: annotation, reuseIdentifier: identifier)
                
                view.animatesDrop = false
                view.canShowCallout = true
                view.isDraggable = true
                
                //Add a button on the rightt that goes to the "edit" screen
                let rightButton = UIButton(type: UIButtonType.detailDisclosure)
                //rightButton.addTarget(nil, action: nil, for: UIControlEvents.touchUpInside) //TODO: Really just remove the line?
                view.rightCalloutAccessoryView = rightButton
            }
            return view
        }
        return nil
    }
    
    func mapView(_ mapView: MKMapView, annotationView view: MKAnnotationView, calloutAccessoryControlTapped control: UIControl) {
        let storyboard = UIStoryboard(name: "Main", bundle: nil)
        let popoverController = storyboard.instantiateViewController(withIdentifier: "WaypointPopover") as! WaypointPopOverController
        popoverController.modalPresentationStyle = UIModalPresentationStyle.popover
        popoverController.popoverPresentationController?.sourceView = mapView
        popoverController.popoverPresentationController?.sourceRect = view.frame
        popoverController.preferredContentSize = CGSize(width: 270, height: 310)
        
        popoverController.waypoint = (view.annotation as! RouteManager.WaypointWithAnnotations)
        popoverController.controller = self
        popoverController.routeManager = routeManager
        
        parentViewController.present(popoverController, animated: true, completion: nil)
    }
    
    func popoverCompleted(_ wp: RouteManager.WaypointWithAnnotations?) {
        //Update the UI after the popover is done
        redrawRoutePolylines()
        
        if let waypoint = wp {
            mapView.removeAnnotation(waypoint as MKAnnotation)
            mapView.addAnnotation(waypoint as MKAnnotation)
        }
    }
    
    func mapView(_ mapView: MKMapView, annotationView view: MKAnnotationView, didChange newState: MKAnnotationViewDragState, fromOldState oldState: MKAnnotationViewDragState) {
        if(oldState == MKAnnotationViewDragState.dragging && newState == MKAnnotationViewDragState.ending) {
            //Redraw the polyline, the annotation coordinate is synced automatically
            redrawRoutePolylines()
        }
    }
}
