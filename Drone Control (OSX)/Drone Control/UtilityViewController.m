//
//  UtilityView.m
//  Drone Control
//
//  Created by Ludger Heide on 18.11.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#import "UtilityViewController.h"

@implementation UtilityViewController
{
    IBOutlet NSButton *buUpdate;
    IBOutlet NSButton *buPlayPause;
    IBOutlet NSTextField *laVideoStatus;
    IBOutlet NSView *nvVideoView;
    IBOutlet NSView *nvContainerView;
    
    IBOutlet NSLayoutConstraint *video_width_constraint;
    IBOutlet NSLayoutConstraint *video_height_constraint;
    
    int media_width;
    int media_height;
    GStreamerBackend* gst_backend;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
    
    //Disable the menu for this window so we don't get another one
    NSMenu* mainMenu = [[NSApplication sharedApplication] menu];
    if(mainMenu != nil) {
        NSMenu* viewMenu = [[mainMenu.itemArray objectAtIndex: 2] submenu];
        if(viewMenu != nil && [[viewMenu.itemArray objectAtIndex: 0] tag] == 42) {
            NSMenuItem* utilityView = [viewMenu.itemArray objectAtIndex: 0];
            [utilityView setState: NSOnState];
            [utilityView setEnabled: NO];
        }
    }
    
    //TODO: Chek if vieo is available
    gst_backend = [[GStreamerBackend alloc] init:self videoView: nvVideoView];
    
    /* Make these constant for now, later we will change them */
    media_width = 640;
    media_height = 480;
}

- (IBAction)buUpdatePressed:(id)sender {
    
}

- (IBAction)buPlayPausePressed:(id)sender {
    [gst_backend play];
    buPlayPause.enabled = NO;
}

- (void)viewDidLayout
{
    CGFloat view_width = nvContainerView.bounds.size.width;
    CGFloat view_height = nvContainerView.bounds.size.height;
    
    CGFloat correct_height = view_width * media_height / media_width;
    CGFloat correct_width = view_height * media_width / media_height;
    
    if (correct_height < view_height) {
        video_height_constraint.constant = correct_height;
        video_width_constraint.constant = view_width;
    } else {
        video_width_constraint.constant = correct_width;
        video_height_constraint.constant = view_height;
    }
}


/* Called when the GStreamer backend has finished initializing
 * and is ready to accept orders. */
-(void) gstreamerInitialized {
    dispatch_async(dispatch_get_main_queue(), ^{
        buPlayPause.enabled = TRUE;
        laVideoStatus.stringValue = @"Ready";
    });
    
}

/* Called when the GStreamer backend wants to output some message
 * to the screen. */
-(void) gstreamerSetUIMessage:(NSString *)message {
    dispatch_async(dispatch_get_main_queue(), ^{
        laVideoStatus.stringValue = message;
    });
}

-(void) viewWillDisappear {
    //TODO: Re-enable the menu
    NSMenu* mainMenu = [[NSApplication sharedApplication] menu];
    if(mainMenu != nil) {
        NSMenu* viewMenu = [[mainMenu.itemArray objectAtIndex: 2] submenu];
        if(viewMenu != nil && [[viewMenu.itemArray objectAtIndex: 0] tag] == 42) {
            NSMenuItem* utilityView = [viewMenu.itemArray objectAtIndex: 0];
            [utilityView setState: NSOffState];
            [utilityView setEnabled: YES];
        }
    }
}

@end
