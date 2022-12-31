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

#include "Coding/FileDecoder.hpp"
#include "Coding/FileEncoder.hpp"
#include "Editor/EditorProject.hpp"
#include "Editor/EditorUtils.hpp"
#include "Editor/Menus/mainMenu.hpp"
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
      m_state( crimild::alloc< EditorState >() ),
      m_layoutManager( crimild::alloc< editor::layout::LayoutManager >() )
{
    CRIMILD_LOG_TRACE();
}

EditorLayer::~EditorLayer( void ) noexcept
{
    saveScene();
    saveProject();

    m_layoutManager = nullptr;

    m_state = nullptr;
    m_previousState = nullptr;
    m_renderDevice = nullptr;
}

Event EditorLayer::handle( const Event &e ) noexcept
{
    switch ( e.type ) {
        case Event::Type::SIMULATION_START: {
            if ( auto sim = Simulation::getInstance() ) {
                // Start simulation paused when using editor
                sim->pause();
            }
            break;
        }

        case Event::Type::NODE_SELECTED: {
            setSelectedNode( e.node );
            break;
        }

        case Event::Type::WINDOW_RESIZE: {
            m_lastResizeEvent = e;
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

    return m_layoutManager->handle( e );
}

void EditorLayer::render( void ) noexcept
{
    editor::mainMenu( this );
    m_layoutManager->render();
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

void EditorLayer::createProject( const std::filesystem::path &path ) noexcept
{
    const auto projectName = path.stem().stem(); // Remove ".project.crimild"
    const auto projectVersion = Version { 1, 0, 0 };

    std::cout << "Creating project: "
              << "\n\tName: " << projectName
              << "\n\tVersion: " << projectVersion.getDescription()
              << "\n\tPath: " << path.parent_path()
              << "\n\tAbsolute Path: " << std::filesystem::absolute( path )
              << "\n\tWork Dir: " << std::filesystem::current_path()
              << "\n\tDONE"
              << std::endl;

    const auto projectRoot = path.parent_path() / projectName;
    if ( std::filesystem::exists( projectRoot ) ) {
        CRIMILD_LOG_ERROR( "Project root direcotory", projectRoot, " already exists" );
        return;
    }

    std::filesystem::create_directories( projectRoot / "Assets" / "Scenes" );
    std::filesystem::create_directories( projectRoot / "Assets" / "Models" );
    std::filesystem::create_directories( projectRoot / "Assets" / "Audio" );
    std::filesystem::create_directories( projectRoot / "Assets" / "Textures" );
    std::filesystem::create_directories( projectRoot / "Assets" / "Prefabs" );

    m_project = crimild::alloc< editor::Project >( projectName.string(), projectVersion );
    m_project->setFilePath( projectRoot / "project.crimild" );
    CRIMILD_LOG_INFO( "Created project: ", m_project->getName(), " ", m_project->getVersion().getDescription() );

    saveProject();
    createNewScene( projectRoot / "Assets" / "Scenes" / "main.crimild" );
    loadDefaultLayout();
}

void EditorLayer::loadProject( const std::filesystem::path &path ) noexcept
{
    if ( !std::filesystem::exists( path ) ) {
        CRIMILD_LOG_ERROR( path, " does not exists" );
        return;
    }

    const auto projectRoot = std::filesystem::is_directory( path ) ? path : path.parent_path();
    const auto projectFilePath = projectRoot / "project.crimild";
    if ( !std::filesystem::exists( projectFilePath ) ) {
        CRIMILD_LOG_ERROR( path, " is not a valid Crimild project directory" );
        return;
    }

    coding::FileDecoder decoder;
    decoder.read( projectFilePath );
    if ( decoder.getObjectCount() == 0 ) {
        CRIMILD_LOG_ERROR( "Cannot decode project from path ", path );
        return;
    }
    m_project = decoder.getObjectAt< editor::Project >( 0 );
    if ( m_project == nullptr ) {
        CRIMILD_LOG_ERROR( "Cannot load project from path ", path );
        return;
    }

    m_project->setFilePath( projectFilePath );

    CRIMILD_LOG_INFO( "Loaded project: ", m_project->getName(), " ", m_project->getVersion().getDescription() );

    loadScene( m_project->getScenePath( m_project->getCurrentSceneName() ) );
    loadDefaultLayout();
}

void EditorLayer::saveProject( void ) noexcept
{
    if ( m_project == nullptr ) {
        return;
    }

    coding::FileEncoder encoder;
    encoder.encode( m_project );
    if ( !encoder.write( m_project->getFilePath() ) ) {
        CRIMILD_LOG_ERROR( "Failed to encode project" );
        return;
    }

    CRIMILD_LOG_INFO( "Saved project: ", m_project->getName(), " ", m_project->getVersion().getDescription() );
}

void EditorLayer::loadDefaultLayout( void ) noexcept
{
    // TODO: FixMe - Loading a layout at this point seems to be breakig the app.
    // m_layoutManager->loadDefaultLayout();
}

void EditorLayer::createNewScene( const std::filesystem::path &path ) noexcept
{
    auto scene = editor::createDefaultScene();
    if ( auto sim = Simulation::getInstance() ) {
        sim->setScene( nullptr );
        handle( Event { .type = Event::Type::SCENE_CHANGED } );

        sim->setScene( editor::createDefaultScene() );
        handle( Event { .type = Event::Type::SCENE_CHANGED } );
    }

    saveSceneAs( path );
}

void EditorLayer::loadScene( const std::filesystem::path &path ) noexcept
{
    if ( !std::filesystem::exists( path ) ) {
        CRIMILD_LOG_ERROR( path, " does not exists" );
        return;
    }

    // Stop simulation to ensure we're handling the right scene
    EditorLayer::getInstance()->setSimulationState( SimulationState::STOPPED );

    {
        auto prevScene = retain( Simulation::getInstance()->getScene() );
        Simulation::getInstance()->setScene( nullptr );
        handle( Event { .type = Event::Type::SCENE_CHANGED } );
    }

    coding::FileDecoder decoder;
    decoder.read( path );
    if ( decoder.getObjectCount() == 0 ) {
        CRIMILD_LOG_ERROR( "Cannot read file ", path );
        return;
    }
    auto scene = decoder.getObjectAt< Node >( 0 );
    Simulation::getInstance()->setScene( scene );
    handle( Event { .type = Event::Type::SCENE_CHANGED } );

    if ( auto project = getProject() ) {
        project->setCurrentSceneName( path.stem().string() );
        saveProject();
    }
}

void EditorLayer::saveScene( void ) noexcept
{
    if ( auto project = getProject() ) {
        auto path = project->getScenePath( project->getCurrentSceneName() );
        saveSceneAs( path );
    }
}

void EditorLayer::saveSceneAs( const std::filesystem::path &path ) noexcept
{
    auto scene = crimild::retain( Simulation::getInstance()->getScene() );
    if ( scene == nullptr ) {
        return;
    }

    coding::FileEncoder encoder;
    encoder.encode( scene );
    encoder.write( path );

    if ( auto project = getProject() ) {
        project->setCurrentSceneName( path.stem().string() );
        saveProject();
    }
}
