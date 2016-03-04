//
//  CrimildViewController.m
//  Crimild
//
//  Created by Hernan Saez on 8/13/15.
//  Copyright (c) 2015 Hernan Saez. All rights reserved.
//

#import "CrimildViewController.h"

#import <Crimild_OpenGL.hpp>

@interface CrimildViewController () {
    crimild::SharedPointer< crimild::Simulation > _simulation;
}

@property (strong, nonatomic) EAGLContext *context;

- (void)setupGL;
- (void)tearDownGL;

@end

@implementation CrimildViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    [self setupGL];
    [self setupCrimild];
    
    self.touchEnabled = YES;
    self.swipeEnabled = NO;
}

- (void)dealloc
{
    _simulation = nullptr;
    
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    
    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        _simulation = nullptr;
        
        [self tearDownGL];
        
        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }
}

- (BOOL)prefersStatusBarHidden {
    return YES;
}

- (crimild::Simulation *)simulation {
    return crimild::get_ptr(_simulation);
}

- (void) viewWillAppear:(BOOL)animated {
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

#pragma mark - Crimild setup

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
    _simulation = crimild::alloc< crimild::Simulation >( "crimild", nullptr );
    _simulation->setRenderer( crimild::alloc< crimild::opengl::OpenGLRenderer >() );

    crimild::FileSystem::getInstance().setBaseDirectory( [[self applicationBundleDirectory] UTF8String] );
    crimild::FileSystem::getInstance().setDocumentsDirectory( [[self applicationDocumentsDirectory] UTF8String] );
    
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

#pragma mark - GL Setup

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];

    glBindVertexArray(0);
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
    if ( _simulation != nullptr ) {
        _simulation->update();
    }
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    if ( _simulation != nullptr ) {
        _simulation->broadcastMessage( crimild::messaging::RenderNextFrame {} );
        
        [((GLKView *) self.view) bindDrawable];
        
        _simulation->broadcastMessage( crimild::messaging::PresentNextFrame {} );
    }
}

#pragma mark - Event handling

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    CGPoint location = [[touches anyObject] locationInView: self.view];
    
    float x = location.x;
    float y = location.y;
    float nx = x / self.view.bounds.size.width;
    float ny = y / self.view.bounds.size.height;
    
    crimild::MessageQueue::getInstance()->pushMessage( crimild::messaging::MouseMotion { x, y, nx, ny } );
    crimild::MessageQueue::getInstance()->pushMessage( crimild::messaging::MouseButtonDown { CRIMILD_INPUT_MOUSE_BUTTON_LEFT } );
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    CGPoint location = [[touches anyObject] locationInView: self.view];
    
    float x = location.x;
    float y = location.y;
    float nx = x / self.view.bounds.size.width;
    float ny = y / self.view.bounds.size.height;
    
    crimild::MessageQueue::getInstance()->pushMessage( crimild::messaging::MouseMotion { x, y, nx, ny } );
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    CGPoint location = [[touches anyObject] locationInView: self.view];
    
    float x = location.x;
    float y = location.y;
    float nx = x / self.view.bounds.size.width;
    float ny = y / self.view.bounds.size.height;
    
    crimild::MessageQueue::getInstance()->pushMessage( crimild::messaging::MouseMotion { x, y, nx, ny } );
    crimild::MessageQueue::getInstance()->pushMessage( crimild::messaging::MouseButtonUp { CRIMILD_INPUT_MOUSE_BUTTON_LEFT } );
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
