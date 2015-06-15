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

#include "Tasks/DispatchMessagesTask.hpp"
#include "Tasks/BeginRenderTask.hpp"
#include "Tasks/EndRenderTask.hpp"
#include "Tasks/UpdateSceneTask.hpp"
#include "Tasks/RenderSceneTask.hpp"
#include "Tasks/ComputeRenderQueueTask.hpp"
#include "Tasks/ProfilerDumpTask.hpp"

#include "SceneGraph/Camera.hpp"

#include "Visitors/FetchCameras.hpp"
#include "Visitors/UpdateWorldState.hpp"
#include "Visitors/UpdateRenderState.hpp"
#include "Visitors/StartComponents.hpp"

#include "Simulation/Systems/RenderSystem.hpp"
#include "Simulation/Systems/UpdateSystem.hpp"

using namespace crimild;

Simulation::Simulation( std::string name, int argc, char **argv )
	: Simulation( name, argc, argv, false )
{

}

Simulation::Simulation( std::string name, int argc, char **argv, bool enableBackgroundLoop )
	: NamedObject( name ),
      _mainLoop( crimild::alloc< RunLoop >( "Main Loop" ) )
{
	// if ( enableBackgroundLoop ) {
    	// _simulationLoop = crimild::alloc< ThreadedRunLoop >( "Background Loop", true );
    // }
    // else {
    	_simulationLoop = _mainLoop;
    // }
    
	// srand( time( NULL ) );

	_settings.parseCommandLine( argc, argv );
	
	FileSystem::getInstance().init( argc, argv );

	addSystem( crimild::alloc< UpdateSystem >() );
	addSystem( crimild::alloc< RenderSystem >() );

	// todo: not sure about this
	// getMainLoop()->startTask( crimild::alloc< ProfilerDumpTask >( Priorities::END_RENDER_PRIORITY ) );
}

Simulation::~Simulation( void )
{
	stopSystems();
}

RunLoopPtr Simulation::getMainLoop( void )
{
	return _mainLoop;
}

RunLoopPtr Simulation::getSimulationLoop( void ) 
{
    return _simulationLoop;
}

void Simulation::start( void )
{
	// getMainLoop()->startTask( crimild::alloc< BeginRenderTask >( Priorities::BEGIN_RENDER_PRIORITY ) );
	// getMainLoop()->startTask( crimild::alloc< EndRenderTask >( Priorities::END_RENDER_PRIORITY ) );
	// getMainLoop()->startTask( crimild::alloc< RenderSceneTask >( Priorities::RENDER_SCENE_PRIORITY ) );

    // getSimulationLoop()->startTask( crimild::alloc< DispatchMessagesTask >( Priorities::HIGHEST_PRIORITY ) );
    // getSimulationLoop()->startTask( crimild::alloc< UpdateSceneTask >( Priorities::UPDATE_SCENE_PRIORITY ) );
    // getSimulationLoop()->startTask( crimild::alloc< ComputeRenderQueueTask >( Priorities::RENDER_SCENE_PRIORITY ) );

    startSystems();
}

bool Simulation::step( void )
{
	// bool result = _mainLoop->update();
	// return result;
	return true;
}

void Simulation::stop( void )
{
	broadcastMessage( messages::TerminateTasks { } );
    // if ( _simulationLoop != nullptr ) {
        // _simulationLoop->stop();
    // }
    
    // if ( _mainLoop != nullptr ) {
        // _mainLoop->stop();
    // }
}

int Simulation::run( void )
{
	start();

	_taskManager.run();

	stopSystems();

	return 0;
}

void Simulation::addTask( TaskPtr const &task )
{
	_taskManager.addTask( task );		
}

void Simulation::addSystem( SystemPtr const &system )
{
	Log::Debug << "Adding system " << system->getName() << Log::End;

	if ( _systems.find( system->getName() ) == _systems.end() ) {
		if ( system->getUpdater() != nullptr ) {
			Log::Debug << "Adding task for system update" << Log::End;
			_taskManager.addTask( system->getUpdater() );
		}
		else {
			Log::Warning << "System does not provide updater" << Log::End;
		}

		_systems.insert( std::make_pair( system->getName(), system ) );
	}
}

SystemPtr Simulation::getSystem( std::string name )
{
	return _systems[ name ];
}

void Simulation::startSystems( void )
{
	Log::Debug << "Starting systems" << Log::End;
	for ( auto s : _systems ) {
		if ( s.second != nullptr ) {
			s.second->start();
		}
	}
}

void Simulation::stopSystems( void )
{
	Log::Debug << "Stopping systems" << Log::End;
	for ( auto s : _systems ) {
		if ( s.second != nullptr ) {
			s.second->stop();
		}
	}
    
    _systems.clear();
}

void Simulation::setScene( NodePtr const &scene )
{
	_scene = scene;
	_cameras.clear();

	if ( _scene != nullptr ) {
		_scene->perform( UpdateWorldState() );
		_scene->perform( UpdateRenderState() );
		_scene->perform( StartComponents() );

		FetchCameras fetchCameras;
		_scene->perform( fetchCameras );
        fetchCameras.foreachCamera( [&]( CameraPtr const &camera ) {
            _cameras.push_back( camera );
        });
	}
	else {
		_assetManager.clear();
	}
}

void Simulation::forEachCamera( std::function< void ( CameraPtr const & ) > callback )
{
	for ( auto camera : _cameras ) {
		callback( camera );
	}
}

