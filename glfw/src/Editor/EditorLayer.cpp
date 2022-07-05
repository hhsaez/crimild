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

#include "Editor/EditorLayer.hpp"

#include "Coding/MemoryDecoder.hpp"
#include "Coding/MemoryEncoder.hpp"
#include "Editor/EditorUtils.hpp"
#include "Editor/Menus/mainMenu.hpp"
#include "Editor/Panels/NodeInspectorPanel.hpp"
#include "Editor/Panels/SceneHierarchyPanel.hpp"
#include "Editor/Panels/ScenePanel.hpp"
#include "Editor/Panels/SimulationPanel.hpp"
#include "Editor/Panels/ToolbarPanel.hpp"
#include "Foundation/Log.hpp"
#include "Simulation/Settings.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/StartComponents.hpp"
#include "Visitors/UpdateWorldState.hpp"

#include <array>

using namespace crimild;

EditorLayer::EditorLayer( vulkan::RenderDevice *renderDevice ) noexcept
    : m_renderDevice( renderDevice )
{
    CRIMILD_LOG_TRACE();

    attach< editor::ToolbarPanel >();
    attach< editor::ScenePanel >( renderDevice, Point2 { 0, 50 }, Extent2D { .width = 1280, .height = 515 } );
    attach< editor::SimulationPanel >( renderDevice, Point2 { 0, 565 }, Extent2D { .width = 1280, .height = 515 } );
    attach< SceneHierarchyPanel >( Point2 { 1280, 50 }, Extent2D { .width = 320, .height = 1030 } );
    attach< NodeInspectorPanel >( Point2 { 1600, 50 }, Extent2D { .width = 320, .height = 1030 } );
}

EditorLayer::~EditorLayer( void ) noexcept
{
    m_selectedNode = nullptr;
    m_renderDevice = nullptr;
}

Event EditorLayer::handle( const Event &e ) noexcept
{
    switch ( e.type ) {
        case Event::Type::SIMULATION_START: {
            if ( auto sim = Simulation::getInstance() ) {
                if ( sim->getScene() == nullptr ) {
                    if ( auto settings = Settings::getInstance() ) {
                        if ( settings->hasKey( "scene" ) ) {
                            editor::loadScene( settings->get< std::string >( "scene", "" ) );
                        }
                    }
                }
                if ( sim->getScene() == nullptr ) {
                    sim->setScene( editor::createDefaultScene() );
                }

                // Start simulation paused when using editor
                sim->pause();
            }
            break;
        }

        case Event::Type::NODE_SELECTED: {
            setSelectedNode( e.node );
            break;
        }

        default: {
            break;
        }
    }

    if ( e.type != Event::Type::TICK ) {
        // TODO: Handle return event
        Simulation::getInstance()->handle( e );
    }

    return Layer::handle( e );
}

void EditorLayer::render( void ) noexcept
{
    editor::mainMenu( this );

    Layer::render();
}

void EditorLayer::setSimulationState( SimulationState newState ) noexcept
{
    if ( m_simulationState == newState ) {
        return;
    }

    if ( m_simulationState == SimulationState::STOPPED && newState == SimulationState::PLAYING ) {
        m_selectedNode = nullptr;
        m_edittableScene = crimild::retain( Simulation::getInstance()->getScene() );

        // Using encoders to clone the scene is overkill since it copies buffers
        // and textures, which should not change during playback. And this is a
        // slow operation. But it also guarrantees that the scene is a full clone,
        // as well as providing a visual representation of what is actually saved
        // in the file system.
        // TODO: consider using a shallow copy (or implement shallow encoder).
        auto encoder = crimild::alloc< coding::MemoryEncoder >();
        encoder->encode( m_edittableScene );
        auto bytes = encoder->getBytes();
        auto decoder = crimild::alloc< coding::MemoryDecoder >();
        decoder->fromBytes( bytes );
        auto simScene = decoder->getObjectAt< Node >( 0 );
        Simulation::getInstance()->setScene( simScene );
        handle( Event { Event::Type::SCENE_CHANGED } );
    } else if ( newState == SimulationState::STOPPED ) {
        m_selectedNode = nullptr;
        Simulation::getInstance()->setScene( m_edittableScene );
        handle( Event { Event::Type::SCENE_CHANGED } );
    }

    m_simulationState = newState;

    if ( newState == SimulationState::PLAYING ) {
        Simulation::getInstance()->resume();
    } else {
        Simulation::getInstance()->pause();
    }
}
