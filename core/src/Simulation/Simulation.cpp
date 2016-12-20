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

using namespace crimild;

Simulation::Simulation( std::string name, SettingsPtr const &settings )
	: NamedObject( name ),
      _settings( settings ),
      _taskManager( 0 ) // by default, disable background threads
{
    Log::info( Version::getDescription() );
    
	addSystem( crimild::alloc< UpdateSystem >() );
	addSystem( crimild::alloc< RenderSystem >() );
    addSystem( crimild::alloc< DebugSystem >() );
    addSystem( crimild::alloc< StreamingSystem >() );
    addSystem( crimild::alloc< UISystem >() );
}

Simulation::~Simulation( void )
{
    stopSystems();
}

void Simulation::start( void )
{
    startSystems();

    _taskManager.start();
}

bool Simulation::update( void )
{
    auto scene = getScene();
    
    broadcastMessage( messaging::SimulationWillUpdate { scene } );
    
    _taskManager.pollMainTasks();
    
    broadcastMessage( messaging::SimulationDidUpdate { scene } );
    
	return _taskManager.isRunning();
}

void Simulation::stop( void )
{
    // stop all unfinished tasks first
    _taskManager.stop();

    setScene( nullptr );
    
    _assetManager.clear();
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
    Log::debug( "Adding system ", system->getName() );

	if ( _systems.find( system->getName() ) == _systems.end() ) {
		_systems.insert( std::make_pair( system->getName(), system ) );
	}
}

SystemPtr Simulation::getSystem( std::string name )
{
	return _systems[ name ];
}

void Simulation::startSystems( void )
{
    Log::debug( "Starting systems" );
	for ( auto s : _systems ) {
		if ( s.second != nullptr ) {
			s.second->start();
		}
	}
}

void Simulation::stopSystems( void )
{
    Log::debug( "Stopping systems" );
	for ( auto s : _systems ) {
		if ( s.second != nullptr ) {
			s.second->stop();
		}
	}
    
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
    }
    
    MessageQueue::getInstance()->clear();
    
    _simulationClock.reset();

    broadcastMessage( messaging::SceneChanged { crimild::get_ptr( _scene ) } );
}

void Simulation::loadScene( std::string filename, SharedPointer< SceneBuilder > const &builder )
{
    AssetManager::getInstance()->clear();

    auto self = this;
    crimild::async( [self, filename, builder] {
        self->broadcastMessage( messaging::LoadScene { filename, builder } );
    });
}

void Simulation::forEachCamera( std::function< void ( Camera * ) > callback )
{
	for ( auto camera : _cameras ) {
		callback( camera );
	}
}

