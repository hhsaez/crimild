/*
 * Copyright (c) 2013, Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import "CrimildViewController.h"
#import "CrimildView.h"

#define CRIMILD_IOS_ENABLE_METAL 1

#import "Metal/CrimildMetalView.h"
#import "EAGL/CrimildEAGLView.h"

@interface CrimildViewController () {
    crimild::SharedPointer< crimild::Simulation > _simulation;
    
    CADisplayLink *_displayLink;
    NSInteger _animationFrameInterval;
}

@property (strong, nonatomic) CrimildView *crimildView;
@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;

@end

@implementation CrimildViewController

- (instancetype) init
{
    self = [super init];
    if (self) {
        _simulation = crimild::alloc< crimild::Simulation >( "crimild", nullptr );
        crimild::concurrency::JobScheduler::getInstance()->configure( 1 );
        _animating = FALSE;
        _animationFrameInterval = 1;
        _displayLink = nil;
        
        self.useMetalRenderPath = NO;
    }
    
    return self;
}

- (instancetype) initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
        _simulation = crimild::alloc< crimild::Simulation >( "crimild", nullptr );
        crimild::concurrency::JobScheduler::getInstance()->configure( 1 );
        _animating = FALSE;
        _animationFrameInterval = 1;
        _displayLink = nil;
        
//        self.useMetalRenderPath = NO;
    }
    
    return self;
}

- (void) loadView
{
    [super loadView];

#if !TARGET_OS_SIMULATOR
    if (self.useMetalRenderPath) {
        self.crimildView = [[CrimildMetalView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
        self.animationFrameInterval = 1;
    }
#endif
    
    if (self.crimildView == nil) {
        self.useMetalRenderPath = NO;
        self.crimildView = [[CrimildEAGLView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
        self.animationFrameInterval = 2;
    }
    
    self.view = self.crimildView;
    
    [self setupCrimild];
    
    self.touchEnabled = YES;
    self.swipeEnabled = NO;
}

- (void) dealloc
{
    _simulation = nullptr;
}

- (void) didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    
    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        _simulation = nullptr;
        self.view = nil;
        self.crimildView = nil;
    }
}

- (BOOL) prefersStatusBarHidden
{
    return YES;
}

- (crimild::Simulation *) simulation
{
    return crimild::get_ptr(_simulation);
}

- (void) viewWillAppear:(BOOL)animated
{
    [super viewWillAppear: animated];
    
    self.view.userInteractionEnabled = self.touchEnabled;
    
    if (self.swipeEnabled) {
        UISwipeGestureRecognizer *upRecognizer = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleSwipeGesture:)];
        upRecognizer.direction = UISwipeGestureRecognizerDirectionUp;
        [self.view addGestureRecognizer:upRecognizer];
        
        UISwipeGestureRecognizer *downRecognizer = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleSwipeGesture:)];
        downRecognizer.direction = UISwipeGestureRecognizerDirectionDown;
        [self.view addGestureRecognizer:downRecognizer];
        
        UISwipeGestureRecognizer *leftRecognizer = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleSwipeGesture:)];
        leftRecognizer.direction = UISwipeGestureRecognizerDirectionLeft;
        [self.view addGestureRecognizer:leftRecognizer];
        
        UISwipeGestureRecognizer *rightRecognizer = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleSwipeGesture:)];
        rightRecognizer.direction = UISwipeGestureRecognizerDirectionRight;
        [self.view addGestureRecognizer:rightRecognizer];
    }
}

- (void) viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    
    [self startAnimation];
}

- (void) viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    
    [self stopAnimation];
    self.simulation->stop();
}

#pragma mark - Crimild

- (NSString *) applicationBundleDirectory
{
    return [[NSBundle mainBundle] resourcePath];
}

- (NSString *) applicationDocumentsDirectory
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *basePath = paths.firstObject;
    return basePath;
}

- (void) setupCrimild
{
    crimild::FileSystem::getInstance().setBaseDirectory( [[self applicationBundleDirectory] UTF8String] );
    crimild::FileSystem::getInstance().setDocumentsDirectory( [[self applicationDocumentsDirectory] UTF8String] );
    
//#if TARGET_OS_TV
//    crimild::TaskManager::getInstance()->setNumThreads( 2 );
//#endif
    
    CGRect framebufferRect = [[UIScreen mainScreen] bounds];
    CGFloat screenScale = [[UIScreen mainScreen] scale];
    auto screenBuffer = crimild::alloc< crimild::FrameBufferObject >( screenScale * framebufferRect.size.width, screenScale * framebufferRect.size.height );
    screenBuffer->setClearColor( crimild::RGBAColorf( 0.0f, 0.0f, 0.0f, 0.0f ) );
    auto renderer = _simulation->getRenderer();
    renderer->setScreenBuffer( screenBuffer );
    renderer->configure();
    
    [self simulationWillStart: self.simulation];

    [self simulation]->start();
}

- (void) simulationWillStart: (crimild::Simulation *) simulation
{
    
}

#pragma mark - Animation

- (NSInteger) animationFrameInterval
{
    return _animationFrameInterval;
}

- (void) setAnimationFrameInterval:(NSInteger)frameInterval
{
    if (frameInterval >= 1) {
        _animationFrameInterval = frameInterval;
        
        if (_animating) {
            [self stopAnimation];
            [self startAnimation];
        }
    }
}

- (void) startAnimation
{
    if (!_animating) {
        _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(simulationStep:)];
        [_displayLink setFrameInterval:_animationFrameInterval];
        [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        
        _animating = TRUE;
    }
}

- (void) stopAnimation
{
    if (_animating) {
        [_displayLink invalidate];
        _displayLink = nil;
        _animating = FALSE;
    }
}

- (void) simulationStep: (id) sender
{
    if ( _simulation != nullptr ) {
        _simulation->update();
        
        if (self.crimildView != nil) {
            [self.crimildView render];
        }
    }
}

#pragma mark - Event handling

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
#if !TARGET_OS_TV
    CGPoint location = [[touches anyObject] locationInView: self.view];
    
    float x = location.x;
    float y = location.y;
    float nx = x / self.view.bounds.size.width;
    float ny = y / self.view.bounds.size.height;
    
    crimild::MessageQueue::getInstance()->pushMessage( crimild::messaging::MouseMotion { x, y, nx, ny } );
    crimild::MessageQueue::getInstance()->pushMessage( crimild::messaging::MouseButtonDown { CRIMILD_INPUT_MOUSE_BUTTON_LEFT } );
#endif
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
#if !TARGET_OS_TV
    CGPoint location = [[touches anyObject] locationInView: self.view];
    
    float x = location.x;
    float y = location.y;
    float nx = x / self.view.bounds.size.width;
    float ny = y / self.view.bounds.size.height;
    
    crimild::MessageQueue::getInstance()->pushMessage( crimild::messaging::MouseMotion { x, y, nx, ny } );
#endif
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
#if !TARGET_OS_TV
    CGPoint location = [[touches anyObject] locationInView: self.view];
    
    float x = location.x;
    float y = location.y;
    float nx = x / self.view.bounds.size.width;
    float ny = y / self.view.bounds.size.height;
    
    crimild::MessageQueue::getInstance()->pushMessage( crimild::messaging::MouseMotion { x, y, nx, ny } );
    crimild::MessageQueue::getInstance()->pushMessage( crimild::messaging::MouseButtonUp { CRIMILD_INPUT_MOUSE_BUTTON_LEFT } );
#endif
}

- (void) handleSwipeGesture: (UISwipeGestureRecognizer *) swipeGesture
{
    if (swipeGesture.state != UIGestureRecognizerStateEnded) {
        return;
    }
    
    switch (swipeGesture.direction) {
        case UISwipeGestureRecognizerDirectionDown:
            crimild::MessageQueue::getInstance()->pushMessage( crimild::messaging::SwipeDown { } );
            break;

        case UISwipeGestureRecognizerDirectionUp:
            crimild::MessageQueue::getInstance()->pushMessage( crimild::messaging::SwipeUp { } );
            break;
            
        case UISwipeGestureRecognizerDirectionLeft:
            crimild::MessageQueue::getInstance()->pushMessage( crimild::messaging::SwipeLeft { } );
            break;
            
        case UISwipeGestureRecognizerDirectionRight:
            crimild::MessageQueue::getInstance()->pushMessage( crimild::messaging::SwipeRight { } );
            break;
    }
}

@end
