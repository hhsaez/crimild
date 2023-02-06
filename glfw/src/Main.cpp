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

// #include "Simulation/Systems/GLFWSystem.hpp"
// #include "Simulation/Systems/GLFWVulkanSystem.hpp"
// #include "Simulation/Systems/GLFWWindowSystem.hpp"
// #include "Simulation/Systems/InputSystem.hpp"

#include "Foundation/GLFWUtils.hpp"
#include "Rendering/GLFWWindow.hpp"
#include "Rendering/STBImageManager.hpp"

#include <Crimild.hpp>
#include <Crimild_GLFW.hpp>
#include <Crimild_Vulkan.hpp>

#ifdef CRIMILD_ENABLE_OPENAL
    #include <Crimild_OpenAL.hpp>
#endif

#ifdef CRIMILD_ENABLE_IMGUI
    #include <Crimild_ImGUI.hpp>
#endif

static void errorCallback( int error, const char *description )
{
    CRIMILD_LOG_FATAL( "GLFW Error: (", error, ") ", description );
}

namespace crimild {

    namespace glfw {

        class GLFWApp {
        public:
            GLFWApp( void ) noexcept
            {
                if ( !glfwInit() ) {
                    CRIMILD_LOG_FATAL( "Cannot start GLFW: glfwInit failed" );
                    exit( EXIT_FAILURE );
                }

                glfwSetErrorCallback( errorCallback );

                int versionMajor;
                int versionMinor;
                int versionRevision;
                glfwGetVersion( &versionMajor, &versionMinor, &versionRevision );
                CRIMILD_LOG_INFO( "Initializing GLFW v", versionMajor, ".", versionMinor, " rev. ", versionRevision );
            }

            ~GLFWApp( void ) noexcept
            {
                glfwTerminate();
                CRIMILD_LOG_INFO( "GLFW terminated" );
            }

            int run( int argc, char **argv ) noexcept
            {
                Settings settings;
                settings.parseCommandLine( argc, argv );

                // This should not be needed anymore.
                // TODO: Deprecate JobScheduler?
                concurrency::JobScheduler jobScheduler;
                jobScheduler.configure( 0 );
                jobScheduler.start();

                // TODO: Move AssetManager here
                // TODO: Move AudioManager here
                crimild::stb::ImageManager imageManager;

                // Create simulation before creating the window, but
                // delay its actual start until the window has been
                // created
                auto simulation = Simulation::create();

                auto options = [] {
                    auto settings = Settings::getInstance();

                    Window::Options ret;
                    ret.extent.width = settings->get< crimild::Int32 >( "video.width", 1920 );
                    ret.extent.height = settings->get< crimild::Int32 >( "video.height", 1080 );
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

                    ret.title = settings->get< std::string >( Settings::SETTINGS_APP_NAME, "Crimild" );

                    return ret;
                }();

                // Create the window and all of its layers
                // TODO: Is this creating two Vulkan instances when multiple windows are available?
                std::vector< std::shared_ptr< Window > > windows;

                // Push simulation window
                windows.push_back( crimild::alloc< Window >( options ) );

                if ( settings.get< bool >( "editor.show", false ) ) {
                    // Push editor window
                    windows.push_back(
                        [ & ] {
                            options.showEditor = true;
                            return crimild::alloc< Window >( options );
                        }()
                    );
                }

                // Start simulation
                simulation->start();

                // Notify simulation started to windows
                for ( auto &w : windows ) {
                    w->handle( Event { .type = Event::Type::SIMULATION_START } );
                }

                while ( !windows.empty() ) {
                    glfwPollEvents();

                    // This also dispatch any sync_frame calls
                    jobScheduler.executeDelayedJobs();

                    // Dispatch deferred messages
                    MessageQueue::getInstance()->dispatchDeferredMessages();

                    const auto tick = Event {
                        .type = Event::Type::TICK,
                    };

                    auto it = windows.begin();
                    while ( it != windows.end() ) {
                        const auto ret = ( *it )->handle( tick );
                        if ( ret.type == Event::Type::TERMINATE ) {
                            // Remove all windows
                            windows.clear();
                            break;
                        } else if ( ret.type == Event::Type::WINDOW_CLOSED ) {
                            it = windows.erase( it );
                        } else {
                            ++it;
                        }
                    }

                    if ( !windows.empty() && simulation->handle( tick ).type == Event::Type::TERMINATE ) {
                        break;
                    }
                }

                windows.clear();

                jobScheduler.stop();
                MessageQueue::getInstance()->clear();

                return 0;
            }
        };

    }

}

using namespace crimild;

int main( int argc, char **argv )
{
    crimild::init();
    crimild::glfw::init();
    crimild::vulkan::init();

    Log::setLevel( Log::Level::LOG_LEVEL_ALL );

    crimild::glfw::GLFWApp app;
    return app.run( argc, argv );
}
