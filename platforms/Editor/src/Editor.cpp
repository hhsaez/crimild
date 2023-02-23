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

#include "Editor.hpp"

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

    setScene( createDefaultScene() );
}

Editor::~Editor( void ) noexcept
{
    m_state = nullptr;

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
