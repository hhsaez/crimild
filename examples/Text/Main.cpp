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
using namespace crimild::rendergraph;
using namespace crimild::sdl;

SharedPointer< Node > generateText( SharedPointer< Font > const &font, std::string str, const Vector3f position, const RGBAColorf &color )
{
    auto text = crimild::alloc< Text >();
    text->setFont( font );
    text->setSize( 1.0f );
    text->setText( str );
    text->setTextColor( color );
    text->local().setTranslate( position );
    return text;
}

int main( int argc, char **argv )
{
    crimild::init();

    auto sim = crimild::alloc< SDLSimulation >( "Rendering text", crimild::alloc< Settings >( argc, argv ) );

    auto scene = crimild::alloc< Group >();

    auto font = crimild::alloc< Font >( FileSystem::getInstance().pathForResource( "Verdana.txt" ) );

    std::string lorem = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\nInteger egestas sit amet arcu at laoreet.";
    
    auto texts = crimild::alloc< Group >();
    texts->attachNode( generateText( font, lorem, Vector3f( -5.0f, 0.0f, 0.0f ), RGBAColorf( 1.0f, 0.0f, 0.0f, 1.0f ) ) );
    texts->attachComponent( crimild::alloc< LambdaComponent >( []( Node *node, const Clock &t ) {
        if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_UP ) ) {
            node->local().translate() -= 2.0f * t.getDeltaTime() * node->getLocal().computeDirection();
        }
        else if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_DOWN ) ) {
            node->local().translate() += 2.0f * t.getDeltaTime() * node->getLocal().computeDirection();
        }
        if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_LEFT ) ) {
            node->local().translate() += 2.0f * t.getDeltaTime() * node->getLocal().computeRight();
        }
        else if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_RIGHT ) ) {
            node->local().translate() -= 2.0f * t.getDeltaTime() * node->getLocal().computeRight();
        }
    }));
    scene->attachNode( texts );

    auto camera = crimild::alloc< Camera >( 45.0f, 4.0f / 3.0f, 0.1f, 1024.0f );
	camera->local().setTranslate( Vector3f( 0.0f, 0.0f, 10.0f ) );

	scene->attachNode( camera );

    sim->setScene( scene );
	return sim->run();
}

