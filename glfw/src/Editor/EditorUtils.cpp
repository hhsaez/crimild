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

#include "Components/FreeLookCameraComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Importers/SceneImporter.hpp"
#include "Loaders/OBJLoader.hpp"
#include "Mathematics/Transformation_lookAt.hpp"
#include "Mathematics/Transformation_operators.hpp"
#include "Mathematics/Transformation_rotation.hpp"
#include "Mathematics/Transformation_scale.hpp"
#include "Mathematics/Transformation_translation.hpp"
#include "Primitives/BoxPrimitive.hpp"
#include "Primitives/QuadPrimitive.hpp"
#include "Rendering/Image.hpp"
#include "Rendering/ImageView.hpp"
#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/Texture.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Group.hpp"
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
        geometry->attachComponent< MaterialComponent >(
            [ & ] {
                auto material = crimild::alloc< materials::PrincipledBSDF >();
                material->setAlbedo( albedo );
                material->setAlbedoMap(
                    [] {
                        auto texture = crimild::alloc< Texture >();
                        texture->imageView = [ & ] {
                            auto imageView = crimild::alloc< ImageView >();
                            imageView->image = Image::CHECKERBOARD_16;
                            return imageView;
                        }();
                        texture->sampler = [ & ] {
                            auto sampler = crimild::alloc< Sampler >();
                            sampler->setMinFilter( Sampler::Filter::NEAREST );
                            sampler->setMagFilter( Sampler::Filter::NEAREST );
                            return sampler;
                        }();
                        return texture;
                    }() );
                return material;
            }() );
        return geometry;
    };

    scene->attachNode(
        [ & ] {
            auto box = geometry( crimild::alloc< BoxPrimitive >(), ColorRGB { 0.5, 0.3, 0.2 } );
            box->setLocal( translation( 0, 1, 0 ) );
            return box;
        }() );

    scene->attachNode(
        [ & ] {
            auto plane = geometry( crimild::alloc< QuadPrimitive >(), ColorRGB { 0.75f, 0.75f, 0.75f } );
            plane->setLocal( rotationX( -numbers::PI_DIV_2 ) * scale( 10.0f ) );
            return plane;
        }() );

    scene->attachNode( [] {
        auto camera = crimild::alloc< Camera >();
        camera->setLocal(
            lookAt(
                Point3 { 10, 10, 10 },
                Point3 { 0, 0, 0 },
                Vector3::Constants::UP ) );
        camera->attachComponent< FreeLookCameraComponent >();
        return camera;
    }() );

    scene->perform( StartComponents() );
    scene->perform( UpdateWorldState() );

    return scene;
}

bool crimild::editor::addToScene( SharedPointer< Node > const &node ) noexcept
{
    // TODO(hernan): I'm assuming the root node of a scene is a group, which might not
    // always be the case. Maybe I should check the class type
    auto scene = crimild::cast_ptr< Group >( Simulation::getInstance()->getScene() );
    node->perform( UpdateWorldState() );
    node->perform( StartComponents() );
    scene->attachNode( node );
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
        // TODO
        CRIMILD_LOG_ERROR( "Crimild binary file not supported" );
        return false;
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

    // OBJLoader loader( fileName ); //path.getAbsolutePath() );
    // loader.setVerbose( true );
    // auto model = loader.load();
    if ( model == nullptr ) {
        CRIMILD_LOG_ERROR( "Cannot import file ", fileName );
        return false;
    }

    return addToScene( model );
}
