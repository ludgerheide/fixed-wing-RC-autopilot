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
    private var mapView: MKMapView!
    
    private var trackPolyLine: MKPolyline?
    
    internal var centerOnCurrentPosition: Bool! = false
    
    override init() {
        super.init()
        
        NSNotificationCenter.defaultCenter().addObserver(
            self,
            selector: "newMapUpdateReady:",
            name: TrackCreator.notificationName,
            object: nil)
    }
    
    convenience init(mv: MKMapView) {
        self.init()
        mapView = mv
        mapView.delegate = self
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
            print("getting the track creator failed!")
        }
    }
    
    @objc func mapView(mapView: MKMapView, rendererForOverlay overlay: MKOverlay) -> MKOverlayRenderer {
        assert(mapView == self.mapView)
        let plRenderer = MKPolylineRenderer(polyline: trackPolyLine!)
        plRenderer.strokeColor = UIColor.redColor()
        plRenderer.lineWidth = 2.0
        return plRenderer
    }
}