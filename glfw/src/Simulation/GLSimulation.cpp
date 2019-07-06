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

#include "GLSimulation.hpp"

#include "Systems/EventSystem.hpp"
#include "Systems/WindowSystem.hpp"
#include "Systems/InputSystem.hpp"

//#include <Rendering/OpenGLRenderer.hpp>
#include <Simulation/Systems/StreamingSystem.hpp>

using namespace crimild;
using namespace crimild::concurrency;
using namespace crimild::glfw;

#ifdef CRIMILD_ENABLE_SCRIPTING
#include <Coding/LuaDecoder.hpp>

using namespace crimild::scripting;
#endif

#ifdef CRIMILD_ENABLE_PHYSICS
#include <Simulation/Systems/PhysicsSystem.hpp>

using namespace crimild::physics;
#endif

#ifdef CRIMILD_ENABLE_SFML
#include <Audio/SFMLAudioManager.hpp>
#endif

void errorCallback( int error, const char* description )
{
	std::cerr << "GLFW Error: (" << error << ") " << description << std::endl;
}

GLSimulation::GLSimulation( std::string name, SettingsPtr const &settings )
	: Simulation( name, settings )
{
#ifdef CRIMILD_ENABLE_SFML
	setAudioManager( crimild::alloc< sfml::SFMLAudioManager >() );
#endif

	if ( !glfwInit() ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot start GLFW: glfwInit failed" );
		exit( 1 );
	}

	glfwSetErrorCallback( errorCallback );

	int versionMajor;
	int versionMinor;
	int versionRevision;
	glfwGetVersion( &versionMajor, &versionMinor, &versionRevision );
	CRIMILD_LOG_INFO( "Initializing GLFW v", versionMajor, ".", versionMinor, " rev. ", versionRevision );

#ifdef CRIMILD_ENABLE_SCRIPTING
	//getSystem< StreamingSystem >()->registerDecoder< coding::LuaDecoder >( "lua" );
#endif

	addSystem( crimild::alloc< EventSystem >() );
    addSystem( crimild::alloc< InputSystem >() );
    addSystem( crimild::alloc< WindowSystem >() );

#ifdef CRIMILD_ENABLE_PHYSICS
    //addSystem( crimild::alloc< PhysicsSystem >() );
#endif
    
    //setRenderer( crimild::alloc< opengl::OpenGLRenderer >() );
}

GLSimulation::~GLSimulation( void )
{
	glfwTerminate();
}

