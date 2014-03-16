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

#include "WindowTask.hpp"
#include "Rendering/GL3/Renderer.hpp"

#include <GL/glfw.h>

using namespace crimild;

WindowTask::WindowTask( int priority, int width, int height )
	: Task( priority ),
	  _width( width ),
	  _height( height )
{
}

WindowTask::~WindowTask( void )
{

}

void WindowTask::start( void )
{
	Pointer< FrameBufferObject > screenBuffer( new FrameBufferObject( _width, _height ) );

	glfwOpenWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	glfwOpenWindowHint( GLFW_OPENGL_VERSION_MAJOR, 3 );
	glfwOpenWindowHint( GLFW_OPENGL_VERSION_MINOR, 2 );
    glfwOpenWindowHint( GLFW_WINDOW_NO_RESIZE, GL_TRUE );

    if ( !glfwOpenWindow( screenBuffer->getWidth(), screenBuffer->getHeight(), 
    					  8, 8, 8, 8,
    					  16, 0,
    					  GLFW_WINDOW ) ) {
    	throw RuntimeException( "Cannot created main window" );
    }

	Simulation::getCurrent()->setRenderer( new gl3::Renderer( screenBuffer.get() ) );
}

void WindowTask::stop( void )
{

}

void WindowTask::update( void )
{
	if ( glfwGetWindowParam( GLFW_OPENED ) ) {
		glfwSwapBuffers();
	}
	else {
		Simulation::getCurrent()->stop();
	}
}

