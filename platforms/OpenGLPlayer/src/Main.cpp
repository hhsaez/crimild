/*
 * Copyright (c) 2002 - present, H. Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Foundation/GLFWUtils.hpp"
#include "Rendering/FrameGraph/GLRenderScene.hpp"
#include "Rendering/GLContext.hpp"

#include <Crimild.hpp>
#include <iostream>

static void glfw_error_callback( int error, const char *description )
{
    CRIMILD_LOG_ERROR( "GLFW Error (", error, "): ", description );
}

int main( int argc, char **argv )
{
    crimild::init();

    crimild::Log::setOutputHandlers(
        {
            std::make_shared< crimild::ConsoleOutputHandler >( crimild::Log::LOG_LEVEL_DEBUG ),
        }
    );

    crimild::Settings settings;
    settings.parseCommandLine( argc, argv );

    // This should not be needed anymore.
    // TODO: Deprecate JobScheduler?
    crimild::concurrency::JobScheduler jobScheduler;
    jobScheduler.configure( 0 );
    jobScheduler.start();

    glfwSetErrorCallback( glfw_error_callback );
    if ( !glfwInit() ) {
        return -1;
    }

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
#ifdef CRIMILD_PLATFORM_OSX
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
#endif

    // Create simulation before creating the window, but
    // delay its actual start until the window has been
    // created
    auto simulation = crimild::Simulation::create();

    const int width = 800;
    const int height = 600;
    GLFWwindow *window = glfwCreateWindow( width, height, "Crimild", nullptr, nullptr );
    if ( window == nullptr ) {
        CRIMILD_LOG_FATAL( "Failed to create GLFW window" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent( window );

    if ( !gladLoadGLLoader( ( GLADloadproc ) glfwGetProcAddress ) ) {
        CRIMILD_LOG_FATAL( "Failed to initialize GLAD" );
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(
        window,
        []( auto window, auto width, auto height ) {
            // TODO
        }
    );

    crimild::gl::Context ctx;
    auto framegraph = std::make_shared< crimild::gl::framegraph::RenderScene >(
        crimild::Extent2D {
            .width = ( float ) width,
            .height = ( float ) height,
        }
    );

    // Start simulation to init all systems
    simulation->start();

    // Main loop
    while ( !glfwWindowShouldClose( window ) ) {
        // This also dispatch any sync_frame calls
        jobScheduler.executeDelayedJobs();

        // Dispatch deferred messages
        crimild::MessageQueue::getInstance()->dispatchDeferredMessages();

        const auto tick = crimild::Event { .type = crimild::Event::Type::TICK };
        if ( simulation->handle( tick ).type == crimild::Event::Type::TERMINATE ) {
            break;
        }

        framegraph->execute( ctx );

        glfwSwapBuffers( window );
        glfwPollEvents();
    }

    framegraph = nullptr;

    jobScheduler.executeDelayedJobs();
    crimild::MessageQueue::getInstance()->dispatchDeferredMessages();

    jobScheduler.stop();
    crimild::MessageQueue::getInstance()->clear();

    simulation = nullptr;

    glfwTerminate();

    return 0;
}
