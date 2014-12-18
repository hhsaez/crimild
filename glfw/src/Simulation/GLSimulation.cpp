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

#include "GLSimulation.hpp"
#include "Tasks/WindowTask.hpp"
#include "Tasks/UpdateTimeTask.hpp"
#include "Tasks/UpdateInputStateTask.hpp"
#include "Tasks/UpdateSceneAndPhysicsTask.hpp"
#include "Rendering/GL3/Renderer.hpp"

#include <Crimild_Scripting.hpp>
#include <Crimild_Physics.hpp>

using namespace crimild;
using namespace crimild::scripting;
using namespace crimild::physics;

GLSimulation::GLSimulation( std::string name, int argc, char **argv )
	: Simulation( name, argc, argv ),
	  _window( nullptr )
{
	if ( !glfwInit() ) {
		Log::Error << "Cannot start GLFW: glfwInit failed" << Log::End;
		throw RuntimeException( "Cannot start GLFW: glwfInit failed!" );
	}

	loadSettings();
	init();
}

GLSimulation::~GLSimulation( void )
{
	glfwTerminate();
}

void GLSimulation::start( void ) 
{
    getMainLoop()->startTask( std::make_shared< WindowTask >( Simulation::Priorities::LOWEST_PRIORITY, _window ) );
	getMainLoop()->startTask( std::make_shared< BeginRenderTask >( Priorities::BEGIN_RENDER_PRIORITY ) );
	getMainLoop()->startTask( std::make_shared< RenderSceneTask >( Priorities::RENDER_SCENE_PRIORITY ) );
	getMainLoop()->startTask( std::make_shared< EndRenderTask >( Priorities::END_RENDER_PRIORITY ) );
	getMainLoop()->startTask( std::make_shared< UpdateInputStateTask >( Simulation::Priorities::HIGHEST_PRIORITY, _window ) );
	
    getSimulationLoop()->startTask( std::make_shared< DispatchMessagesTask >( Priorities::HIGHEST_PRIORITY ) );
	getSimulationLoop()->startTask( std::make_shared< UpdateTimeTask >( Simulation::Priorities::LOWEST_PRIORITY ) );
    getSimulationLoop()->startTask( std::make_shared< UpdateSceneAndPhysicsTask >( Priorities::UPDATE_SCENE_PRIORITY ) );
    getSimulationLoop()->startTask( std::make_shared< ComputeRenderQueueTask >( Priorities::RENDER_SCENE_PRIORITY ) );
}

void GLSimulation::loadSettings( void )
{
	ScriptContext context;
	if ( context.load( FileSystem::getInstance().pathForResource( "crimild.lua" ) ) ) {
		getSettings().add( "video.width", context.eval< int >( "video.width"  ) );
		getSettings().add( "video.height", context.eval< int >( "video.height"  ) );
		
		getSettings().add( "video.clearColor.r", context.eval< float >( "video.clearColor.r"  ) );
		getSettings().add( "video.clearColor.g", context.eval< float >( "video.clearColor.g"  ) );
		getSettings().add( "video.clearColor.b", context.eval< float >( "video.clearColor.b"  ) );
		getSettings().add( "video.clearColor.a", context.eval< float >( "video.clearColor.a"  ) );

		getSettings().add( "physics.gravity.x", context.eval< float >( "physics.gravity.x"  ) );
		getSettings().add( "physics.gravity.y", context.eval< float >( "physics.gravity.y"  ) );
		getSettings().add( "physics.gravity.z", context.eval< float >( "physics.gravity.z"  ) );
	}
}

void GLSimulation::init( void )
{
	int width = getSettings().get( "video.width", 1024 );
	int height = getSettings().get( "video.height", 768 );
	float r = getSettings().get( "video.clearColor.r", 0.0f );
	float g = getSettings().get( "video.clearColor.g", 0.0f );
	float b = getSettings().get( "video.clearColor.b", 0.0f );
	float a = getSettings().get( "video.clearColor.a", 1.0f );

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 2 );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );

	_window = glfwCreateWindow( width, height, getName().c_str(), NULL, NULL );
	if ( _window == nullptr ) {
		Log::Error << "Cannot create window" << Log::End;
		glfwTerminate();
		throw new RuntimeException( "Cannot create window" );
	}

  	glfwMakeContextCurrent( _window );

    int framebufferWidth, framebufferHeight;
	glfwGetFramebufferSize( _window, &framebufferWidth, &framebufferHeight);

    auto screenBuffer = std::make_shared< FrameBufferObject >( framebufferWidth, framebufferHeight );
	screenBuffer->setClearColor( RGBAColorf( r, g, b, a ) );
    Simulation::getCurrent()->setRenderer( std::make_shared< gl3::Renderer >( screenBuffer ) );
}

