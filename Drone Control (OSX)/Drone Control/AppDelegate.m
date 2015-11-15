//
//  AppDelegate.m
//  Drone Control
//
//  Created by Ludger Heide on 13.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#import "AppDelegate.h"
#import "ORSSerial/ORSSerialPort.h"

@interface AppDelegate ()

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    NSLog(@"Closing!");
    // Insert code here to tear down your application
    ORSSerialPort *myPort = [ORSSerialPort serialPortWithPath: @"/dev/tty.usbserial-DA017KGT"];
    [myPort close];
}

@end
