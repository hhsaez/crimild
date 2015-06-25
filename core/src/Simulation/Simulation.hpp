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

#include "Concurrency/TaskManager.hpp"

#include "Systems/System.hpp"

#include "Settings.hpp"
#include "AssetManager.hpp"

#include "Foundation/NamedObject.hpp"
#include "Foundation/Profiler.hpp"
#include "Foundation/Singleton.hpp"
#include "Foundation/Streaming.hpp"

#include "Messaging/MessageQueue.hpp"

#include "Mathematics/Clock.hpp"
#include "SceneGraph/Node.hpp" 
#include "SceneGraph/Camera.hpp"
#include "Rendering/Renderer.hpp"

#include <functional>
#include <list>
#include <thread>

namespace crimild {
    
    namespace messaging {
        
        struct SceneChanged {
            NodePtr scene;
        };
        
        struct SimulationWillUpdate {
            NodePtr scene;
        };
        
        struct SimulationDidUpdate {
            NodePtr scene;
        };
        
    }

	class Simulation : 
		public NamedObject, 
		public SharedObject,
		public Messenger,
		public DynamicSingleton< Simulation > {

	public:
		Simulation( std::string name, int argc, char **argv );
		virtual ~Simulation( void );

        virtual void start( void );
        virtual bool update( void );
        virtual void stop( void );
            
        virtual int run( void );
        
    public:
		Settings &getSettings( void ) { return _settings; }

    private:
        Settings _settings;
            
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
		TaskManager _taskManager;

	public:
		void addSystem( SystemPtr const &system );
		
		SystemPtr getSystem( std::string name );

		template< class SYSTEM_CLASS >
        SharedPointer< SYSTEM_CLASS > getSystem( std::string name )
		{
            return std::static_pointer_cast< SYSTEM_CLASS >( getSystem( name ) );
		}

	private:
		void startSystems( void );
		void stopSystems( void );

	private:
		using SystemMap = std::map< std::string, SystemPtr >;
		SystemMap _systems;
        
	public:
		void setRenderer( RendererPtr const &renderer ) { _renderer = renderer; }
		RendererPtr getRenderer( void ) { return _renderer; }

	private:
		RendererPtr _renderer;

	public:
		void setScene( NodePtr const &scene );
		NodePtr getScene( void ) { return _scene; }
            
        void loadScene( std::string filename, SceneBuilderPtr const &sceneBuilder = nullptr );

        CameraPtr getMainCamera( void ) { return _cameras.size() > 0 ? _cameras.front() : CameraPtr(); }
		void forEachCamera( std::function< void ( CameraPtr const & ) > callback );

	private:
		NodePtr _scene;
		std::list< CameraPtr > _cameras;
	};

}

#endif

