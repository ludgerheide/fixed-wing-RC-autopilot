//
//  PFDViewController.m
//  Drone Control
//
//  Created by Ludger Heide on 13.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#import "PFDViewController.h"
#import <CoreImage/CoreImage.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>

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
    IBOutlet NSTextField *label_heading;
    IBOutlet NSImageView *view_horizon;
    
    IBOutlet NSTextField* label_controller;
    NSNumber* pitch;
    NSNumber* yaw;
    NSNumber* thrust;
    
    NSImage* img_speedTape;
    NSImage* img_altituteTape;
    NSImage* img_invalid;
    
    CIContext* context;
    
    CIImage* ci_invalid;
    CIImage* ci_horizon;
    CIImage* ci_mask;
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
    
    //Create NSImages for the software rendered stuff
    img_speedTape = [NSImage imageNamed: @"Speed Tape"];
    img_altituteTape = [NSImage imageNamed: @"Altitude Tape"];
    img_invalid = [NSImage imageNamed: @"Invalid"];
    
    //Create the CI context
    context = [CIContext contextWithCGContext: [[NSGraphicsContext currentContext] graphicsPort]
                                      options: nil];
    
    //Create CIIimages for the hardware rendered stuff
    NSImage* img_mask = [NSImage imageNamed: @"Mask"];
    NSImage* img_horizon = [NSImage imageNamed: @"Horizon"];
    //Create CIImage for X, Horizon and overlay
    NSData* tiffData;
    NSBitmapImageRep* bitmap;
    //X
    tiffData = [img_invalid TIFFRepresentation];
    bitmap = [NSBitmapImageRep imageRepWithData:tiffData];
    ci_invalid = [[CIImage alloc] initWithBitmapImageRep:bitmap];
    
    //Horizon
    tiffData = [img_horizon TIFFRepresentation];
    bitmap = [NSBitmapImageRep imageRepWithData:tiffData];
    ci_horizon = [[CIImage alloc] initWithBitmapImageRep:bitmap];
    
    //Overlay
    tiffData = [img_mask TIFFRepresentation];
    bitmap = [NSBitmapImageRep imageRepWithData:tiffData];
    ci_mask = [[CIImage alloc] initWithBitmapImageRep:bitmap];
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

-(void) updateHorizon {
    const CGFloat horizon_height = 4400;
    const CGFloat horizon_width = 1000;
    
    NSImage* new_horizon;
    if(pitch && roll) {
        //Do the pitch transform
        CIFilter *cropFilter = [CIFilter filterWithName: @"CICrop"];
        CIVector* cropRect = [CIVector vectorWithCGRect: CGRectMake(0, ((horizon_height/(2.0*PITCH_AT_END)) * (pitch.doubleValue + PITCH_AT_END) - horizon_width/2), horizon_width, horizon_width)];
        [cropFilter setValue: ci_horizon forKey: @"inputImage"];
        [cropFilter setValue: cropRect forKey: @"inputRectangle"];
        
        //TODO: Do the roll transformation
        CIFilter *rollFilter = [CIFilter filterWithName: @"CIAffineTransform"];
        NSAffineTransform* transform = [NSAffineTransform transform];
        [transform translateXBy: +horizon_width/2
                            yBy: +horizon_width/2];
        [transform rotateByDegrees: roll.doubleValue];
        [transform translateXBy: -horizon_width/2
                            yBy: -(horizon_height/(2.0*PITCH_AT_END)) * (pitch.doubleValue + PITCH_AT_END)];
        [rollFilter setValue: cropFilter.outputImage forKey: @"inputImage"];
        [rollFilter setValue: transform forKey: @"inputTransform"];
        
        //Overlay the mask
        CIFilter* maskFilter = [CIFilter filterWithName: @"CISourceOverCompositing"];
        [maskFilter setValue: rollFilter.outputImage forKey: @"inputBackgroundImage"];
        [maskFilter setValue: ci_mask forKey: @"inputImage"];
        
        //Crop back to a square shape
        CIFilter *cropFilter2 = [CIFilter filterWithName: @"CICrop"];
        CIVector* cropRect2 = [CIVector vectorWithCGRect: CGRectMake(0, 0, horizon_width, horizon_width)];
        [cropFilter2 setValue: maskFilter.outputImage forKey: @"inputImage"];
        [cropFilter2 setValue: cropRect2 forKey: @"inputRectangle"];
        
        //Output the image
        NSCIImageRep *rep = [NSCIImageRep imageRepWithCIImage: cropFilter2.outputImage];
        new_horizon = [[NSImage alloc] initWithSize:rep.size];
        [new_horizon addRepresentation:rep];
    } else {
        CIFilter* maskFilter = [CIFilter filterWithName: @"CISourceOverCompositing"];
        [maskFilter setValue: ci_invalid forKey: @"inputBackgroundImage"];
        [maskFilter setValue: ci_mask forKey: @"inputImage"];
        
        //Output the image
        NSCIImageRep *rep = [NSCIImageRep imageRepWithCIImage: maskFilter.outputImage];
        new_horizon = [[NSImage alloc] initWithSize:rep.size];
        [new_horizon addRepresentation:rep];
    }
    
    view_horizon.image = new_horizon;
}

-(void) updateHeading {
    NSString* headingString;
    if(heading) {
        double roundedHeadng = round(heading.doubleValue);
        headingString = [NSString stringWithFormat: @"%03.0f", roundedHeadng];
        label_heading.textColor = [NSColor whiteColor];
    } else {
        headingString = @" X ";
        label_heading.textColor = [NSColor redColor];
    }
    label_heading.stringValue = headingString;
    CGFloat fontSize = view_speedTape.bounds.size.width / 3;
    NSFont* theFont = [NSFont fontWithDescriptor: [NSFontDescriptor fontDescriptorWithName: @"Monaco" size: fontSize] size: fontSize];
    label_heading.font = theFont;
}

//Method for ControllerDelegate
-(void) controllerChangedWithPitch: (NSNumber*) thePitch yaw: (NSNumber*) theYaw thrust: (NSNumber*) theThrust {
    pitch = thePitch;
    yaw = theYaw;
    thrust = theThrust;
    [self updateControllerText];
}

-(void) updateControllerText {
    NSString* controllerString;
    if(pitch && yaw && thrust) {
        controllerString = [NSString stringWithFormat: @"P: %+1.1f, Y: %+1.1f, T: %+1.1f", pitch.doubleValue, yaw.doubleValue, thrust.doubleValue];
        label_controller.textColor = [NSColor whiteColor];
    } else {
        controllerString = @"No Controller!";
        label_controller.textColor = [NSColor redColor];
    }
    label_controller.stringValue = controllerString;
    CGFloat fontSize = view_speedTape.bounds.size.width / 6;
    NSFont* theFont = [NSFont fontWithDescriptor: [NSFontDescriptor fontDescriptorWithName: @"Monaco" size: fontSize] size: fontSize];
    label_controller.font = theFont;
}

- (void) viewDidLayout {
    [self updateSpeedTape];
    [self updateAltitudeTape];
    [self updateHorizon];
    [self updateHeading];
    [self updateControllerText];
}
@end
