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

#include "Foundation/GLFWUtils.hpp"
#include "Rendering/PlayerWindow.hpp"

#include <Crimild.hpp>
#include <Crimild_Vulkan.hpp>

static void glfw_error_callback( int error, const char *description )
{
    CRIMILD_LOG_FATAL( "GLFW Error: (", error, ") ", description );
}

int main( int argc, char **argv )
{
    crimild::init();
    crimild::vulkan::init();

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

    if ( !glfwInit() ) {
        CRIMILD_LOG_FATAL( "Cannot start GLFW: glfwInit failed" );
        exit( EXIT_FAILURE );
    }

    glfwSetErrorCallback( glfw_error_callback );

    int versionMajor;
    int versionMinor;
    int versionRevision;
    glfwGetVersion( &versionMajor, &versionMinor, &versionRevision );
    CRIMILD_LOG_INFO( "Initializing GLFW v", versionMajor, ".", versionMinor, " rev. ", versionRevision );

    // Create simulation before creating the window, but
    // delay its actual start until the window has been
    // created
    auto simulation = crimild::Simulation::create();

    auto window = crimild::alloc< crimild::player::Window >(
        [] {
            auto settings = crimild::Settings::getInstance();

            crimild::player::Window::Options ret;
            ret.extent.width = settings->get< crimild::Int32 >( "video.width", 1280 );
            ret.extent.height = settings->get< crimild::Int32 >( "video.height", 720 );
            ret.fullscreen = settings->get< crimild::Bool >( "video.fullscreen", false );
            if ( settings->hasKey( "video.resolution" ) ) {
                auto resolution = settings->get< std::string >( "video.resolution", "1080p" );
                if ( resolution == "480p" ) {
                    ret.extent.width = 640;
                    ret.extent.height = 480;
                } else if ( resolution == "sd" ) {
                    ret.extent.width = 720;
                    ret.extent.height = 576;
                } else if ( resolution == "hd" || resolution == "720p" ) {
                    ret.extent.width = 1280;
                    ret.extent.height = 720;
                } else if ( resolution == "fullHD" || resolution == "2k" || resolution == "1080p" ) {
                    ret.extent.width = 1920;
                    ret.extent.height = 1080;
                } else if ( resolution == "uhd" ) {
                    ret.extent.width = 3840;
                    ret.extent.height = 2160;
                } else if ( resolution == "4k" ) {
                    ret.extent.width = 4096;
                    ret.extent.height = 2160;
                } else if ( resolution == "8k" ) {
                    ret.extent.width = 7680;
                    ret.extent.height = 4230;
                }
            }

            // disable Retina by default
            ret.enableHDPI = settings->get< crimild::Bool >( "video.hdpi", false );

            // use discrete GPU by default, by disabling automatic graphics switching
            ret.enableGraphicsSwitching = settings->get< crimild::Bool >( "video.graphicsSwitching", false );

            ret.title = settings->get< std::string >( crimild::Settings::SETTINGS_APP_NAME, "Crimild" );

            return ret;
        }()
    );

    simulation->start();
    window->handle( crimild::Event { .type = crimild::Event::Type::SIMULATION_START } );

    while ( true ) {
        glfwPollEvents();

        // This also dispatch any sync_frame calls
        jobScheduler.executeDelayedJobs();

        // Dispatch deferred messages
        crimild::MessageQueue::getInstance()->dispatchDeferredMessages();

        const auto tick = crimild::Event { .type = crimild::Event::Type::TICK };
        auto ret = window->handle( tick );
        if ( ret.type == crimild::Event::Type::TERMINATE || ret.type == crimild::Event::Type::WINDOW_CLOSED ) {
            break;
        }
        if ( simulation->handle( tick ).type == crimild::Event::Type::TERMINATE ) {
            break;
        }
    }

    jobScheduler.stop();
    crimild::MessageQueue::getInstance()->clear();

    window = nullptr;

    glfwTerminate();
    CRIMILD_LOG_INFO( "GLFW terminated" );

    return 0;
}
