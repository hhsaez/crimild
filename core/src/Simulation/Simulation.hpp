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

#include "Concurrency/JobScheduler.hpp"

#include "Systems/System.hpp"

#include "Settings.hpp"
#include "AssetManager.hpp"
#include "Input.hpp"

#include "Foundation/NamedObject.hpp"
#include "Foundation/Profiler.hpp"
#include "Foundation/Singleton.hpp"

#include "Streaming/SceneBuilder.hpp"

#include "Messaging/MessageQueue.hpp"

#include "Mathematics/Clock.hpp"
#include "SceneGraph/Node.hpp" 
#include "SceneGraph/Camera.hpp"
#include "Rendering/Renderer.hpp"
#include "Audio/AudioManager.hpp"

#include <functional>
#include <list>
#include <thread>

#ifdef CRIMILD_PLATFORM_EMSCRIPTEN
#define CRIMILD_SIMULATION_LIFETIME static
#else
#define CRIMILD_SIMULATION_LIFETIME
#endif

namespace crimild {
    
    namespace messaging {
        
        struct SceneChanged {
            Node *scene;
        };
        
        struct SimulationWillUpdate {
            Node *scene;
        };
        
        struct SimulationDidUpdate {
            Node *scene;
        };
        
    }

	class Simulation : 
		public NamedObject, 
		public SharedObject,
		public Messenger,
		public DynamicSingleton< Simulation > {

	public:
		Simulation( std::string name, SettingsPtr const &settings );
		virtual ~Simulation( void );

        virtual void start( void );
        virtual bool update( void );
        virtual void stop( void );
            
        virtual int run( void );
            
    private:
        concurrency::JobScheduler _jobScheduler;
        
    public:
		SettingsPtr &getSettings( void ) { return _settings; }

    private:
        SettingsPtr _settings;
            
    public:
		Clock &getSimulationClock( void ) { return _simulationClock; }
		const Clock &getSimulationClock( void ) const { return _simulationClock; }

    private:
        Clock _simulationClock;

    public:
        AssetManager &getAssets( void ) { return _assetManager; }
            
    private:
        AssetManager _assetManager;
            
    private:
		Profiler _profiler;
        Input _input;

	public:
		void addSystem( SystemPtr const &system );
		
		System *getSystemWithName( std::string name ) { return crimild::get_ptr( _systems[ name ] ); }

		template< class SYSTEM_CLASS >
        SYSTEM_CLASS *getSystem( void )
		{
            return static_cast< SYSTEM_CLASS * >( getSystemWithName( SYSTEM_CLASS::__CLASS_NAME ) );
		}

	private:
		void startSystems( void );
		void stopSystems( void );

	private:
		using SystemMap = containers::Map< std::string, SystemPtr >;
		SystemMap _systems;
        
	public:
		void setRenderer( SharedPointer< Renderer > const &renderer ) { _renderer = renderer; }
        Renderer *getRenderer( void ) { return crimild::get_ptr( _renderer ); }

	private:
		SharedPointer< Renderer > _renderer;

	public:
		void setScene( SharedPointer< Node > const &scene );
        Node *getScene( void ) { return crimild::get_ptr( _scene ); }
            
        void loadScene( std::string filename );

        Camera *getMainCamera( void ) { return Camera::getMainCamera(); }
		void forEachCamera( std::function< void ( Camera * ) > callback );

	private:
		SharedPointer< Node > _scene;
		std::list< Camera * > _cameras;

	public:
		audio::AudioManager *getAudioManager( void ) { return crimild::get_ptr( _audioManager ); }
		void setAudioManager( SharedPointer< audio::AudioManager > const &audioManager ) { _audioManager = audioManager; }

	private:
		SharedPointer< audio::AudioManager > _audioManager;
	};

}

#endif

