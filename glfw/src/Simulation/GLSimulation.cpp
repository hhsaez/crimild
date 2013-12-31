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

#include <GL/glfw.h>

using namespace crimild;

GLSimulation::GLSimulation( std::string name, int argc, char **argv )
	: Simulation( name, argc, argv ),
	  _width( 1024 ),
	  _height( 768 )
{
	for ( int i = 1; i < argc; i++ ) {
		std::string arg( argv[ i ] );
		if ( arg == "--width" ) {
			_width = atoi( argv[ i + 1 ] );
		}
		else if ( arg == "--height" ) {
			_height = atoi( argv[ i + 1 ] );
		}
	}
}

GLSimulation::~GLSimulation( void )
{
	glfwTerminate();
}

void GLSimulation::start( void ) 
{
	if ( !glfwInit() ) {
		throw RuntimeException( "Cannot start GLFW: glwfInit failed!" );
	}

	getMainLoop()->startTask( new WindowTask( 9000, _width, _height ) );
	getMainLoop()->startTask( new UpdateTimeTask( 9999 ) );
	getMainLoop()->startTask( new UpdateInputStateTask( 0 ) );

	Simulation::start();
}

