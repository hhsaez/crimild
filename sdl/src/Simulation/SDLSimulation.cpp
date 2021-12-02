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

#include "SDLSimulation.hpp"

#include "Systems/WindowSystem.hpp"
#include "Systems/SDLEventSystem.hpp"
#include "Systems/AudioSystem.hpp"

#include <Exceptions/RuntimeException.hpp>
#include <Rendering/OpenGLRenderer.hpp>
#include <Simulation/Systems/StreamingSystem.hpp>
#include <Simulation/FileSystem.hpp>

#include <SDL.h>

using namespace crimild;
using namespace crimild::concurrency;
using namespace crimild::sdl;

#ifdef CRIMILD_ENABLE_SCRIPTING
#include <Coding/LuaDecoder.hpp>

using namespace crimild::scripting;
#endif

#ifdef CRIMILD_ENABLE_PHYSICS
#include <Simulation/Systems/PhysicsSystem.hpp>

using namespace crimild::physics;
#endif

SDLSimulation::SDLSimulation( std::string name, SettingsPtr const &settings )
	: Simulation( name, settings )
{
	if ( SDL_Init( 0 ) != 0 ) {
		Log::fatal( CRIMILD_CURRENT_CLASS_NAME, "SDL_Init failed: ", SDL_GetError() );
		exit( 1 );
	}

	char *basePath = SDL_GetBasePath();
	if ( basePath ){
//        FileSystem::getInstance().setBaseDirectory( basePath );
		SDL_free( basePath );
	}

	if ( JobScheduler::getInstance()->getNumWorkers() == 0 ) {
		// enable some threads if not already specified
        if ( settings != nullptr ) {
            auto workerCount = settings->get< crimild::Int32 >( "simulation.threading.workers", 0 );
            JobScheduler::getInstance()->configure( workerCount );
        }
	}

#ifdef CRIMILD_ENABLE_SCRIPTING
	getSystem< StreamingSystem >()->registerDecoder< coding::LuaDecoder >( "lua" );
#endif

    addSystem( crimild::alloc< SDLEventSystem >() );
    addSystem( crimild::alloc< WindowSystem >() );
	addSystem( crimild::alloc< AudioSystem >() );

#ifdef CRIMILD_ENABLE_PHYSICS
    addSystem( crimild::alloc< PhysicsSystem >() );
#endif
    
    setRenderer( crimild::alloc< opengl::OpenGLRenderer >() );
}

SDLSimulation::~SDLSimulation( void )
{
	SDL_Quit();
}

