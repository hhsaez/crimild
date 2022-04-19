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
#include "Behaviors/withBehavior.hpp"
#include "Coding/FileDecoder.hpp"
#include "Coding/FileEncoder.hpp"
#include "Coding/JSONDecoder.hpp"
#include "Coding/JSONEncoder.hpp"
#include "Components/FreeLookCameraComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Concurrency/Async.hpp"
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
#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/ShaderProgram.hpp"
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
                material->setProgram(
                    std::make_shared< ShaderProgram >(
                        Array< SharedPointer< Shader > > {
                            std::make_shared< Shader >(
                                Shader::Stage::FRAGMENT,
                                R"(
                                    vec3 gridLines( float size, float lineWidth, vec3 P, vec3 N )
                                    {
                                        vec3 G = mod( P, size / 100.0 );
                                        G = G * G;
                                        vec3 s = vec3(
                                            step( G.x, lineWidth / 100.0 ),
                                            step( G.y, lineWidth / 100.0 ),
                                            step( G.z, lineWidth / 100.0 )
                                        );

                                        vec3 grid = vec3(
                                            dot(
                                                vec3(
                                                    mix(
                                                        mix(
                                                            s.r + s.b,
                                                            s.g + s.b,
                                                            abs( N.x )
                                                        ),
                                                        s.r + s.g,
                                                        abs( N.z )
                                                    )
                                                ),
                                                vec3( 1.0 )
                                            )
                                        );

                                        return grid;
                                    }

                                    vec3 checker( float size, vec3 P, vec3 N )
                                    {
                                        float PTC = 0.0;
                                        float contrast = 0.5;
                                        vec3 wallColor = vec3( 0.5, 0.5, 0.6 );
                                        vec3 floorColor = vec3( 0.5, 0.5, 0.6 );

                                        vec3 A0 = P * vec3( -1.0 );
                                        float A1 = size / 100.0;

                                        vec3 B0 = mod( A0, vec3( A1 ) );
                                        vec3 B1 = mod( P, vec3( A1 ) );
                                        vec3 B2 = vec3( A1 / 2.0 );
                                        vec3 B3 = P * vec3( 100.0, 100.0, 100.0 );

                                        vec3 C0 = B0 - B2;
                                        vec3 C1 = B1 - B2;

                                        vec3 D0 = C0 * vec3( -1 );
                                        vec3 D1 = C1;
                                        vec3 D2 = clamp( B3, vec3( 0 ), vec3( 1.0 ) );

                                        vec3 E0 = mix( D0, D1, D2 );

                                        float F0 = E0.x;
                                        float F1 = E0.y;
                                        float F2 = E0.z;

                                        float F3 = 0 - PTC;
                                        float F4 = PTC + 1;
                                        float F5 = abs( N.z );
                                        float F6 = abs( N.y );

                                        float G0 = F2 * F1;
                                        float G1 = F0 * F1;
                                        float G2 = F2 * F0;

                                        float G3 = mix( F3, F4, F5 );
                                        float G4 = mix( F3, F4, F6 );

                                        float H0 = step( G0, 0 );
                                        float H1 = step( G1, 0 );
                                        float H2 = step( G2, 0 );

                                        float H3 = clamp( G3, 0, 1 );
                                        float H4 = clamp( G4, 0, 1 );

                                        float I0 = mix( H0, H1, H3 );

                                        float J0 = mix( I0, H2, H4 );

                                        float K0 = mix( 1.0, J0, contrast );
                                        vec3 K1 = mix( wallColor, floorColor, H4 );

                                        return K0 * K1;
                                    }

                                    void frag_main( inout Fragment frag )
                                    {
                                        float size = 100.0;
                                        float lineWidth = 0.01;

                                        vec3 P = frag.position;
                                        vec3 N = frag.normal;

                                        // frag.albedo = gridLines( size, lineWidth, P, N ) + checker( size, P, N ) + 0.25 * gridLines( 0.5 * size, 0.5 * lineWidth, P, N );
                                        frag.albedo = gridLines( size, lineWidth, P, N ) + 0.25 * gridLines( 0.5 * size, 0.5 * lineWidth, P, N );
                                    }
                                )" ),
                        } ) );
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
            return behaviors::withBehavior(
                box,
                behaviors::actions::rotate(
                    normalize( Vector3::Constants::UNIT_Y ),
                    0.1f ) );
            ;
        }() );

    scene->attachNode(
        [ & ] {
            auto plane = geometry( crimild::alloc< QuadPrimitive >(), ColorRGB { 0.75f, 0.75f, 0.75f } );
            plane->setLocal( rotationX( -numbers::PI_DIV_2 ) * scale( 10.0f ) );
            return plane;
        }() );

    scene->attachNode(
        [ & ] {
            auto light = crimild::alloc< Light >( Light::Type::DIRECTIONAL );
            light->setEnergy( 5 );
            light->setLocal(
                lookAt(
                    Point3 { 0, 10, 10 },
                    Point3 { 0, 0, 0 },
                    Vector3 { 0, 1, 0 } ) );
            light->setCastShadows( true );
            return light;
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

bool crimild::editor::loadNewScene( void )
{
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
    auto scene = crimild::cast_ptr< Group >( Simulation::getInstance()->getScene() );
    coding::JSONEncoder encoder;
    encoder.encode( crimild::retain( scene ) );
    encoder.write( fileName );

    return true;
}

bool crimild::editor::loadScene( std::string fileName )
{
    coding::JSONDecoder decoder;
    CRIMILD_LOG_DEBUG( "Before decode" );
    if ( !decoder.fromFile( fileName ) ) {
        return false;
    }

    CRIMILD_LOG_DEBUG( "Adfter decode" );

    if ( decoder.getObjectCount() == 0 ) {
        return false;
    }

    CRIMILD_LOG_DEBUG( "Loaded" );

    auto scene = decoder.getObjectAt< Group >( 0 );
    Simulation::getInstance()->setScene( scene );

    CRIMILD_LOG_DEBUG( "Added" );
    return true;
}

bool crimild::editor::exportScene( std::string fileName )
{
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
