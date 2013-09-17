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
#include "Tasks/BeginRenderTask.hpp"
#include "Tasks/EndRenderTask.hpp"
#include "Tasks/UpdateSceneTask.hpp"
#include "Tasks/UpdatePhysicsTask.hpp"
#include "Tasks/RenderSceneTask.hpp"
#include "SceneGraph/Camera.hpp"
#include "Visitors/FetchCameras.hpp"

#define UPDATE_SCENE_PRIORITY 100
#define UPDATE_PHYSICS_PRIORITY 200
#define BEGIN_RENDER_PRIORITY 1000
#define RENDER_SCENE_PRIORITY 2000
#define END_RENDER_PRIORITY 3000

using namespace crimild;

Simulation *Simulation::_currentSimulation = nullptr;

Simulation::Simulation( std::string name, int argc, char **argv )
	: NamedObject( name ),
	  _mainLoop( new RunLoop() )
{
	_currentSimulation = this;

	FileSystem::getInstance().init( argc, argv );
}

Simulation::~Simulation( void )
{
	_currentSimulation = nullptr;
}

void Simulation::start( void )
{
	Log::Info << "Starting simulation \"" << getName() << "\"" << Log::End;

	BeginRenderTaskPtr beginRender( new BeginRenderTask( BEGIN_RENDER_PRIORITY ) );
	getMainLoop()->startTask( beginRender );

	EndRenderTaskPtr endRender( new EndRenderTask( END_RENDER_PRIORITY ) );
	getMainLoop()->startTask( endRender );
}

bool Simulation::step( void )
{
	return _mainLoop->update();
}

void Simulation::stop( void )
{
	_mainLoop->stop();
	
	Log::Info << "Simulation completed" << Log::End;
}

int Simulation::run( void )
{
	start();
	while( step() );
	return 0;
}

void Simulation::attachScene( NodePtr scene )
{
	FetchCameras fetchCameras;
	scene->perform( fetchCameras );
	if ( fetchCameras.hasCameras() ) {
		fetchCameras.foreachCamera( [&]( Camera *camera ) mutable {
			UpdateSceneTaskPtr updateScene( new UpdateSceneTask( UPDATE_SCENE_PRIORITY, scene ) );
			getMainLoop()->startTask( updateScene );

			UpdatePhysicsTaskPtr updatePhysics( new UpdatePhysicsTask( UPDATE_PHYSICS_PRIORITY, scene ) );
			getMainLoop()->startTask( updatePhysics );

			RenderSceneTaskPtr renderScene( new RenderSceneTask( RENDER_SCENE_PRIORITY, scene, camera ) );
			getMainLoop()->startTask( renderScene );
		});
	}
	else {
		Log::Error << "Cannot find cameras for scene with name " << ( scene->getName().length() > 0 ? scene->getName() : "<unknown>" ) << Log::End;
	}

}

