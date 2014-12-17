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
	: Task( priority )
{
}

WindowTask::~WindowTask( void )
{

}

void WindowTask::start( void )
{
	glfwOpenWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	glfwOpenWindowHint( GLFW_OPENGL_VERSION_MAJOR, 3 );
	glfwOpenWindowHint( GLFW_OPENGL_VERSION_MINOR, 2 );
    glfwOpenWindowHint( GLFW_WINDOW_NO_RESIZE, GL_TRUE );

    auto screenBuffer = Simulation::getCurrent()->getRenderer()->getScreenBuffer();

    if ( !glfwOpenWindow( screenBuffer->getWidth(), screenBuffer->getHeight(), 
    					  8, 8, 8, 8,
    					  16, 0,
    					  GLFW_WINDOW ) ) {
    	throw RuntimeException( "Cannot created main window" );
    }
    
    glfwSwapInterval( 0 );
    
    glfwSetWindowTitle( Simulation::getCurrent()->getName().c_str() );
}

void WindowTask::stop( void )
{

}

void WindowTask::update( void )
{
    static double min = 1.0f;
    static double max = 0.0f;
    static double accum = 0.0f;
    static Time windowTime;
    
	if ( glfwGetWindowParam( GLFW_OPENED ) ) {
		glfwSwapBuffers();

		Time &t = Simulation::getCurrent()->getSimulationTime();
        
        windowTime.update( t.getCurrentTime() );
        double delta = windowTime.getDeltaTime();
        
        accum += delta;
        if ( accum >= 1.0f ) {
            max = delta;
            min = delta;
            accum = 0.0f;
        }
        
        max = Numericf::max( max, delta );
        min = Numericf::min( min, delta );

        std::stringstream str;
		str.precision( 6 );
		str << Simulation::getCurrent()->getName()
			<< " "
            << "(max=" << std::fixed << max << "ms"
            << " min=" << min << "ms"
            << " avg=" << (0.5f * (min + max)) << "ms"
            << ")";
        
		glfwSetWindowTitle( str.str().c_str() );
#if 1
        if ( windowTime.getDeltaTime() < 0.002 ) {
            // this trick prevents the simulation to run at very high speeds
            // this usually happens when the window is sent to background
            // and there is nothing to render
            int sleepTime = ( int )( ( 1.0 / 60.0 - windowTime.getDeltaTime() ) * 1000.0 );
            std::this_thread::sleep_for( std::chrono::milliseconds( sleepTime ) );
        }
#endif
	}
	else {
		Simulation::getCurrent()->stop();
	}
}

