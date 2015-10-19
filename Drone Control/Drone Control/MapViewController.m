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
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
    myMapView = (MKMapView*) self.view;
    myMapView.mapType = MKMapTypeHybridFlyover;
    myMapView.scrollEnabled = NO;
    myMapView.pitchEnabled = NO;
}

-(void)updateMapWithLatitude: (NSNumber*) latitude longitude: (NSNumber*) longitude {
    CLLocationCoordinate2D coords;
    coords.latitude = latitude.doubleValue;
    coords.longitude = longitude.doubleValue;
    
    myMapView.centerCoordinate = coords;
}

@end
