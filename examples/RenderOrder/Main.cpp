/*
 * Copyright (c) 2013, Hernan Saez
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

#include <Crimild.hpp>
#include <Crimild_SDL.hpp>

using namespace crimild;
using namespace crimild::sdl;

SharedPointer< Node > createBillboard( const Vector3f &position, std::string imageFileName )
{
    auto geometry = crimild::alloc< Geometry >( imageFileName );
    geometry->attachPrimitive( crimild::alloc< QuadPrimitive >( 1.0f, 1.0f, VertexFormat::VF_P3_UV2, Vector2f( 0.0f, 0.0f ) ) );
    geometry->local().setTranslate( position );
    geometry->attachComponent< BillboardComponent >();

    auto material = crimild::alloc< Material >();
    auto texture = crimild::alloc< Texture >( crimild::alloc< ImageTGA >( FileSystem::getInstance().pathForResource( imageFileName ) ) );
    material->setColorMap( texture );
    material->setCullFaceState( crimild::alloc< CullFaceState >( false ) );
    material->getAlphaState()->setEnabled( true );
    geometry->getComponent< MaterialComponent >()->attachMaterial( material );

    return geometry;
}

int main( int argc, char **argv )
{
    auto sim = crimild::alloc< SDLSimulation >( "RenderOrder", crimild::alloc< Settings >( argc, argv ) );

    auto scene = crimild::alloc< Group >();

    auto billboards = crimild::alloc< Group >();
    billboards->attachNode( createBillboard( Vector3f( 1.0f, 0.0f, 0.0f ), "opengl.tga" ) );
    billboards->attachNode( createBillboard( Vector3f( -1.0f, 0.0f, 0.0f ), "cpp.tga" ) );
    billboards->attachComponent< RotationComponent >( Vector3f( 0.0f, 1.0f, 0.0f ), 0.1f );
    scene->attachNode( billboards );

    auto camera = crimild::alloc< Camera >();
    camera->local().setTranslate( Vector3f( 0.0f, 0.0f, 2.0f ) );
    scene->attachNode( camera );
    
    sim->setScene( scene );
	return sim->run();
}

