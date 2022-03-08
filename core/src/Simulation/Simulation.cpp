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

#include "Simulation.hpp"

#include "Concurrency/Async.hpp"
#include "FileSystem.hpp"
#include "Foundation/Log.hpp"
#include "Foundation/Version.hpp"
#include "Messaging/MessageQueue.hpp"
#include "SceneGraph/Camera.hpp"
#include "Simulation/Console/ConsoleCommand.hpp"
#include "Simulation/Event.hpp"
#include "Simulation/Systems/ConsoleSystem.hpp"
#include "Simulation/Systems/DebugSystem.hpp"
#include "Simulation/Systems/RenderSystem.hpp"
#include "Simulation/Systems/StreamingSystem.hpp"
#include "Simulation/Systems/UISystem.hpp"
#include "Simulation/Systems/UpdateSystem.hpp"
#include "Visitors/FetchCameras.hpp"
#include "Visitors/Picking.hpp"
#include "Visitors/StartComponents.hpp"
#include "Visitors/UpdateComponents.hpp"
#include "Visitors/UpdateRenderState.hpp"
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild;

#ifdef CRIMILD_PLATFORM_EMSCRIPTEN

    #include <emscripten.h>
    #include <emscripten/html5.h>

void simulation_step( void )
{
    crimild::Simulation::getInstance()->update();
}

#endif

#ifndef CRIMILD_SIMULATION_FORCE_SLEEP_ON_UPDATE
    #define CRIMILD_SIMULATION_FORCE_SLEEP_ON_UPDATE 1
#endif

void Simulation::start( void ) noexcept
{
    CRIMILD_LOG_INFO( "Initializing simulation ", getName() );

    Version version;
    Log::info( CRIMILD_CURRENT_CLASS_NAME, version.getDescription() );

    // enable some threads if not already specified
    if ( getSettings() != nullptr ) {
        auto workerCount = getSettings()->get< crimild::Int32 >( "simulation.threading.workers", 0 );
        _jobScheduler.configure( workerCount );
    } else {
        // Disable worker threads
        _jobScheduler.configure( 0 );
    }

#ifdef CRIMILD_PLATFORM_EMSCRIPTEN
    if ( getSettings() == nullptr ) {
        _settings = crimild::alloc< Settings >();
    }

    // override window size based on canvas
    int width, height;
    if ( emscripten_get_canvas_element_size( "crimild_canvas", &width, &height ) != EMSCRIPTEN_RESULT_SUCCESS ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot obtain canvas size" );
    }

    Log::info( CRIMILD_CURRENT_CLASS_NAME, "Canvas size = (", width, "x", height, ")" );

    getSettings()->set( "video.width", width );
    getSettings()->set( "video.height", height );
    getSettings()->set( "video.fullscreen", false );
#endif

    //    addSystem( crimild::alloc< ConsoleSystem >() );
    //addSystem( crimild::alloc< UpdateSystem >() );
    /*
    addSystem( crimild::alloc< RenderSystem >() );
    addSystem( crimild::alloc< DebugSystem >() );
    addSystem( crimild::alloc< StreamingSystem >() );
    addSystem( crimild::alloc< UISystem >() );
	*/

    //    Console::getInstance()->registerCommand( crimild::alloc< SimpleConsoleCommand >( "quit", []( Console *console, ConsoleCommand::ConsoleCommandArgs const & ) {
    //        // we need to disable the console so no further commands are triggered
    //        console->setEnabled( false );
    //
    //        crimild::concurrency::sync_frame( [] {
    //            Simulation::getInstance()->stop();
    //        } );
    //    } ) );

#ifdef CRIMILD_PLATFORM_EMSCRIPTEN
    // disable threads in web
    _jobScheduler.configure( 0 );

    emscripten_set_main_loop( simulation_step, 0, false );
#endif

    const auto ret = handle( Event { .type = Event::Type::SIMULATION_START } );
    if ( ret.type == Event::Type::TERMINATE ) {
        return;
    }

    // Call `onAwake` before starting any system
    // onAwake();

    // Invoke onInit before starting systems
    // m_systems.each( []( auto system ) { system->onInit(); } );

    // Start all systems
    // m_systems.each( []( auto system ) { system->start(); } );

    // Call `onStarted` here to load scenes if needed
    // onStarted();

    // Finalize startup
    // m_systems.each( []( auto system ) { system->lateStart(); } );

    _jobScheduler.start();
}

bool Simulation::step( void ) noexcept
{
#if CRIMILD_SIMULATION_FORCE_SLEEP_ON_UPDATE
    constexpr auto MIN_FRAME_TIME = std::chrono::milliseconds( 16 );
    using clock = std::chrono::high_resolution_clock;
    auto frameStartTime = clock::now();
#endif

    auto scene = getScene();

    if ( scene != nullptr && Camera::getMainCamera() == nullptr ) {
        // fetch all cameras from the scene
        FetchCameras fetchCameras;
        _scene->perform( fetchCameras );
        fetchCameras.forEachCamera( [ & ]( Camera *camera ) {
            if ( Camera::getMainCamera() == nullptr || camera->isMainCamera() ) {
                Camera::setMainCamera( camera );
            }
        } );
    }

    // broadcastMessage( messaging::SimulationWillUpdate { scene } );

    _simulationClock.tick();

    _jobScheduler.executeDelayedJobs();

    MessageQueue::getInstance()->dispatchDeferredMessages();

    if ( handle( Event { .type = Event::Type::SIMULATION_UPDATE } ).type == Event::Type::TERMINATE ) {
        return false;
    }

    if ( handle( Event { .type = Event::Type::SIMULATION_RENDER } ).type == Event::Type::TERMINATE ) {
        return false;
    }

    // update
    // m_systems.each( []( auto system ) { system->earlyUpdate(); } );
    // m_systems.each( []( auto system ) { system->fixedUpdate(); } ); // TODO: this has to be called at fixed intervals
    // m_systems.each( []( auto system ) { system->update(); } );
    // m_systems.each( []( auto system ) { system->lateUpdate(); } );

    // render
    // m_systems.each( []( auto system ) { system->onPreRender(); } );
    // m_systems.each( []( auto system ) { system->onRender(); } );
    // m_systems.each( []( auto system ) { system->onPostRender(); } );

    // broadcastMessage( messaging::SimulationDidUpdate { scene } );

    if ( scene != nullptr ) {
        scene->perform( UpdateComponents( _simulationClock ) );
        scene->perform( UpdateWorldState() );
    }

    if ( !_jobScheduler.isRunning() ) {
        return false;
    }

#if CRIMILD_SIMULATION_FORCE_SLEEP_ON_UPDATE
    auto frameEndTime = clock::now();
    auto delta = frameEndTime - frameStartTime;
    auto t = std::max(
        Int64( 1 ),
        Int64( ( MIN_FRAME_TIME - std::chrono::duration_cast< std::chrono::nanoseconds >( delta ) ).count() ) );
    std::this_thread::sleep_for( std::chrono::nanoseconds( t ) );
#endif

    return true;
}

void Simulation::stop( void ) noexcept
{
    // stop all unfinished tasks first
    _jobScheduler.stop();

    // clear all messages
    MessageQueue::getInstance()->clear();

    setScene( nullptr );

    _assetManager.clear( true );

#ifdef CRIMILD_PLATFORM_EMSCRIPTEN
    emscripten_cancel_main_loop();
#endif

    handle( Event { .type = Event::Type::SIMULATION_STOP } );
    // m_systems.each( []( auto system ) { system->onBeforeStop(); } );

    // m_systems.each( []( auto system ) { system->stop(); } );
}

Event Simulation::handle( const Event &e ) noexcept
{
    _input.handle( e );

    if ( e.type == Event::Type::MOUSE_CLICK ) {
        if ( auto scene = getScene() ) {
            if ( auto camera = Camera::getMainCamera() ) {
                auto x = e.button.npos.x;
                auto y = e.button.npos.y;
                Ray3 R;
                if ( camera->getPickRay( x, y, R ) ) {
                    Picking::Results res;
                    scene->perform( Picking( R, res, []( auto node ) { return node->getClassName() == Geometry::__CLASS_NAME; } ) );
                    if ( res.hasResults() ) {
                        auto node = res.getBestCandidate();
                        return Event {
                            .type = Event::Type::NODE_SELECTED,
                            .node = node,
                        };
                    }
                }
            }
        }
    }

    return e;
}

// int Simulation::run( void ) noexcept
// {
//     start();

// #if !defined( CRIMILD_PLATFORM_EMSCRIPTEN )
//     bool done = false;

//     while ( !done ) {
//         done = !step();
//     }
// #endif

//     stop();

//     // m_systems.each( []( auto system ) { system->onTerminate(); } );

//     CRIMILD_LOG_INFO( "Simulation terminated" );

//     return 0;
// }

// void Simulation::attachSystem( SharedPointer< System > const &system ) noexcept
// {
//     Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Adding system ", system->getClassName() );

//     m_systems.add( system );
//     system->onAttach();
// }

// void Simulation::detachAllSystems( void ) noexcept
// {
//     m_systems.each( []( auto system ) { system->onDetach(); } );
//     m_systems.clear();
// }

void Simulation::setScene( SharedPointer< Node > const &scene )
{
    _scene = scene;
    /*
	_scene = scene;
	_cameras.clear();

	if ( _scene != nullptr ) {
		_scene->perform( UpdateWorldState() );
		_scene->perform( UpdateRenderState() );

        // fetch all cameras from the scene
		FetchCameras fetchCameras;
		_scene->perform( fetchCameras );
        fetchCameras.forEachCamera( [&]( Camera *camera ) {
			if ( Camera::getMainCamera() == nullptr || camera->isMainCamera() ) {
				Camera::setMainCamera( camera );
			}

            _cameras.push_back( camera );
        });

        // compute actual aspect ratio for main camera
        auto renderer = Simulation::getInstance()->getRenderer();
        if ( getMainCamera() != nullptr && renderer != nullptr && renderer->getScreenBuffer() != nullptr ) {
            auto screen = renderer->getScreenBuffer();
            auto aspect = ( float ) screen->getWidth() / ( float ) screen->getHeight();

            if ( getMainCamera() != nullptr ) {
                getMainCamera()->setAspectRatio( aspect );
            }
        }

        // start all components
        _scene->perform( StartComponents() );

		// update state one more time after starting components
		_scene->perform( UpdateWorldState() );
		_scene->perform( UpdateRenderState() );
    }
	else {
		// invalid scene. reset camera
		Camera::setMainCamera( nullptr );
	}

    MessageQueue::getInstance()->clear();

    _simulationClock.reset();

    broadcastMessage( messaging::SceneChanged { crimild::get_ptr( _scene ) } );
    */
}

void Simulation::loadScene( std::string filename )
{
    // broadcastMessage( messaging::LoadScene { filename } );
}

void Simulation::forEachCamera( std::function< void( Camera * ) > callback )
{
    for ( auto camera : _cameras ) {
        callback( camera );
    }
}
