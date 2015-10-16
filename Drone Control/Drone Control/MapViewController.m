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

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
    MKMapView* myMapView = (MKMapView*) self.view;
    [myMapView setMapType: MKMapTypeHybrid];
}

@end
