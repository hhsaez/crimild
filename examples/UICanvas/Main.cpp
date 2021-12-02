/*
 * Copyright (c) 2002-present, H. Hernán Saez
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
#include <Crimild_OpenGL.hpp>

#include "UI/UIFrame.hpp"
#include "UI/UICanvas.hpp"
#include "UI/UIBackground.hpp"
#include "UI/UIButton.hpp"
#include "UI/UILabel.hpp"
#include "UI/UIFrameConstraint.hpp"
#include "UI/UIFrameConstraintMaker.hpp"
#include "Boundings/Box2DBoundingVolume.hpp"

using namespace crimild;
using namespace crimild::rendergraph;
using namespace crimild::rendergraph::passes;
using namespace crimild::ui;

SharedPointer< Node > buildUI( void )
{
	auto canvas = crimild::alloc< Group >();
	canvas->attachComponent< UICanvas >( 800, 600 );
    canvas->attachComponent< UIBackground >( RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ) );

	auto view1 = crimild::alloc< Group >();
	view1->attachComponent< UIFrame >()->pin()->top( canvas )->left( canvas )->size( 200, 400 )->margin( 10.0f );
	view1->attachComponent< UIBackground >( RGBAColorf( 1.0f, 0.0f, 0.0f, 1.0f ) );
	canvas->attachNode( view1 );

	auto view2 = crimild::alloc< Group >();
	view2->attachComponent< UIFrame >()->pin()->fillParent()->margin( 20 );
	view2->attachComponent< UIBackground >( RGBAColorf( 0.0f, 1.0f, 0.0f, 1.0f ) );
	view1->attachNode( view2 );

	auto view3 = crimild::alloc< Group >();
	view3->attachComponent< UIFrame >()->pin()->size( 200, 300 )->after( view1 )->centerY( view1 )->marginLeft( 10 );
	view3->attachComponent< UIBackground >( RGBAColorf( 0.0f, 0.0f, 1.0f, 1.0f ) );
	canvas->attachNode( view3 );

	auto view4 = crimild::alloc< Group >();
	view4->attachComponent< UIFrame >()->pin()->size( 50, 30 )->below( view3 )->centerX( view3 )->marginTop( 20 );
	view4->attachComponent< UIBackground >( RGBAColorf( 0.0f, 1.0f, 1.0f, 1.0f ) );
	canvas->attachNode( view4 );

	auto view5 = crimild::alloc< Group >();
	view5->attachComponent< UIFrame >()->pin()->after( view3 )->top( view3 )->right( canvas )->height( 250 )->margin( 0, 10, 0, 10 );
	view5->attachComponent< UIBackground >( RGBAColorf( 1.0f, 0.0f, 1.0f, 1.0f ) );
	canvas->attachNode( view5 );

	auto view6 = crimild::alloc< Group >();
	view6->attachComponent< UIFrame >()->pin()->after( view3 )->below( view5 )->bottom( view4 )->right( canvas )->margin( 10, 5, 0, 5 );
	view6->attachComponent< UIBackground >( RGBAColorf( 1.0f, 0.5f, 0.0f, 1.0f ) );
	canvas->attachNode( view6 );

    auto lblHello = crimild::alloc< Group >();
    lblHello->attachComponent< UIFrame >()->pin()->size( 100, 40 )->left()->bottom()->margin( 10, 10, 10, 10 );
    lblHello->attachComponent< UILabel >( "Hello World!", RGBAColorf( 1.0f, 0.0f, 1.0f, 1.0f ) );
    canvas->attachNode( lblHello );

	auto btnTest = crimild::alloc< Group >();
	btnTest->attachComponent< UIFrame >()->pin()->size( 300, 50 )->centerX( view6 )->right()->bottom();
	btnTest->attachComponent< UIBackground >( RGBAColorf( 1.0f, 1.0f, 0.0f, 1.0f ) );
    btnTest->attachComponent< UIButton >( []( Node *node ) {
        static int count = 0;
        std::stringstream ss;
        ss << "Click count: " << ++count;
        node->getComponent< UILabel >()->setText( ss.str() );
        return true;
    });
    btnTest->attachComponent< UILabel >( "Click Here!", RGBAColorf( 1.0f, 0.0f, 1.0f, 1.0f ) );
    canvas->attachNode( btnTest );

	return canvas;
}

int main( int argc, char **argv )
{
    auto settings = crimild::alloc< Settings >( argc, argv );
    settings->set( "video.show_frame_time", true );
    settings->set( "fonts.default", "assets/fonts/Verdana.txt" );
    auto sim = crimild::alloc< sdl::SDLSimulation >( "UICanvas", settings );

    auto scene = crimild::alloc< Group >();

	scene->attachNode( buildUI() );

	auto camera = crimild::alloc< Camera >();
	camera->local().setTranslate( -400.0f, 400.0f, 400.0f );
	camera->local().lookAt( Vector3f::ZERO );
	scene->attachNode( camera );
    
    sim->setScene( scene );

	sim->registerMessageHandler< crimild::messaging::KeyReleased >( [ scene, camera ]( crimild::messaging::KeyReleased const &msg ) {
		switch ( msg.key ) {
			case CRIMILD_INPUT_KEY_Q:
                scene->perform( Apply( []( Node *node ) {
                    if ( auto canvas = node->getComponent< UICanvas >() ) {
                        canvas->setRenderSpace( UICanvas::RenderSpace::CAMERA );
                    }
                }));
				break;

			case CRIMILD_INPUT_KEY_W:
                scene->perform( Apply( []( Node *node ) {
                    if ( auto canvas = node->getComponent< UICanvas >() ) {
                        canvas->setRenderSpace( UICanvas::RenderSpace::WORLD );
                    }
                }));
				break;
		}
	});

	return sim->run();
}

