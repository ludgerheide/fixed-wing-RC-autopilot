//
//  SplitViewController.m
//  Drone Control
//
//  Created by Ludger Heide on 16.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#import "SplitViewController.h"
#import "PFDViewController.h"
#import "MapViewController.h"

@interface SplitViewController ()

@end

@implementation SplitViewController
{
    MapViewController* myMap;
    PFDViewController* myPFD;
    CommsModel* myComms;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
    NSSplitViewItem* first = self.splitViewItems.firstObject;
    first.minimumThickness = 160;
    if([first.viewController isKindOfClass: [PFDViewController class]]) {
        myPFD = (PFDViewController*)first.viewController;
    } else {
        NSLog(@"Error in view hierarchy!");
        [[NSApplication sharedApplication] terminate: nil];
    }
    
    NSSplitViewItem* second = self.splitViewItems.lastObject;
    second.minimumThickness = 160;
    if([second.viewController isKindOfClass: [MapViewController class]]) {
        myMap = (MapViewController*)second.viewController;
    } else {
        NSLog(@"Error in view hierarchy!");
        [[NSApplication sharedApplication] terminate: nil];
    }
    
    //Initialize the comms model
    myComms = [[CommsModel alloc] init];
    myComms.attitudeDelegate = self;
    myComms.positionDelegate = self;
    myComms.controllerDelegate = myPFD;
    myComms.batteryDelegate = myPFD;
}

//Display the SLP alert after the view has appeared
- (void)viewDidAppear {
    //Send sea level pressure to the plane
    NSAlert *alert = [[NSAlert alloc] init];
    [alert setMessageText:@"Please enter the current sea level pressure"];
    [alert addButtonWithTitle:@"Ok"];
    [alert addButtonWithTitle:@"Cancel"];
    
    NSTextField *input = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 200, 24)];
    [input setStringValue:@""];
    
    [alert setAccessoryView:input];
    NSInteger button = [alert runModal];
    if (button == NSAlertFirstButtonReturn) {
        NSString* slpString = [input stringValue];
        NSNumberFormatter *f = [[NSNumberFormatter alloc] init];
        f.numberStyle = NSNumberFormatterDecimalStyle;
        NSNumber *myNumber = [f numberFromString: slpString];
        [myComms sendSeaLevelPressure: myNumber];
    } else if (button == NSAlertSecondButtonReturn) {
        
    }
}

//Protocol methods
- (void) positionChangedToLatitude:(NSNumber *)latitude longitude:(NSNumber *)longitude altitude:(NSNumber *)altitude courseOverGround:(NSNumber *)courseOverGround speed:(NSNumber *)speed {
    [myMap updateMapWithLatitude: latitude longitude: longitude altitude: altitude course: courseOverGround speed: speed];
    myPFD.speed = speed;
    myPFD.altitude = altitude;
}

- (void) attituteChangedToCourse:(NSNumber *)course pitch:(NSNumber *)pitch roll:(NSNumber *)roll {
    myPFD.heading = course;
    myPFD.pitch = pitch;
    myPFD.roll = roll;
    [myPFD updateHorizon];
}

@end
