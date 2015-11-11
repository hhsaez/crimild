//
//  CrimildViewController.h
//  Crimild
//
//  Created by Hernan Saez on 8/13/15.
//  Copyright (c) 2015 Hernan Saez. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#import <Crimild.hpp>

@interface CrimildViewController : GLKViewController

@property (nonatomic, readonly) crimild::Simulation *simulation;

@end
