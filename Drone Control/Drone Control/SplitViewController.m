//
//  SplitViewController.m
//  Drone Control
//
//  Created by Ludger Heide on 16.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#import "SplitViewController.h"

@interface SplitViewController ()

@end

@implementation SplitViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
    NSSplitViewItem* first = self.splitViewItems.firstObject;
    first.minimumThickness = 160;
    
    NSSplitViewItem* second = self.splitViewItems.lastObject;
    second.minimumThickness = 160;
    
}

@end
