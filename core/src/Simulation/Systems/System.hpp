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

#ifndef CRIMILD_SIMULATION_SYSTEM_
#define CRIMILD_SIMULATION_SYSTEM_

#include "Foundation/Memory.hpp"
#include "Foundation/RTTI.hpp"
#include "Foundation/Types.hpp"
#include "Simulation/Event.hpp"

#include <string>
#include <vector>

namespace crimild {

    class System : public RTTI {
    public:
        virtual ~System( void ) = default;

        void attachSystem( std::unique_ptr< System > &&other ) noexcept;

        template< class SystemType, typename... Args >
        SystemType *attachSystem( Args &&... args ) noexcept
        {
            m_systems.push_back( std::make_unique< SystemType >( std::forward< Args >( args )... ) );
            return static_cast< SystemType * >( m_systems.back().get() );
        }

        Event dispatch( const Event &e ) noexcept;

        virtual Event handle( const Event & ) noexcept;

    private:
        std::vector< std::unique_ptr< System > > m_systems;

    public:
        /**
           \brief Executed when the system is attached to the simulation
         */
        [[deprecated]] virtual void onAttach( void ) noexcept { }

        /**
           \brief Executed when the system is detached from the simulation
         */
        [[deprecated]] virtual void onDetach( void ) noexcept { }

        /**
           \brief Executed after simulation has started but before starting individual systems
         */
        [[deprecated]] virtual void onInit( void ) noexcept { }

        /**
           \brief Starts this system

           Keep in mind that this method might be called before any scene has been loaded.
         */
        [[deprecated]] virtual void start( void ) noexcept { }

        /**
           \brief Executed after all systems have been started

           This methods is called after the initial scene has been loaded
         */
        [[deprecated]] virtual void lateStart( void ) noexcept { }

        /**
           \brief Executed once per frame before all systems are updated
         */
        [[deprecated]] virtual void earlyUpdate( void ) noexcept { }

        /**
           \brief Executed at least once per frame before updating all systems
         */
        [[deprecated]] virtual void fixedUpdate( void ) noexcept { }

        /**
           \brief Updates this system once per frame
        */
        [[deprecated]] virtual void update( void ) noexcept { }

        /**
           \brief Executed after all systems have been updated
         */
        [[deprecated]] virtual void lateUpdate( void ) noexcept { }

        /**
           \brief Executed before rendering systems
         */
        [[deprecated]] virtual void onPreRender( void ) noexcept { }

        /**
           \brief Executed during rendering
         */
        [[deprecated]] virtual void onRender( void ) noexcept { }

        /**
           \brief Executed after rendering
         */
        [[deprecated]] virtual void onPostRender( void ) noexcept { }

        /**
           \brief Executed when simulation is paused
         */
        [[deprecated]] virtual void onPause( void ) noexcept { }

        /**
           \brief Executed when simulation is resumed after being paused
         */
        [[deprecated]] virtual void onResume( void ) noexcept { }

        /**
           \brief Executed when the simulation is about to stop

           This function is called before all systems are stopped, giving you the opportunity to
           do some extra process. For example, the capture system might take a screenshot at this
           point, since the rendering system is still valid.
         */
        [[deprecated]] virtual void onBeforeStop( void ) noexcept { }

        /**
           \brief Stops this system
         */
        [[deprecated]] virtual void stop( void ) noexcept { }

        /**
           \brief Executed after all systems have been stopped and the simulation is about to quit
         */
        [[deprecated]] virtual void onTerminate( void ) noexcept { }
    };

}

#endif
