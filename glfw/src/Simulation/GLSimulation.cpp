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

#include "Systems/WindowSystem.hpp"
#include "Systems/InputSystem.hpp"

#include <Crimild_OpenGL.hpp>

using namespace crimild;

#ifdef CRIMILD_ENABLE_PHYSICS
#include <Crimild_Physics.hpp>
#endif

GLSimulation::GLSimulation( std::string name, SettingsPtr const &settings )
	: Simulation( name, settings )
{
	if ( !glfwInit() ) {
		Log::Error << "Cannot start GLFW: glfwInit failed" << Log::End;
		throw RuntimeException( "Cannot start GLFW: glwfInit failed!" );
	}

	if ( TaskManager::getInstance()->getNumThreads() == 0 ) {
		// enable some threads if not already specified
		TaskManager::getInstance()->setNumThreads( 2 );
	}

    addSystem( crimild::alloc< WindowSystem >() );
    addSystem( crimild::alloc< InputSystem >() );
    
    setRenderer( crimild::alloc< opengl::OpenGLRenderer >() );
}

GLSimulation::~GLSimulation( void )
{
	glfwTerminate();
}

