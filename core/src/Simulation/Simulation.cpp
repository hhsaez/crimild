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

#include "SceneGraph/Camera.hpp"

#include "Visitors/FetchCameras.hpp"
#include "Visitors/UpdateWorldState.hpp"
#include "Visitors/UpdateRenderState.hpp"
#include "Visitors/StartComponents.hpp"

using namespace crimild;

#define CRIMILD_ENABLE_SIMULATION_THREAD 1

Simulation *Simulation::_currentSimulation = nullptr;

Simulation::Simulation( std::string name, int argc, char **argv )
	: NamedObject( name ),
      _mainLoop( crimild::alloc< RunLoop >() )
{
#if CRIMILD_ENABLE_SIMULATION_THREAD
    _simulationLoop = crimild::alloc< ThreadedRunLoop >( true );
#endif
    
	srand( time( NULL ) );

	_currentSimulation = this;

	_settings.parseCommandLine( argc, argv );
	
	FileSystem::getInstance().init( argc, argv );
}

Simulation::~Simulation( void )
{
	_currentSimulation = nullptr;
}

RunLoopPtr Simulation::getMainLoop( void )
{
	return _mainLoop;
}

RunLoopPtr Simulation::getSimulationLoop( void ) 
{
#if CRIMILD_ENABLE_SIMULATION_THREAD
    return _simulationLoop;
#else
    return _mainLoop;
#endif
}

void Simulation::start( void )
{
	getMainLoop()->startTask( crimild::alloc< BeginRenderTask >( Priorities::BEGIN_RENDER_PRIORITY ) );
	getMainLoop()->startTask( crimild::alloc< EndRenderTask >( Priorities::END_RENDER_PRIORITY ) );
	getMainLoop()->startTask( crimild::alloc< RenderSceneTask >( Priorities::RENDER_SCENE_PRIORITY ) );

    getSimulationLoop()->startTask( crimild::alloc< DispatchMessagesTask >( Priorities::HIGHEST_PRIORITY ) );
    getSimulationLoop()->startTask( crimild::alloc< UpdateSceneTask >( Priorities::UPDATE_SCENE_PRIORITY ) );
    getSimulationLoop()->startTask( crimild::alloc< ComputeRenderQueueTask >( Priorities::RENDER_SCENE_PRIORITY ) );
}

bool Simulation::step( void )
{
	return _mainLoop->update();
}

void Simulation::stop( void )
{
    if ( _simulationLoop != nullptr ) {
        _simulationLoop->stop();
    }
    
    if ( _mainLoop != nullptr ) {
        _mainLoop->stop();
    }
}

int Simulation::run( void )
{
	start();
	while( step() );
	return 0;
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

