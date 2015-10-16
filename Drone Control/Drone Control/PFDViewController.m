//
//  PFDViewController.m
//  Drone Control
//
//  Created by Ludger Heide on 13.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#import "PFDViewController.h"

@interface PFDViewController ()

@end

@implementation PFDViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
    SKView* spriteView = (SKView*) self.view;
    spriteView = (SKView*) self.view;
    spriteView.showsDrawCount = YES;
    spriteView.showsNodeCount = YES;
    spriteView.showsFPS = YES;
}

@end
