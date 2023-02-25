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

#include "Simulation/Editor.hpp"

#include "Simulation/Project.hpp"

using namespace crimild;
using namespace crimild::editor;

CRIMILD_CREATE_SIMULATION( crimild::editor::Editor, "Crimild" );

Editor *Editor::s_instance = nullptr;

void Editor::State::encode( coding::Encoder &encoder )
{
    Codable::encode( encoder );

    encoder.encode( "selectedObject", selectedObject );
}

void Editor::State::decode( coding::Decoder &decoder )
{
    Codable::decode( decoder );

    SharedPointer< coding::Codable > selected;
    decoder.decode( "selectedObject", selected );
    selectedObject = get_ptr( selected );
}

Editor::Editor( void ) noexcept
    : m_state( crimild::alloc< State >() )
{
    s_instance = this;

    CRIMILD_LOG_TRACE();

    loadRecentProjects();

    if ( !m_recentProjects.empty() ) {
        auto autoload = Settings::getInstance()->get( "editor.load_last_project", false );
        if ( autoload ) {
            auto path = m_recentProjects.front();
            crimild::concurrency::sync_frame(
                [ path ]() {
                    Editor::getInstance()->loadProject( path );
                }
            );
        }
    }

    // setScene( createDefaultScene() );
}

Editor::~Editor( void ) noexcept
{
    saveProject();

    m_state = nullptr;
    m_project = nullptr;

    s_instance = nullptr;
}

Event Editor::handle( const Event &e ) noexcept
{
    const auto ret = Simulation::handle( e );
    return ret;
}

SharedPointer< Node > Editor::createDefaultScene( void ) noexcept
{
    auto scene = crimild::alloc< Group >();

    auto geometry = []( SharedPointer< Primitive > const &primitive, const ColorRGB &albedo ) {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive( primitive );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::WorldGrid >() );
        return geometry;
    };

    scene->attachNode(
        [ & ] {
            auto box = geometry( crimild::alloc< BoxPrimitive >(), ColorRGB { 0.5, 0.3, 0.2 } );
            box->setLocal( translation( 0, 1, 0 ) );
            return behaviors::withBehavior(
                box,
                [] {
                    auto repeat = crimild::alloc< behaviors::decorators::Repeat >();
                    repeat->setBehavior(
                        behaviors::actions::rotate(
                            normalize( Vector3::Constants::UNIT_Y ),
                            0.1f
                        )
                    );
                    return repeat;
                }()
            );
        }()
    );

    scene->attachNode(
        [ & ] {
            auto plane = geometry( crimild::alloc< QuadPrimitive >(), ColorRGB { 0.75f, 0.75f, 0.75f } );
            plane->setLocal( rotationX( -numbers::PI_DIV_2 ) * scale( 10.0f ) );
            return plane;
        }()
    );

    scene->attachNode(
        [ & ] {
            auto light = crimild::alloc< Light >( Light::Type::DIRECTIONAL );
            light->setEnergy( 5 );
            light->setLocal(
                lookAt(
                    Point3 { -10, 10, 10 },
                    Point3 { 0, 0, 0 },
                    Vector3 { 0, 1, 0 }
                )
            );
            light->setCastShadows( true );
            return light;
        }()
    );

    scene->attachNode( [] {
        auto camera = crimild::alloc< Camera >();
        camera->setLocal(
            lookAt(
                Point3 { 10, 10, 10 },
                Point3 { 0, 0, 0 },
                Vector3::Constants::UP
            )
        );
        return camera;
    }() );

    scene->attachNode(
        [] {
            auto skybox = crimild::alloc< Skybox >( ColorRGB { 0.28, 0.63, 0.72 } );
            return skybox;
        }()
    );

    scene->perform( StartComponents() );
    scene->perform( UpdateWorldState() );

    return scene;
}

void Editor::createProject( const std::filesystem::path &path ) noexcept
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
}

void Editor::loadProject( const std::filesystem::path &path ) noexcept
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
    saveRecentProjects();
}

void Editor::saveProject( void ) noexcept
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

    saveRecentProjects();
}

void Editor::saveRecentProjects( void ) noexcept
{
    m_recentProjects.remove( m_project->getFilePath().string() );
    m_recentProjects.push_front( m_project->getFilePath().string() );

    auto settingsPath = std::filesystem::current_path() / "recent_projects.txt";
    auto fs = std::ofstream( settingsPath, std::ios::out );
    if ( !fs.is_open() ) {
        CRIMILD_LOG_WARNING( "Cannot open recent projects file: ", settingsPath );
        return;
    }
    for ( const auto &path : m_recentProjects ) {
        if ( !path.empty() ) {
            fs << path << "\n";
        }
    }
    fs.close();
}

void Editor::loadRecentProjects( void ) noexcept
{
    m_recentProjects.clear();
    auto settingsPath = std::filesystem::current_path() / "recent_projects.txt";
    auto fs = std::ifstream( settingsPath, std::ios::in );
    if ( !fs.is_open() ) {
        return;
    }
    while ( !fs.eof() ) {
        char buff[ 1024 ];
        fs.getline( buff, 1024 );
        auto path = std::string( buff );
        if ( !path.empty() ) {
            m_recentProjects.push_back( std::string( buff ) );
        }
    }
    fs.close();
}

void Editor::createNewScene( const std::filesystem::path &path ) noexcept
{
    if ( auto sim = Simulation::getInstance() ) {
        sim->setScene( nullptr );
        handle( Event { .type = Event::Type::SCENE_CHANGED } );

        sim->setScene( createDefaultScene() );
        handle( Event { .type = Event::Type::SCENE_CHANGED } );
    }

    saveSceneAs( path );
}

void Editor::loadScene( const std::filesystem::path &path ) noexcept
{
    if ( !std::filesystem::exists( path ) ) {
        CRIMILD_LOG_ERROR( path, " does not exists" );
        return;
    }

    // Stop simulation to ensure we're handling the right scene
    // EditorLayer::getInstance()->setSimulationState( SimulationState::STOPPED );

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

void Editor::saveScene( void ) noexcept
{
    if ( auto project = getProject() ) {
        auto path = project->getScenePath( project->getCurrentSceneName() );
        saveSceneAs( path );
    }
}

void Editor::saveSceneAs( const std::filesystem::path &path ) noexcept
{
    // Always save the editable scene, not the simulated one
    auto scene = [ & ] {
        if ( m_edittableScene != nullptr ) {
            return m_edittableScene;
        } else {
            return crimild::retain( Simulation::getInstance()->getScene() );
        }
    }();
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