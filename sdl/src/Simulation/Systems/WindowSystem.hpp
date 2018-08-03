/*
 * Copyright (c) 2013 - 2018, Hugo Hernan Saez
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of CRIMILD nor the
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

#ifndef CRIMILD_SDL_SIMULATION_SYSTEMS_WINDOW_
#define CRIMILD_SDL_SIMULATION_SYSTEMS_WINDOW_

#include <Simulation/Systems/System.hpp>
#include <Foundation/Types.hpp>
#include <Mathematics/Clock.hpp>

#include <string>

#include <SDL.h>

namespace crimild {

	namespace sdl {
    
		class WindowSystem;
		
		namespace messaging {
			
			struct WindowSystemDidStart { 
				SDL_Window *window;
			};
			
			struct WindowSystemWillStop { 
				SDL_Window *window;
			};
			
			struct WindowSystemWillUpdate { 
				SDL_Window *window;
			};
			
			struct WindowSystemDidUpdate { 
				SDL_Window *window;
			};
			
			struct WindowSystemDidCreateWindow { 
				SDL_Window *window;
			};
			
			struct WindowSystemWillDestroyWindow { 
				SDL_Window *window;
			};
		}
		
		class WindowSystem : public System {
			CRIMILD_IMPLEMENT_RTTI( crimild::sdl::WindowSystem )
			
		public:
			WindowSystem( void );

			virtual ~WindowSystem( void );
			
			virtual crimild::Bool start( void ) override;
			
			virtual void update( void );
			
			virtual void stop( void ) override;

		private:
			crimild::Bool createWindow( void );
			void destroyWindow( void );
			
			crimild::Bool configureRenderer( void );
			void cleanupRenderer( void );

		private:
			SDL_Window *_window = nullptr;
			SDL_GLContext _renderContext = nullptr;
			Clock _clock;
		};

	}

}

#endif

