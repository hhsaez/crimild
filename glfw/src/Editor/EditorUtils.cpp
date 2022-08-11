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

#include "Editor/EditorUtils.hpp"

#include "Behaviors/Actions/Rotate.hpp"
#include "Behaviors/Decorators/Repeat.hpp"
#include "Behaviors/withBehavior.hpp"
#include "Coding/FileDecoder.hpp"
#include "Coding/FileEncoder.hpp"
#include "Coding/JSONDecoder.hpp"
#include "Coding/JSONEncoder.hpp"
#include "Components/MaterialComponent.hpp"
#include "Concurrency/Async.hpp"
#include "Editor/EditorLayer.hpp"
#include "Importers/SceneImporter.hpp"
#include "Loaders/OBJLoader.hpp"
#include "Mathematics/Transformation_euler.hpp"
#include "Mathematics/Transformation_lookAt.hpp"
#include "Mathematics/Transformation_operators.hpp"
#include "Mathematics/Transformation_rotation.hpp"
#include "Mathematics/Transformation_scale.hpp"
#include "Mathematics/Transformation_translation.hpp"
#include "Primitives/BoxPrimitive.hpp"
#include "Primitives/QuadPrimitive.hpp"
#include "Rendering/Image.hpp"
#include "Rendering/ImageView.hpp"
#include "Rendering/Materials/WorldGridMaterial.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/Texture.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Group.hpp"
#include "SceneGraph/Skybox.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/StartComponents.hpp"
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild;

SharedPointer< Node > crimild::editor::createDefaultScene( void ) noexcept
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
                    Point3 { 0, 10, 10 },
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

bool crimild::editor::loadNewScene( void )
{
    // Stop simulation to ensure we're handling the right scene
    EditorLayer::getInstance()->setSimulationState( SimulationState::STOPPED );

    auto scene = createDefaultScene();
    Simulation::getInstance()->setScene( scene );
}

bool crimild::editor::addToScene( SharedPointer< Node > const &node ) noexcept
{
    // TODO(hernan): I'm assuming the root node of a scene is a group, which might not
    // always be the case. Maybe I should check the class type
    auto scene = crimild::cast_ptr< Group >( Simulation::getInstance()->getScene() );
    if ( scene == nullptr ) {
        Simulation::getInstance()->setScene( node );
    } else {
        scene->attachNode( node );
        node->perform( UpdateWorldState() );
        node->perform( StartComponents() );
    }
    return true;
}

bool crimild::editor::saveSceneAs( std::string fileName )
{
    // Stop simulation to ensure we're handling the right scene
    EditorLayer::getInstance()->setSimulationState( SimulationState::STOPPED );

    auto path = FilePath {
        .path = fileName,
        .pathType = FilePath::PathType::ABSOLUTE,
        .fileType = FilePath::FileType::DOCUMENT,
    };

    auto scene = crimild::cast_ptr< Group >( Simulation::getInstance()->getScene() );
    if ( path.getExtension() == "crimild" ) {
        coding::FileEncoder encoder;
        encoder.encode( crimild::retain( scene ) );
        encoder.write( fileName );
    } else {
        coding::JSONEncoder encoder;
        encoder.encode( crimild::retain( scene ) );
        encoder.write( fileName );
    }

    return true;
}

bool crimild::editor::loadScene( std::string fileName )
{
    // Stop simulation to ensure we're handling the right scene
    EditorLayer::getInstance()->setSimulationState( SimulationState::STOPPED );

    auto path = FilePath {
        .path = fileName,
        .pathType = FilePath::PathType::ABSOLUTE,
        .fileType = FilePath::FileType::DOCUMENT,
    };

    if ( path.getExtension() == "crimild" ) {
        coding::FileDecoder decoder;
        decoder.read( fileName );
        if ( decoder.getObjectCount() == 0 ) {
            CRIMILD_LOG_ERROR( "Cannot read file ", fileName );
            return false;
        }
        auto scene = decoder.getObjectAt< Node >( 0 );
        Simulation::getInstance()->setScene( scene );
    } else {
        coding::JSONDecoder decoder;
        if ( !decoder.fromFile( fileName ) ) {
            return false;
        }

        if ( decoder.getObjectCount() == 0 ) {
            return false;
        }

        auto scene = decoder.getObjectAt< Group >( 0 );
        Simulation::getInstance()->setScene( scene );
    }

    return true;
}

bool crimild::editor::exportScene( std::string fileName )
{
    // Stop simulation to ensure we're handling the right scene
    EditorLayer::getInstance()->setSimulationState( SimulationState::STOPPED );

    auto scene = crimild::cast_ptr< Group >( Simulation::getInstance()->getScene() );
    coding::FileEncoder encoder;
    encoder.encode( crimild::retain( scene ) );
    encoder.write( fileName );
    return true;
}

bool crimild::editor::importFile( std::string fileName ) noexcept
{
    auto path = FilePath {
        .path = fileName,
        .pathType = FilePath::PathType::ABSOLUTE,
        .fileType = FilePath::FileType::DOCUMENT,
    };

    SharedPointer< Node > model;
    if ( path.getExtension() == "crimild" ) {
        coding::FileDecoder decoder;
        decoder.read( fileName );
        if ( decoder.getObjectCount() == 0 ) {
            CRIMILD_LOG_ERROR( "Cannot read file ", fileName );
            return false;
        }
        model = decoder.getObjectAt< Node >( 0 );
    } else if ( path.getExtension() == "obj" ) {
        OBJLoader loader( path.getAbsolutePath() );
        loader.setVerbose( false );
        model = loader.load();
    } else {
#ifdef CRIMILD_ENABLE_IMPORT
        import::SceneImporter importer;
        model = importer.import( path.getAbsolutePath() );
#else
        CRIMILD_LOG_ERROR( "Unsupported file type" );
        return false;
#endif
    }

    if ( model == nullptr ) {
        CRIMILD_LOG_ERROR( "Cannot import file ", fileName );
        return false;
    }

    return addToScene( model );
}

bool crimild::editor::cloneSelected( void ) noexcept
{
    auto editor = EditorLayer::getInstance();
    if ( editor == nullptr ) {
        return false;
    }

    auto selected = editor->getSelectedNode();
    if ( selected == nullptr ) {
        return false;
    }

    auto copy = selected->perform< ShallowCopy >();
    if ( auto parent = dynamic_cast< Group * >( selected->getParent() ) ) {
        parent->attachNode( copy );
    } else {
        return false;
    }

    editor->setSelectedNode( get_ptr( copy ) );

    return true;
}

bool crimild::editor::deleteSelected( void ) noexcept
{
    auto editor = EditorLayer::getInstance();
    if ( editor == nullptr ) {
        return false;
    }

    auto selected = editor->getSelectedNode();
    if ( selected == nullptr ) {
        return false;
    }

    selected->detachFromParent();
    editor->setSelectedNode( nullptr );

    return true;
}
