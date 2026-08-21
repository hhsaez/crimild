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

#include <crimild/core/Simulation/Simulation.hpp>

#include <crimild/core/Concurrency/Async.hpp>
#include <crimild/core/Simulation/FileSystem.hpp>
#include <crimild/core/Messaging/MessageQueue.hpp>
#include <crimild/core/SceneGraph/Camera.hpp>
#include <crimild/core/Simulation/Console/ConsoleCommand.hpp>
#include <crimild/core/Simulation/Event.hpp>
#include <crimild/core/Simulation/Systems/ConsoleSystem.hpp>
#include <crimild/core/Simulation/Systems/DebugSystem.hpp>
#include <crimild/core/Simulation/Systems/RenderSystem.hpp>
#include <crimild/core/Simulation/Systems/StreamingSystem.hpp>
#include <crimild/core/Simulation/Systems/UISystem.hpp>
#include <crimild/core/Simulation/Systems/UpdateSystem.hpp>
#include <crimild/core/Visitors/FetchCameras.hpp>
#include <crimild/core/Visitors/Picking.hpp>
#include <crimild/core/Visitors/StartComponents.hpp>
#include <crimild/core/Visitors/UpdateComponents.hpp>
#include <crimild/core/Visitors/UpdateRenderState.hpp>
#include <crimild/core/Visitors/UpdateWorldState.hpp>

#include <crimild/foundation.hpp>

using namespace crimild;

#ifndef CRIMILD_SIMULATION_FORCE_SLEEP_ON_UPDATE
   #define CRIMILD_SIMULATION_FORCE_SLEEP_ON_UPDATE 1
#endif

void Simulation::start( void ) noexcept
{
   CRIMILD_LOG_INFO( "Initializing simulation ", getName() );

   Version version;
   Log::info( CRIMILD_CURRENT_CLASS_NAME, version.getDescription() );

   const auto ret = handle( Event { .type = Event::Type::SIMULATION_START } );
   if ( ret.type == Event::Type::TERMINATE ) {
      return;
   }

   m_running = true;
}

bool Simulation::step( void ) noexcept
{
   if ( !m_running ) {
      return true;
   }

#if CRIMILD_SIMULATION_FORCE_SLEEP_ON_UPDATE
   constexpr auto MIN_FRAME_TIME = std::chrono::milliseconds( 16 );
   using clock = std::chrono::high_resolution_clock;
   auto frameStartTime = clock::now();
#endif

   auto scene = getScene();

   _simulationClock.tick();

   if ( handle( Event { .type = Event::Type::SIMULATION_UPDATE } ).type == Event::Type::TERMINATE ) {
      return false;
   }

   if ( handle( Event { .type = Event::Type::SIMULATION_RENDER } ).type == Event::Type::TERMINATE ) {
      return false;
   }

   if ( scene != nullptr ) {
      scene->perform( UpdateComponents( _simulationClock ) );
      scene->perform( UpdateWorldState() );
   }

#if CRIMILD_SIMULATION_FORCE_SLEEP_ON_UPDATE
   auto frameEndTime = clock::now();
   auto delta = frameEndTime - frameStartTime;
   auto t = std::max(
      Int64( 1 ),
      Int64( ( MIN_FRAME_TIME - std::chrono::duration_cast< std::chrono::nanoseconds >( delta ) ).count() )
   );
   std::this_thread::sleep_for( std::chrono::nanoseconds( t ) );
#endif

   return true;
}

void Simulation::stop( void ) noexcept
{
   setScene( nullptr );

   _assetManager.clear( true );

#ifdef CRIMILD_PLATFORM_EMSCRIPTEN
   emscripten_cancel_main_loop();
#endif

   handle( Event { .type = Event::Type::SIMULATION_STOP } );

   m_running = false;
}

Event Simulation::handle( const Event &e ) noexcept
{
   _input.handle( e );

   switch ( e.type ) {
      case Event::Type::WINDOW_RESIZE: {
         // Real aspect = e.extent.width / e.extent.height;
         // if ( getMainCamera() != nullptr ) {
         //     getMainCamera()->setAspectRatio( aspect );
         // }
         break;
      }

      case Event::Type::TICK: {
         if ( !step() ) {
            return Event { .type = Event::Type::TERMINATE };
         }
         break;
      }

      case Event::Type::MOUSE_CLICK: {
         // if ( e.button.button == CRIMILD_INPUT_MOUSE_BUTTON_LEFT ) {
         //     if ( auto scene = getScene() ) {
         //         if ( auto camera = Camera::getMainCamera() ) {
         //             auto x = e.button.npos.x;
         //             auto y = e.button.npos.y;
         //             Ray3 R;
         //             if ( camera->getPickRay( x, y, R ) ) {
         //                 Picking::Results res;
         //                 scene->perform( Picking( R, res, []( auto node ) { return node->getClassName() == Geometry::__CLASS_NAME; } ) );
         //                 if ( res.hasResults() ) {
         //                     auto node = res.getBestCandidate();
         //                     return Event {
         //                         .type = Event::Type::NODE_SELECTED,
         //                         .node = node,
         //                     };
         //                 }
         //             }
         //         }
         //     }
         //     return Event {
         //         .type = Event::Type::NODE_SELECTED,
         //         .node = nullptr,
         //     };
         // }
         break;
      }

      default:
         break;
   }

   return e;
}

void Simulation::setScene( SharedPointer< Node > const &scene )
{
   // TODO: Ensure that we always have a valid scene
   // If input scene is null, create a NullNode
   // This way we avoid a lot of checks of wheter the scene is valid or not
   // (same for main camera?)
   _scene = scene;

   m_cameras.clear();
   m_mainCamera = nullptr;

   if ( _scene != nullptr ) {
      // fetch all cameras from the scene
      FetchCameras fetchCameras;
      _scene->perform( fetchCameras );
      fetchCameras.forEachCamera( [ & ]( Camera *camera ) {
         if ( m_mainCamera == nullptr || camera->isMainCamera() ) {
            m_mainCamera = camera;
         }
         m_cameras.push_back( camera );
      } );

      _scene->perform( UpdateWorldState() );
      _scene->perform( StartComponents() );
   }
}

void Simulation::forEachCamera( std::function< void( Camera * ) > callback )
{
   for ( auto camera : m_cameras ) {
      callback( camera );
   }
}
