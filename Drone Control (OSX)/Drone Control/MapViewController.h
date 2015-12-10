//
//  MapViewController.h
//  Drone Control
//
//  Created by Ludger Heide on 13.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <MapKit/MapKit.h>


@interface MapViewController : NSViewController <MKMapViewDelegate>

-(void)updateMapWithLatitude: (NSNumber*) latitude longitude: (NSNumber*) longitude course: (NSNumber*) course speed: (NSNumber*) speed;

@end
