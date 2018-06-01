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

Simulation::Simulation( std::string name, SettingsPtr const &settings )
	: NamedObject( name ),
      _settings( settings )
{
    Version version;
    Log::info( CRIMILD_CURRENT_CLASS_NAME, version.getDescription() );

    // worker threads are disabled by default
    _jobScheduler.configure( 0 );
    
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
    startSystems();

    _jobScheduler.start();
}

bool Simulation::update( void )
{
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
    
	return _jobScheduler.isRunning();
}

void Simulation::stop( void )
{
    // stop all unfinished tasks first
    _jobScheduler.stop();

    // clear all messages
    MessageQueue::getInstance()->clear();
    
    setScene( nullptr );
    
    _assetManager.clear( true );
}

int Simulation::run( void )
{
	start();

    bool done = false;
    while ( !done ) {
        done = !update();
    }
    
	return 0;
}

void Simulation::addSystem( SystemPtr const &system )
{
    Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Adding system ", system->getClassName() );

    if ( !_systems.contains( system->getClassName() ) ) {
        _systems[ system->getClassName() ] = system;
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

