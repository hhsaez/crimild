//
//  CrimildMetalView.m
//  Voyage
//
//  Created by Hernan Saez on 4/9/16.
//  Copyright © 2016 Hernan Saez. All rights reserved.
//

#import "CrimildMetalView.h"

#import <QuartzCore/CAMetalLayer.h>

#import "Rendering/CrimildMetalRenderer.h"

@implementation CrimildMetalView

+ (Class) layerClass
{
    return [CAMetalLayer class];
}

- (instancetype) initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        if (![self configure]) {
            return nil;
        }
    }
    
    return self;
}

- (instancetype) initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
        if (![self configure]) {
            return nil;
        }
    }
    
    return self;
}

- (BOOL) configure
{
    id< MTLDevice > device = MTLCreateSystemDefaultDevice();
    if ( device == nil ) {
        return NO;
    }

    crimild::Simulation::getInstance()->setRenderer( crimild::alloc< crimild::metal::MetalRenderer >( ( CrimildMetalView * ) self, device ) );
    
    return YES;
}

- (void) render
{
    [super render];
}

@end
