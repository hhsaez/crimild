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

#include <fstream>
#include <string>
#include <vector>

using namespace crimild;
using namespace crimild::sdl;

SharedPointer< Node > makeGround( void )
{
	auto primitive = crimild::alloc< QuadPrimitive >( 10.0f, 10.0f, VertexFormat::VF_P3_N3_UV2, Vector2f( 0.0f, 0.0f ), Vector2f( 3.0f, 3.0f ) );
	auto geometry = crimild::alloc< Geometry >();
	geometry->attachPrimitive( primitive );
	geometry->local().setRotate( Vector3f( 1.0f, 0.0f, 0.0f ), -Numericf::HALF_PI );
    
    auto material = crimild::alloc< Material >();
    // material->setDiffuse( RGBAColorf( 0.0f, 0.16f, 0.25f, 1.0f ) );
    material->setSpecular( RGBAColorf( 0.0f, 0.0f, 0.0f, 1.0f ) );
    material->setColorMap( crimild::alloc< Texture >( crimild::alloc< ImageTGA >( FileSystem::getInstance().pathForResource( "assets/grid.tga" ) ) ) );
    material->setEmissiveMap( crimild::alloc< Texture >( crimild::alloc< ImageTGA >( FileSystem::getInstance().pathForResource( "assets/grid.tga" ) ) ) );
    geometry->getComponent< MaterialComponent >()->attachMaterial( material );
	
	return geometry;
}

int main( int argc, char **argv )
{
	auto sim = crimild::alloc< SDLSimulation >( "Lightcycle", crimild::alloc< Settings >( argc, argv ) );

	auto scene = crimild::alloc< Group >();

	OBJLoader loader( FileSystem::getInstance().pathForResource( "assets/HQ_Movie cycle.obj" ) );
	auto model = loader.load();
	if ( model != nullptr ) {
		auto group = crimild::alloc< Group >();
		group->attachNode( model );
		scene->attachNode( group );
	}
    
    scene->attachNode( makeGround() );

	auto light = crimild::alloc< Light >();
	light->local().setTranslate( -20.0f, 10.0f, 10.0f );
    light->local().lookAt( Vector3f( -1.0f, 0.0f, 0.0f ), Vector3f( 0.0f, 1.0f, 0.0f ) );
    //light->setCastShadows( true );
    //light->setShadowNearCoeff( 1.0f );
    //light->setShadowFarCoeff( 50.0f );
	scene->attachNode( light );
    
    auto cameraPivot = crimild::alloc< Group >();
    cameraPivot->local().setTranslate( 0.0f, 2.0f, 0.0f );
    cameraPivot->attachComponent( crimild::alloc< RotationComponent >( Vector3f( 0.0f, 1.0f, 0.0f ), -0.01f ) );

	auto camera = crimild::alloc< Camera >( 45.0f, 4.0f / 3.0f, 1.0f, 100.0f );
	camera->local().setTranslate( 0.0f, 3.0f, 5.0f );
    camera->local().lookAt( Vector3f( 0.0f, 1.0f, 0.0f ) );
	camera->local().setTranslate( 0.0f, 0.0f, 4.0f );
	cameraPivot->attachNode( camera );
    scene->attachNode( cameraPivot );
    
    auto font = crimild::alloc< Font >( FileSystem::getInstance().pathForResource( "assets/LucidaGrande.txt" ) );
	auto text = crimild::alloc< Text >();
	text->setFont( font );
	text->setSize( 0.075f );
	text->setText( "Deferred Render Pass" );
	text->setTextColor( RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0 ) );
	text->local().setTranslate( -0.95f, 0.9f, -1.0f );
    camera->attachNode( text );

    auto forwardPass = crimild::alloc< StandardRenderPass >();
    auto deferredPass = crimild::alloc< DeferredRenderPass >();
    //deferredPass->getImageEffects().add( crimild::alloc< opengl::BloomImageEffect >( 0.5f, 16.0f, 0.1f, 2.0f ) );
    
    //camera->setRenderPass( deferredPass );

    bool useDeferredRenderPass = false;
    
    scene->attachComponent( crimild::alloc< LambdaComponent >( [&]( Node *, const Clock & ) {
        if ( Input::getInstance()->isKeyDown( '1' ) ) {
            useDeferredRenderPass = !useDeferredRenderPass;
            if ( useDeferredRenderPass ) {
                camera->setRenderPass( deferredPass );
                text->setText( "Deferred Render Pass" );
            }
            else {
                camera->setRenderPass( forwardPass );
                text->setText( "Forward Render Pass" );
            }
        }
        else if ( Input::getInstance()->isKeyDown( '2' ) ) {
            deferredPass->enableDebugMode( !deferredPass->isDebugModeEnabled() );
        }
    }));

	sim->setScene( scene );

	return sim->run();
}

