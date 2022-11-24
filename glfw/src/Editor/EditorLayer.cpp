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
#include "Editor/Panels/BehaviorEditorPanel.hpp"
#include "Editor/Panels/ToolbarPanel.hpp"
#include "Foundation/Log.hpp"
#include "Simulation/Settings.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/StartComponents.hpp"
#include "Visitors/UpdateWorldState.hpp"

#include <array>
#include <imgui.h>

using namespace crimild;

EditorLayer::EditorLayer( vulkan::RenderDevice *renderDevice ) noexcept
    : m_renderDevice( renderDevice ),
      m_state( crimild::alloc< EditorState >() )
{
    CRIMILD_LOG_TRACE();

    m_layout = [ & ] {
        using namespace crimild::editor;
        using namespace crimild::editor::layout;
        
        auto split = crimild::alloc< Layout >( Layout::Direction::UP, Layout::Pixels( 40 ) );
        split->setFirst( crimild::alloc< ToolbarPanel >() );
        split->setSecond(
            [ & ] {
                auto split = crimild::alloc< Layout >( Layout::Direction::LEFT, Layout::Pixels( 300 ) );
                split->setFirst( crimild::alloc< SceneHierarchyPanel >() );
                split->setSecond(
                    [ & ] {
                        auto split = crimild::alloc< Layout >( Layout::Direction::RIGHT, Layout::Pixels( 300 ) );
                        split->setFirst( crimild::alloc< NodeInspectorPanel >( renderDevice ) );
                        split->setSecond(
                            [ & ] {
                                auto split = crimild::alloc< Layout >( Layout::Direction::UP, Layout::Fraction( 0.5 ) );
                                split->setFirst( crimild::alloc< ScenePanel >( renderDevice ) );
                                split->setSecond(
                                    [&] {
                                        auto tab = crimild::alloc< layout::Tab >();
                                        tab->setFirst( crimild::alloc< editor::SimulationPanel >( renderDevice ) );
                                        tab->setSecond( crimild::alloc< editor::BehaviorEditorPanel >() );
                                        return tab;
                                    }()
                                );
                                return split;
                            }()
                        );
                        return split;
                    }()
                );
                return split;
            }()
        );
        return split;
    }();

    m_dockspace = [ & ] {
        auto dock = crimild::alloc< editor::layout::Dockspace >();
        dock->setFirst( m_layout );
        return dock;
    }();
}

EditorLayer::~EditorLayer( void ) noexcept
{
    m_state = nullptr;
    m_previousState = nullptr;
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

    return m_dockspace->handle( e );
}

void EditorLayer::render( void ) noexcept
{
    editor::mainMenu( this );
    m_dockspace->render();
}

void EditorLayer::setSimulationState( SimulationState newState ) noexcept
{
    if ( m_simulationState == newState ) {
        return;
    }

    if ( m_simulationState == SimulationState::STOPPED && newState == SimulationState::PLAYING ) {
        m_edittableScene = crimild::retain( Simulation::getInstance()->getScene() );
        m_previousState = m_state;

        // Using encoders to clone the scene is overkill since it copies buffers
        // and textures, which should not change during playback. And this is a
        // slow operation. But it also guarrantees that the scene is a full clone,
        // as well as providing a visual representation of what is actually saved
        // in the file system.
        // TODO: consider using a shallow copy (or implement shallow encoder).
        auto encoder = crimild::alloc< coding::MemoryEncoder >();
        encoder->encode( m_edittableScene );
        encoder->encode( m_state );

        auto bytes = encoder->getBytes();
        auto decoder = crimild::alloc< coding::MemoryDecoder >();
        decoder->fromBytes( bytes );

        auto simScene = decoder->getObjectAt< Node >( 0 );
        m_state = decoder->getObjectAt< EditorState >( 1 );

        Simulation::getInstance()->setScene( simScene );
        handle( Event { .type = Event::Type::SCENE_CHANGED } );
    } else if ( newState == SimulationState::STOPPED ) {
        Simulation::getInstance()->setScene( m_edittableScene );
        m_state = m_previousState;
        m_previousState = nullptr;
        handle( Event { .type = Event::Type::SCENE_CHANGED } );
    }

    if ( auto selected = getSelectedNode() ) {
        std::cout << "Selected " << selected->getName() << std::endl;
    }

    m_simulationState = newState;

    if ( newState == SimulationState::PLAYING ) {
        Simulation::getInstance()->resume();
    } else {
        Simulation::getInstance()->pause();
    }
}
