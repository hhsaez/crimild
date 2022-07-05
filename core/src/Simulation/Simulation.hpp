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
 *     * Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CRIMILD_SIMULATION_
#define CRIMILD_SIMULATION_

#include "AssetManager.hpp"
#include "Concurrency/JobScheduler.hpp"
#include "Foundation/NamedObject.hpp"
#include "Foundation/Profiler.hpp"
#include "Foundation/Singleton.hpp"
#include "Input.hpp"
#include "Mathematics/Clock.hpp"
#include "Rendering/Renderer.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Node.hpp"
#include "Settings.hpp"
#include "Simulation/Event.hpp"
#include "Systems/System.hpp"

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

    /**
       \brief Implements a simulation that is executed by the application

       Clients should inherit from this class and override `onAwake` and/or `onStarted`
       with their own implementations.

       Different platforms might use either `run` or call `start`, `update` and `stop` manually.
     */
    class Simulation
        : public NamedObject,
          public RTTI,
          public DynamicSingleton< Simulation > {
        CRIMILD_IMPLEMENT_RTTI( crimild::Simulation )

    public:
        static std::unique_ptr< Simulation > create( void ) noexcept;

    public:
        Simulation( void ) = default;
        virtual ~Simulation( void ) = default;

        virtual Event handle( const Event &e ) noexcept;

        /**
           \name Hooks
         */
        //@{

    public:
        /**
           \brief Executed when the simulation is about to start

           This method is called before any system is initialized. It's a good entry point
           for users to add new systems. Advanced users can remove all existing systems
           and configure a simulation based on their own needs.
         */
        [[deprecated]] virtual void onAwake( void ) noexcept { }

        /**
           brief Executed after all systems have been started

           This is a good entry point for clients to load an initial scene or
           setup the frame composition
         */
        [[deprecated]] virtual void onStarted( void ) noexcept { }

        //@}

    public:
        void start( void ) noexcept;
        bool step( void ) noexcept;
        void stop( void ) noexcept;

        void pause( void ) noexcept { m_running = false; }
        void resume( void ) noexcept { m_running = true; }

    private:
        bool m_running = false;

    public:
        inline void setSettings( SettingsPtr const &settings ) noexcept { _settings = settings; }
        inline Settings *getSettings( void ) { return crimild::get_ptr( _settings ); }

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

        // public:
        //     void attachSystem( SharedPointer< System > const &system ) noexcept;

        //     template< typename SystemType >
        //     SystemType *attachSystem( void ) noexcept
        //     {
        //         auto system = crimild::alloc< SystemType >();
        //         attachSystem( system );
        //         return crimild::get_ptr( system );
        //     }

        //     void detachAllSystems( void ) noexcept;

        // private:
        //     using SystemArray = Array< SharedPointer< System > >;
        //     SystemArray m_systems;

    public:
        void setRenderer( SharedPointer< Renderer > const &renderer ) { _renderer = renderer; }
        Renderer *getRenderer( void ) { return crimild::get_ptr( _renderer ); }

    private:
        SharedPointer< Renderer > _renderer;

    public:
        void setScene( SharedPointer< Node > const &scene );
        Node *getScene( void ) { return crimild::get_ptr( _scene ); }

        /**
         * \brief Get the main camera for the current scene
         */
        inline const Camera *getMainCamera( void ) const noexcept { return m_mainCamera; }
        inline Camera *getMainCamera( void ) noexcept { return m_mainCamera; }

        void forEachCamera( std::function< void( Camera * ) > callback );

    private:
        SharedPointer< Node > _scene;
        std::vector< Camera * > m_cameras;
        Camera *m_mainCamera = nullptr;
    };

}

#define CRIMILD_CREATE_SIMULATION( SimulationType, SimulationName )                     \
    std::unique_ptr< crimild::Simulation > crimild::Simulation::create( void ) noexcept \
    {                                                                                   \
        auto sim = std::make_unique< SimulationType >();                                \
        if ( auto settings = crimild::Settings::getInstance() ) {                       \
            settings->set( crimild::Settings::SETTINGS_APP_NAME, SimulationName );      \
        }                                                                               \
        sim->setName( SimulationName );                                                 \
        return sim;                                                                     \
    }

#endif
