//
//  MapViewController.m
//  Drone Control
//
//  Created by Ludger Heide on 13.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#import "MapViewController.h"

@interface MapViewController ()

@end

@implementation MapViewController
{
    MKMapView* myMapView;
    
    NSMutableArray* track;
    MKPolyline* polyLine;
    MKPointAnnotation *currentPosPin;
    
    CLLocationCoordinate2D* points;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
    myMapView = (MKMapView*) self.view;
    myMapView.mapType = MKMapTypeStandard;
    myMapView.scrollEnabled = YES;
    myMapView.pitchEnabled = NO;
    myMapView.delegate = self;
    
    //Initialize the treck array
    track = [[NSMutableArray alloc] init];
}

-(void)updateMapWithLatitude: (NSNumber*) latitude longitude: (NSNumber*) longitude course: (NSNumber*) course speed: (NSNumber*) speed {
    if(latitude == nil || longitude == nil) {
        //We do not have valid cdoordinates. remove the pin
        [myMapView removeAnnotation: currentPosPin];
        return;
    }
    CLLocationCoordinate2D coords;
    coords.latitude = latitude.doubleValue;
    coords.longitude = longitude.doubleValue;
    
    //Re-Center the view of necessary
    if(!MKMapRectContainsPoint(myMapView.visibleMapRect, MKMapPointForCoordinate(coords))) {
        [myMapView setCenterCoordinate: coords animated: YES];
    }
    
    //Add out point to the track
    CLLocation* loc = [[CLLocation alloc] initWithCoordinate: coords
                                                    altitude: 0
                                          horizontalAccuracy: 0
                                            verticalAccuracy: 0
                                                      course: course.doubleValue
                                                       speed: speed.doubleValue
                                                   timestamp:[NSDate date]];
    [track addObject: loc];
    
    
    //Add a pin at the current position
    [myMapView removeAnnotation: currentPosPin];
    currentPosPin = [[MKPointAnnotation alloc] init];
    currentPosPin.coordinate = coords;
    [myMapView addAnnotation: currentPosPin];
    
    //Delete the old points array and polyLine
    [myMapView removeOverlay: polyLine];
    free(points);
    
    //Create a C array and a polyline
    points = malloc(sizeof(CLLocationCoordinate2D) * track.count);
    if(points != NULL) {
        for(NSUInteger i = 0; i < track.count; i++) {
            CLLocation *trackPoint = [track objectAtIndex: i];
            points[i] = trackPoint.coordinate;
        }
        //Remove the old Polyine and add the new one
        polyLine = [MKPolyline polylineWithCoordinates: points count: track.count];
        [myMapView addOverlay: polyLine];
    }
}

- (MKOverlayRenderer *)mapView:(MKMapView *)mapView rendererForOverlay:(id <MKOverlay>)overlay {
    MKPolylineRenderer *polyLineRenderer = [[MKPolylineRenderer alloc] initWithPolyline:overlay];
    polyLineRenderer.strokeColor = [NSColor redColor];
    polyLineRenderer.lineWidth = 5.0;
    
    return polyLineRenderer;
    
}

//- (nullable MKAnnotationView *)mapView:(MKMapView *)mapView viewForAnnotation:(id <MKAnnotation>)annotation {
//    MKPinAnnotationView *pinView = [[MKPinAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:@"DETAILPIN_ID"];
//    [pinView setAnimatesDrop: NO];
//    [pinView setCanShowCallout:NO];
//    return pinView;
//}

@end
