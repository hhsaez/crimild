/*
 * Copyright (c) 2013 - 2018, Hugo Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of CRIMILD nor the
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

#include "WindowSystem.hpp"

#include <Rendering/FrameBufferObject.hpp>
#include <Simulation/Simulation.hpp>
#include <Simulation/Systems/RenderSystem.hpp>

using namespace crimild;
using namespace crimild::sdl;

void WindowSystem::start( void ) noexcept
{
    SDL_InitSubSystem( SDL_INIT_VIDEO );

    if ( !createWindow() || !configureRenderer() ) {
        return;
    }

    broadcastMessage( messaging::WindowSystemDidStart { _window } );
}

crimild::Bool WindowSystem::createWindow( void )
{
    CRIMILD_LOG_TRACE( "Creating window" );

    auto settings = Simulation::getInstance()->getSettings();

    crimild::Int32 width = settings->get( "video.width", 1024 );
    crimild::Int32 height = settings->get( "video.height", 768 );
    crimild::Bool vsync = settings->get( "video.vsync", true );
    crimild::Bool highdpi = settings->get< crimild::Bool >( "video.highdpi", true );
    crimild::Bool fullscreen = settings->get< crimild::Bool >( "video.fullscreen", false );

    crimild::Int32 flags = SDL_WINDOW_OPENGL;

    if ( fullscreen )
        flags |= SDL_WINDOW_FULLSCREEN;
    if ( highdpi )
        flags |= SDL_WINDOW_ALLOW_HIGHDPI;

    if ( vsync )
        SDL_GL_SetSwapInterval( 1 );

#ifdef CRIMILD_PLATFORM_EMSCRIPTEN
    flags |= SDL_WINDOW_RESIZABLE;
#endif

    _window = SDL_CreateWindow(
        "Crimild SDL",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        flags );

    if ( _window == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot create window: ", SDL_GetError() );
        return false;
    }

    Log::info( CRIMILD_CURRENT_CLASS_NAME, "Created SDL window with dimensions ", width, "x", height );

    SDL_SetWindowTitle( _window, Simulation::getInstance()->getName().c_str() );

    SDL_GL_SwapWindow( _window );

    broadcastMessage( messaging::WindowSystemDidCreateWindow { _window } );

    return true;
}

crimild::Bool WindowSystem::configureRenderer( void )
{
#if !defined( CRIMILD_PLATFORM_EMSCRIPTEN )
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );

    crimild::Int32 glContextMajorVersion = 0;
    crimild::Int32 glContextMinorVersion = 0;

    SDL_GL_GetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, &glContextMajorVersion );
    SDL_GL_GetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, &glContextMinorVersion );
    Log::info( CRIMILD_CURRENT_CLASS_NAME, "Creating GL Context with version ", glContextMajorVersion, ".", glContextMinorVersion );
#endif

    _renderContext = SDL_GL_CreateContext( _window );
    if ( _renderContext == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot create render context: ", SDL_GetError() );
        return false;
    }

    auto *renderer = Simulation::getInstance()->getRenderer();
    if ( renderer == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "No valid renderer found" );
        return false;
    }

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    SDL_GL_GetDrawableSize( _window, &framebufferWidth, &framebufferHeight );
    renderer->getScreenBuffer()->resize( framebufferWidth, framebufferHeight );
    renderer->configure();
    if ( auto mainCamera = Simulation::getInstance()->getMainCamera() ) {
        auto screen = renderer->getScreenBuffer();
        auto aspect = ( float ) screen->getWidth() / ( float ) screen->getHeight();
        mainCamera->setAspectRatio( aspect );
    }

    Log::info( CRIMILD_CURRENT_CLASS_NAME, "Created screen buffer with dimensions ", framebufferWidth, "x", framebufferHeight );

    return true;
}

void WindowSystem::update( void ) noexcept
{
    CRIMILD_PROFILE( "Window System - Update" )

    _clock.tick();

    // Move to event handlers?
    switch ( Input::getInstance()->getMouseCursorMode() ) {
        case Input::MouseCursorMode::NORMAL:
            SDL_CaptureMouse( SDL_FALSE );
            SDL_ShowCursor( SDL_ENABLE );
            break;

        case Input::MouseCursorMode::HIDDEN:
            SDL_CaptureMouse( SDL_TRUE );
            SDL_ShowCursor( SDL_DISABLE );
            break;

        case Input::MouseCursorMode::GRAB:
            SDL_CaptureMouse( SDL_TRUE );
            SDL_ShowCursor( SDL_DISABLE );
            SDL_SetRelativeMouseMode( SDL_TRUE );
            break;
    }

    SDL_GL_SwapWindow( _window );

    if ( Simulation::getInstance()->getSettings()->get( "video.show_frame_time", false ) ) {
        std::string name = Simulation::getInstance()->getName();
        std::stringstream ss;
        ss << name << " (" << _clock.getDeltaTime() << "ms)";
        SDL_SetWindowTitle( _window, ss.str().c_str() );
    }
}

void WindowSystem::stop( void ) noexcept
{
    System::stop();

    cleanupRenderer();
    destroyWindow();
}

void WindowSystem::destroyWindow( void )
{
    CRIMILD_LOG_TRACE( "Destroying window" );
    if ( _window != nullptr ) {
        SDL_DestroyWindow( _window );
        _window = nullptr;
    }
}

void WindowSystem::cleanupRenderer( void )
{
    if ( _renderContext != nullptr ) {
        SDL_GL_DeleteContext( _renderContext );
        _renderContext = nullptr;
    }
}
