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

#ifndef CRIMILD_SIMULATION_
#define CRIMILD_SIMULATION_

#include "RunLoop.hpp"
#include "Foundation/NamedObject.hpp"
#include "SceneGraph/Node.hpp" 
#include "Rendering/Renderer.hpp"

namespace Crimild {

	class Simulation : public NamedObject {
	private:
		static Simulation *_currentSimulation;

	public:
		static Simulation *getCurrent( void ) { return _currentSimulation; }

	public:
		Simulation( std::string name = "" );
		virtual ~Simulation( void );

		RunLoop *getMainLoop( void ) { return _mainLoop.get(); }

		virtual void start( void );

		virtual bool step( void );
		
		virtual void stop( void );

		virtual int run( void );

	private:
		RunLoopPtr _mainLoop;

	public:
		void setRenderer( RendererPtr renderer ) { _renderer = renderer; }
		Renderer *getRenderer( void ) { return _renderer.get(); }

	private:
		RendererPtr _renderer;

	public:
		void attachScene( NodePtr scene );
	};

	typedef std::shared_ptr< Simulation > SimulationPtr;

}

#endif

