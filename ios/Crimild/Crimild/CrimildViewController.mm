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
    auto screenBuffer = crimild::alloc< crimild::FrameBufferObject >( 2 * framebufferRect.size.width, 2 * framebufferRect.size.height );
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
    crimild::InputState::getCurrentState().setMouseButtonState( 0, crimild::InputState::MouseButtonState::PRESSED );

    CGPoint location = [[touches anyObject] locationInView: self.view];
    crimild::InputState::getCurrentState().setMousePosition( crimild::Vector2i( location.x, location.y ) );
    crimild::InputState::getCurrentState().setNormalizedMousePosition( crimild::Vector2f( (float) location.x / self.view.bounds.size.width, (float) location.y / self.view.bounds.size.height ) );
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    crimild::InputState::getCurrentState().setMouseButtonState( 0, crimild::InputState::MouseButtonState::RELEASED );
    
    CGPoint location = [[touches anyObject] locationInView: self.view];
    crimild::InputState::getCurrentState().setMousePosition( crimild::Vector2i( location.x, location.y ) );
    crimild::InputState::getCurrentState().setNormalizedMousePosition( crimild::Vector2f( (float) location.x / self.view.bounds.size.width, (float) location.y / self.view.bounds.size.height ) );
}

@end
