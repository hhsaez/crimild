//
//  CrimildViewController.m
//  Crimild
//
//  Created by Hernan Saez on 8/13/15.
//  Copyright (c) 2015 Hernan Saez. All rights reserved.
//

#import "CrimildViewController.h"

#import <Crimild_GLES.hpp>

#import <OpenGLES/ES2/glext.h>

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
    return crimild::getRawPointer(_simulation);
}

#pragma mark - Crimild setup

- (void)setupCrimild
{
    _simulation = crimild::alloc< crimild::gles::GLESSimulation >();

    NSString *tileDirectory = [[NSBundle mainBundle] resourcePath];
    crimild::FileSystem::getInstance().setBaseDirectory( [tileDirectory UTF8String] );
    
    CGRect framebufferRect = [[UIScreen mainScreen] bounds];
    auto screenBuffer = crimild::alloc< crimild::FrameBufferObject >( framebufferRect.size.width, framebufferRect.size.height );
    screenBuffer->setClearColor( crimild::RGBAColorf( 0.0f, 0.0f, 0.0f, 0.0f ) );
    auto renderer = _simulation->getRenderer();
    renderer->setScreenBuffer( screenBuffer );
    renderer->configure();

    [self simulation]->start();
}

#pragma mark - GL Setup

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];

    glBindVertexArrayOES(0);
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{

}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    if ( _simulation != nullptr ) {
        _simulation->update();
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
