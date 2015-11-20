//
//  UtilityView.h
//  Drone Control
//
//  Created by Ludger Heide on 18.11.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "GStreamerBackend.h"

@interface UtilityViewController : NSViewController <GStreamerBackendDelegate>

//Msthods for the GStreamerBackendDelegate Protocol
/* Called when the GStreamer backend has finished initializing
 * and is ready to accept orders. */
-(void) gstreamerInitialized;

/* Called when the GStreamer backend wants to output some message
 * to the screen. */
-(void) gstreamerSetUIMessage:(NSString *)message;

@end
