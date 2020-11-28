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
#include "Foundation/SharedObject.hpp"
#include "Foundation/Types.hpp"
#include "Messaging/MessageQueue.hpp"

#include <string>

namespace crimild {

    class System
        : public SharedObject,
          public RTTI,
          public Messenger {
    protected:
        virtual ~System( void ) = default;

    public:
        /**
           \brief Executed when the system is attached to the simulation
         */
        virtual void onAttach( void ) noexcept { }

        /**
           \brief Executed when the system is detached from the simulation
         */
        virtual void onDetach( void ) noexcept { }

        /**
           \brief Executed after simulation has started but before starting individual systems
         */
        virtual void onInit( void ) noexcept { }

        /**
           \brief Starts this system

           Keep in mind that this method might be called before any scene has been loaded.
         */
        virtual void start( void ) noexcept { }

        /**
           \brief Executed after all systems have been started

           This methods is called after the initial scene has been loaded
         */
        virtual void lateStart( void ) noexcept { }

        /**
           \brief Executed once per frame before all systems are updated
         */
        virtual void earlyUpdate( void ) noexcept { }

        /**
           \brief Executed at least once per frame before updating all systems
         */
        virtual void fixedUpdate( void ) noexcept { }

        /**
           \brief Updates this system once per frame
        */
        virtual void update( void ) noexcept { }

        /**
           \brief Executed after all systems have been updated
         */
        virtual void lateUpdate( void ) noexcept { }

        /**
           \brief Executed before rendering systems
         */
        virtual void onPreRender( void ) noexcept { }

        /**
           \brief Executed during rendering
         */
        virtual void onRender( void ) noexcept { }

        /**
           \brief Executed after rendering
         */
        virtual void onPostRender( void ) noexcept { }

        /**
           \brief Executed when simulation is paused
         */
        virtual void onPause( void ) noexcept { }

        /**
           \brief Executed when simulation is resumed after being paused
         */
        virtual void onResume( void ) noexcept { }

        /**
           \brief Stops this system
         */
        virtual void stop( void ) noexcept { }

        /**
           \brief Executed after all systems have been stopped and the simulation is about to quit
         */
        virtual void onTerminate( void ) noexcept { }
    };

}

#endif
