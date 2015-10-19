//
//  PFDViewController.m
//  Drone Control
//
//  Created by Ludger Heide on 13.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#import "PFDViewController.h"

#define MIN_SPEED 0
#define MAX_SPEED 100.0

#define MIN_ALTITUDE 0
#define MAX_ALTITUDE 500.0

#define PITCH_AT_END 110.0

@interface PFDViewController ()
{
    IBOutlet NSImageView* view_speedTape;
    IBOutlet NSTextField *label_Speed;
    IBOutlet NSImageView* view_altitudeTape;
    IBOutlet NSTextFieldCell *label_Altitude;
    IBOutlet NSImageView* view_horizon;
    IBOutlet NSTextField *label_heading;
    
    NSImage* img_speedTape;
    NSImage* img_altituteTape;
    NSImage* img_horizon;
    NSImage* img_invalid;
    NSImage* img_mask;
}

@end

@implementation PFDViewController

@synthesize speed;

-(void) setSpeed: (NSNumber*) theSpeed {
    speed = theSpeed;
    [self updateSpeedTape];
}

@synthesize altitude;

-(void) setAltitude: (NSNumber*) theAltitude {
    altitude = theAltitude;
    [self updateAltitudeTape];
}

@synthesize pitch;
@synthesize roll;

@synthesize heading;

-(void) setHeading: (NSNumber*) theHeading {
heading = theHeading;
[self updateHeading];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
    img_speedTape = [NSImage imageNamed: @"Speed Tape"];
    img_altituteTape = [NSImage imageNamed: @"Altitude Tape"];
    img_horizon = [NSImage imageNamed: @"Horizon"];
    img_invalid = [NSImage imageNamed: @"Invalid"];
    img_mask = [NSImage imageNamed: @"Mask"];
}

- (void) updateSpeedTape {
    NSImage* rendered_speedTape = [[NSImage alloc] initWithSize: view_speedTape.bounds.size];
    
    if(speed) {
        //Copy a portion (or the whole) from the source image so that the middle is at the correct speed
        NSRect destRect = NSMakeRect(0, 0, rendered_speedTape.size.width, rendered_speedTape.size.height);
        
        //Get the y-location of the target speed on the speed tape
        CGFloat scalingFactor = rendered_speedTape.size.width / img_speedTape.size.width;
        
        CGFloat requiredOffsetFromBottom = (img_speedTape.size.height/MAX_SPEED) * speed.doubleValue;
        
        //Now get the top and bottom location y-values for the source Image
        //The destination height divided by the scaling factor is the total source height we want
        CGFloat sourceHeight = rendered_speedTape.size.height / scalingFactor;
        
        //We need to go sourceheight/2 to the top and bottom
        CGFloat sourceBottom = requiredOffsetFromBottom - sourceHeight/2;
        
        NSRect sourceRect = NSMakeRect(0, sourceBottom, img_speedTape.size.width, sourceHeight);
        
        [rendered_speedTape lockFocus];
        [img_speedTape drawInRect: destRect
                         fromRect: sourceRect
                        operation: NSCompositeCopy
                         fraction: 1.0];
        [rendered_speedTape unlockFocus];
    } else {
        //Copy the X
        NSRect destRect = NSMakeRect(0, 0, rendered_speedTape.size.width, rendered_speedTape.size.height);
        NSRect sourceRect = NSMakeRect(0, 0, img_invalid.size.width, img_invalid.size.height);
        
        [rendered_speedTape lockFocus];
        [img_invalid drawInRect: destRect
                       fromRect: sourceRect
                      operation: NSCompositeCopy
                       fraction: 1.0];
        [rendered_speedTape unlockFocus];
    }
    
    view_speedTape.image = rendered_speedTape;
    
    NSString* speedString;
    if(speed) {
        double roundedSpeed = round(speed.doubleValue);
        speedString = [NSString stringWithFormat: @"%3.0f", roundedSpeed];
        label_Speed.textColor = [NSColor blackColor];
    } else {
        speedString = @" X ";
        label_Speed.textColor = [NSColor redColor];
    }
    label_Speed.stringValue = speedString;
    CGFloat fontSize = view_speedTape.bounds.size.width / 3;
    NSFont* theFont = [NSFont fontWithDescriptor: [NSFontDescriptor fontDescriptorWithName: @"Monaco" size: fontSize] size: fontSize];
    label_Speed.font = theFont;
}

- (void) updateAltitudeTape {
    NSImage* rendered_altituteTape = [[NSImage alloc] initWithSize: view_altitudeTape.bounds.size];
    
    if(altitude) {
        //Copy a portion (or the whole) from the source image so that the middle is at the correct speed
        NSRect destRect = NSMakeRect(0, 0, rendered_altituteTape.size.width, rendered_altituteTape.size.height);
        
        //Get the y-location of the target speed on the speed tape
        CGFloat scalingFactor = rendered_altituteTape.size.width / img_altituteTape.size.width;
        
        CGFloat requiredOffsetFromBottom = (img_altituteTape.size.height/MAX_ALTITUDE) * altitude.doubleValue;
        
        //Now get the top and bottom location y-values for the source Image
        //The destination height divided by the scaling factor is the total source height we want
        CGFloat sourceHeight = rendered_altituteTape.size.height / scalingFactor;
        
        //We need to go sourceheight/2 to the top and bottom
        CGFloat sourceBottom = requiredOffsetFromBottom - sourceHeight/2;
        
        NSRect sourceRect = NSMakeRect(0, sourceBottom, img_altituteTape.size.width, sourceHeight);
        
        [rendered_altituteTape lockFocus];
        [img_altituteTape drawInRect: destRect
                            fromRect: sourceRect
                           operation: NSCompositeCopy
                            fraction: 1.0];
        [rendered_altituteTape unlockFocus];
    } else {
        //Copy the X
        NSRect destRect = NSMakeRect(0, 0, rendered_altituteTape.size.width, rendered_altituteTape.size.height);
        NSRect sourceRect = NSMakeRect(0, 0, img_invalid.size.width, img_invalid.size.height);
        
        [rendered_altituteTape lockFocus];
        [img_invalid drawInRect: destRect
                       fromRect: sourceRect
                      operation: NSCompositeCopy
                       fraction: 1.0];
        [rendered_altituteTape unlockFocus];
    }
    
    view_altitudeTape.image = rendered_altituteTape;
    
    NSString* altitudeString;
    if(speed) {
        double roundedAltitude = round(altitude.doubleValue);
        altitudeString = [NSString stringWithFormat: @"%3.0f", roundedAltitude];
        label_Altitude.textColor = [NSColor blackColor];
    } else {
        altitudeString = @" X ";
        label_Altitude.textColor = [NSColor redColor];
    }
    label_Altitude.stringValue = altitudeString;
    CGFloat fontSize = view_speedTape.bounds.size.width / 3;
    NSFont* theFont = [NSFont fontWithDescriptor: [NSFontDescriptor fontDescriptorWithName: @"Monaco" size: fontSize] size: fontSize];
    label_Altitude.font = theFont;
}

- (void) updateHorizon {
    NSImage* rendered_horizon = [[NSImage alloc] initWithSize: view_horizon.bounds.size];
    
    if(pitch && roll) {
        NSImage* translated_horizon = [[NSImage alloc] initWithSize: view_horizon.bounds.size];
        CGRect sourceRect = CGRectMake(0, (img_horizon.size.height/(2*PITCH_AT_END)) * (pitch.doubleValue + PITCH_AT_END) - img_horizon.size.width/2, img_horizon.size.width, img_horizon.size.width);
        CGRect targetRect = CGRectMake(0, 0, translated_horizon.size.width, translated_horizon.size.height);
        
        [translated_horizon lockFocus];
        [img_horizon drawInRect: targetRect
                       fromRect: sourceRect
                      operation: NSCompositeCopy
                       fraction: 1.0];
        [translated_horizon unlockFocus];
        
        //Now rotate the rendered horizon
        NSAffineTransform* transform = [NSAffineTransform transform];
        [transform translateXBy: translated_horizon.size.width/2
                            yBy: translated_horizon.size.height/2];
        [transform rotateByDegrees: roll.doubleValue];
        [transform translateXBy: -translated_horizon.size.width
                            yBy: -translated_horizon.size.height];
        
        [rendered_horizon lockFocus];
        [transform concat];
        [translated_horizon drawAtPoint: NSMakePoint(rendered_horizon.size.width/2, rendered_horizon.size.height/2)
                               fromRect: NSZeroRect
                              operation: NSCompositeCopy
                               fraction: 1.0];
        [rendered_horizon unlockFocus];
        
    } else {
        //Copy the X
        NSRect destRect = NSMakeRect(0, 0, rendered_horizon.size.width, rendered_horizon.size.height);
        NSRect sourceRect = NSMakeRect(0, 0, img_invalid.size.width, img_invalid.size.height);
        
        [rendered_horizon lockFocus];
        [img_invalid drawInRect: destRect
                       fromRect: sourceRect
                      operation: NSCompositeCopy
                       fraction: 1.0];
        [rendered_horizon unlockFocus];
    }
    
    
    //Now add the mask
    [rendered_horizon lockFocus];
    [img_mask drawInRect: NSMakeRect(0, 0, rendered_horizon.size.width, rendered_horizon.size.height)
                fromRect:NSZeroRect
               operation: NSCompositeSourceOver
                fraction: 1.0];
    [rendered_horizon unlockFocus];
    
    view_horizon.image = rendered_horizon;
}

-(void) updateHeading {
    NSString* headingString;
    if(heading) {
        double roundedHeadng = round(heading.doubleValue);
        headingString = [NSString stringWithFormat: @"%03.0f", roundedHeadng];
        label_heading.textColor = [NSColor blackColor];
    } else {
        headingString = @" X ";
        label_heading.textColor = [NSColor redColor];
    }
    label_heading.stringValue = headingString;
    CGFloat fontSize = view_speedTape.bounds.size.width / 3;
    NSFont* theFont = [NSFont fontWithDescriptor: [NSFontDescriptor fontDescriptorWithName: @"Monaco" size: fontSize] size: fontSize];
    label_heading.font = theFont;
}

- (void) viewDidLayout {
    [self updateSpeedTape];
    [self updateAltitudeTape];
    [self updateHorizon];
    [self updateHeading];
}
@end
