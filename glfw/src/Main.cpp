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

#include <Crimild.hpp>
#include <Crimild_STB.hpp>
#include <Crimild_Vulkan.hpp>

#ifdef CRIMILD_ENABLE_OPENAL
    #include <Crimild_OpenAL.hpp>
#endif

#ifdef CRIMILD_ENABLE_IMGUI
    #include <Crimild_ImGUI.hpp>
#endif

/**
 * GLFWApp (glfw)
 *  VulkamSystem (instance, debug callback, physical device)
 *  AudioSystem (openal)
 *  ImageSystem (stb-image)
 *  ImGuiSystem
 *  WindowSystem (glfw)
 *      Window (glfw, vulkan surface)
 *          VulkanPresentantionLayer (render device, swapchain)
 *              Editor (imgui)
 *                  Simulation (core)
 *                      Scene
 *
 */

void errorCallback( int error, const char *description )
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

                crimild::stb::ImageManager imageManager;

                Window window;

                while ( true ) {
                    glfwPollEvents();

                    const auto ret = window.handle( Event { Event::Type::TICK } );
                    if ( ret.type == Event::Type::TERMINATE ) {
                        break;
                    }
                }

                return 0;
            }
        };

    }

}

using namespace crimild;
// using namespace crimild::glfw;

int main( int argc, char **argv )
{
    crimild::init();
    crimild::vulkan::init();

    Log::setLevel( Log::Level::LOG_LEVEL_ALL );

    crimild::glfw::GLFWApp app;
    return app.run( argc, argv );

    //     CRIMILD_SIMULATION_LIFETIME auto sim = Simulation::create();

    //     sim->setSettings( crimild::alloc< Settings >( argc, argv ) );

    //     SharedPointer< ImageManager > imageManager = crimild::alloc< crimild::stb::ImageManager >();

    //     sim->attachSystem(
    //         [] {
    //             auto sys = std::make_unique< GLFWSystem >();
    //             sys->attachSystem(
    //                 [] {
    //                     auto sys = std::make_unique< WindowSystem >();
    //                     return sys;
    //                 }() );
    //             return sys;
    //         }() );

    //     // sim->attachSystem< GLFWSystem >();
    //     // sim->attachSystem< WindowSystem >();
    //     // sim->attachSystem< GLFWVulkanSystem >();
    //     // sim->attachSystem< vulkan::CaptureSystem >();
    //     // sim->attachSystem< EventSystem >();
    //     // sim->attachSystem< InputSystem >();
    //     // sim->attachSystem< UpdateSystem >();
    //     // sim->attachSystem< RenderSystem >();

    // #ifdef CRIMILD_ENABLE_OPENAL
    //     // sim->attachSystem< audio::OpenALAudioSystem >();
    // #endif

    // #ifdef CRIMILD_ENABLE_IMGUI
    //     // sim->attachSystem< imgui::ImGUISystem >();
    // #endif

    //     return sim->run();
}
