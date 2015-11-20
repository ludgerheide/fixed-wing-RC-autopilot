//
//  GStreamerBackend.h
//  Drone Control
//
//  Created by Ludger Heide on 18.11.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@protocol GStreamerBackendDelegate <NSObject>

@optional
/* Called when the GStreamer backend has finished initializing
 * and is ready to accept orders. */
-(void) gstreamerInitialized;

/* Called when the GStreamer backend wants to output some message
 * to the screen. */
-(void) gstreamerSetUIMessage:(NSString *)message;

@end

@interface GStreamerBackend : NSObject

/* Initialization method. Pass the delegate that will take care of the UI.
 * This delegate must implement the GStreamerBackendDelegate protocol.
 * Pass also the UIView object that will hold the video window. */
-(id) init:(id <GStreamerBackendDelegate>) uiDelegate videoView: (NSView*) videoView;

/* Set the pipeline to PLAYING */
-(void) play;

/* Set the pipeline to PAUSED */
-(void) pause;


@end
