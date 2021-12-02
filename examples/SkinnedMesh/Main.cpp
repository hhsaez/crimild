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
#include <Crimild_Import.hpp>

using namespace crimild;
using namespace crimild::animation;
using namespace crimild::messaging;
using namespace crimild::import;
using namespace crimild::sdl;

int main( int argc, char **argv )
{
    auto sim = crimild::alloc< SDLSimulation >( "Sninned Mesh", crimild::alloc< Settings >( argc, argv ) );
	sim->getRenderer()->getScreenBuffer()->setClearColor( RGBAColorf( 0.5f, 0.5f, 0.5f, 0.0f ) );

    auto scene = crimild::alloc< Group >();

    auto camera = crimild::alloc< Camera >();
	camera->local().setTranslate( 0.0f, 0.0f, 15.0f );
    scene->attachNode( camera );

    auto light = crimild::alloc< Light >( Light::Type::DIRECTIONAL );
    light->local().setTranslate( 1.0f, 1.0f, 1.0f );
    light->local().lookAt( Vector3f( 0.0f, 0.0f, 0.0f ), Vector3f( 0.0f, 1.0f, 0.0f ) );
    camera->attachNode( light );

	SceneImporter importer;
	auto model = importer.import( FileSystem::getInstance().pathForResource( "assets/models/astroboy/astroBoy_walk_Max.dae" ) );
	if ( model != nullptr ) {
		model->perform( UpdateWorldState() );

        // make sure the object is properly scaled and centered
        float scale = 10.0f / model->getWorldBound()->getRadius();
        model->local().setScale( scale );
        model->local().translate() -= scale * Vector3f( 0.0f, model->getWorldBound()->getCenter()[ 1 ], 0.0f );

		scene->attachNode( model );

		if ( auto skeleton = model->getComponent< Skeleton >() ) {
			if ( skeleton->getClips().size() > 0 ) {
				auto animation = crimild::alloc< Animation >( skeleton->getClips()[ "combinedAnim_0" ] );

				model->attachComponent< LambdaComponent >( [ animation, skeleton ]( Node *node, const Clock &c ) {
					animation->update( c );
					skeleton->animate( crimild::get_ptr( animation ) );
				});
			}
		}
		else {
			std::cout << "No skeleton" << std::endl;
		}
	}
	else {
		Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot load model" );
		return -1;
	}

	sim->registerMessageHandler< KeyPressed >( []( KeyPressed const &msg ) {
		if ( msg.key == 'K' ) {
			MessageQueue::getInstance()->broadcastMessage( ToggleDebugInfo {} );
		}
	});

    sim->setScene( scene );
	
	return sim->run();
}

