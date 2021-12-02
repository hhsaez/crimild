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

#include "Simulation.hpp"
#include "FileSystem.hpp"
 
#include "Foundation/Log.hpp"
#include "Foundation/Version.hpp"

#include "Concurrency/Async.hpp"

#include "SceneGraph/Camera.hpp"

#include "Rendering/FrameBufferObject.hpp"

#include "Visitors/FetchCameras.hpp"
#include "Visitors/UpdateWorldState.hpp"
#include "Visitors/UpdateRenderState.hpp"
#include "Visitors/StartComponents.hpp"

#include "Simulation/Systems/RenderSystem.hpp"
#include "Simulation/Systems/UpdateSystem.hpp"
#include "Simulation/Systems/DebugSystem.hpp"
#include "Simulation/Systems/StreamingSystem.hpp"
#include "Simulation/Systems/UISystem.hpp"
#include "Simulation/Systems/ConsoleSystem.hpp"

#include "Simulation/Console/ConsoleCommand.hpp"

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

Simulation::Simulation( std::string name, SettingsPtr const &settings )
	: NamedObject( name ),
      _settings( settings )
{
    Version version;
    Log::info( CRIMILD_CURRENT_CLASS_NAME, version.getDescription() );

    // worker threads are disabled by default
    _jobScheduler.configure( 0 );

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
    
    addSystem( crimild::alloc< ConsoleSystem >() );
	addSystem( crimild::alloc< UpdateSystem >() );
	addSystem( crimild::alloc< RenderSystem >() );
    addSystem( crimild::alloc< DebugSystem >() );
    addSystem( crimild::alloc< StreamingSystem >() );
    addSystem( crimild::alloc< UISystem >() );

    Console::getInstance()->registerCommand( crimild::alloc< SimpleConsoleCommand >( "quit", []( Console *console, ConsoleCommand::ConsoleCommandArgs const & ) {
        // we need to disable the console so no further commands are triggered
        console->setEnabled( false );

        crimild::concurrency::sync_frame( [] {
            Simulation::getInstance()->stop();
        });
    }));
}

Simulation::~Simulation( void )
{
    stop();
    stopSystems();
}

void Simulation::start( void )
{
#ifdef CRIMILD_PLATFORM_EMSCRIPTEN
	// disable threads in web
    _jobScheduler.configure( 0 );

	emscripten_set_main_loop( simulation_step, 0, false );
#endif

    startSystems();

    _jobScheduler.start();
}

bool Simulation::update( void )
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
        fetchCameras.forEachCamera( [&]( Camera *camera ) {
			if ( Camera::getMainCamera() == nullptr || camera->isMainCamera() ) {
				Camera::setMainCamera( camera );
			}
        });
	}
    
    broadcastMessage( messaging::SimulationWillUpdate { scene } );
    
    _jobScheduler.executeDelayedJobs();
    
    broadcastMessage( messaging::SimulationDidUpdate { scene } );
    
    if ( !_jobScheduler.isRunning() ) {
        return false;
    }

#if CRIMILD_SIMULATION_FORCE_SLEEP_ON_UPDATE
    auto frameEndTime = clock::now();
    auto delta = frameEndTime - frameStartTime;
    auto t = std::max(
      1ll,
      ( MIN_FRAME_TIME - std::chrono::duration_cast< std::chrono::nanoseconds>( delta ) ).count()
      );
    std::this_thread::sleep_for( std::chrono::nanoseconds( t ) );
#endif

    return true;
}

void Simulation::stop( void )
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
}

int Simulation::run( void )
{
	start();

#if !defined( CRIMILD_PLATFORM_EMSCRIPTEN )
    bool done = false;

    while ( !done ) {
        done = !update();
    }
#endif
    
	return 0;
}

void Simulation::addSystem( SystemPtr const &system )
{
    Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Adding system ", system->getClassName() );

    if ( !_systems.contains( system->getClassName() ) ) {
        _systems.insert( system->getClassName(), system );
    }
}

void Simulation::startSystems( void )
{
    Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Starting systems" );    
    _systems.eachValue( []( SystemPtr &s ) {
        if ( s != nullptr ) {
            s->start();
        }
    });
}

void Simulation::stopSystems( void )
{
    Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Stopping systems" );
    _systems.eachValue( []( SystemPtr &s ) {
        if ( s != nullptr ) {
            s->stop();
        }
    });
    
    _systems.clear();
}

void Simulation::setScene( SharedPointer< Node > const &scene )
{
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
}

void Simulation::loadScene( std::string filename )
{
    broadcastMessage( messaging::LoadScene { filename } );
}

void Simulation::forEachCamera( std::function< void ( Camera * ) > callback )
{
	for ( auto camera : _cameras ) {
		callback( camera );
	}
}

