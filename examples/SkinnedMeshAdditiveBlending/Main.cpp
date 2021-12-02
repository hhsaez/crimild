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

int main( int argc, char **argv )
{
    auto sim = crimild::alloc< sdl::SDLSimulation >( "Sninned Mesh with Additive Blending", crimild::alloc< Settings >( argc, argv ) );
	sim->getRenderer()->getScreenBuffer()->setClearColor( RGBAColorf( 0.5f, 0.5f, 0.5f, 0.0f ) );

	Input::getInstance()->setMouseCursorMode( Input::MouseCursorMode::HIDDEN );

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

				auto centerAnim = crimild::alloc< Animation >(
					crimild::alloc< Clip >(
						"center",
						crimild::alloc< Quaternion4fChannel >(
							"astroBoy_newSkeleton_neck01[r]",
							containers::Array< crimild::Real32 > { 2.0 },
							containers::Array< Quaternion4f > { Quaternion4f::createFromAxisAngle( Vector3f::UNIT_Y, 0.0 ) }
							)
						)
					);
				
				auto leftAnim = crimild::alloc< Animation >(
					crimild::alloc< Clip >(
						"left",
						crimild::alloc< Quaternion4fChannel >(
							"astroBoy_newSkeleton_neck01[r]",
							containers::Array< crimild::Real32 > { 2.0 },
							containers::Array< Quaternion4f > { Quaternion4f::createFromAxisAngle( Vector3f::UNIT_Z, -0.45 * Numericf::PI ) }
							)
						)
					);
				
				auto rightAnim = crimild::alloc< Animation >(
					crimild::alloc< Clip >(
						"right",
						crimild::alloc< Quaternion4fChannel >(
							"astroBoy_newSkeleton_neck01[r]",
							containers::Array< crimild::Real32 > { 2.0 },
							containers::Array< Quaternion4f > { Quaternion4f::createFromAxisAngle( Vector3f::UNIT_Z, 0.45 * Numericf::PI ) }
							)
						)
					);
				
				auto upAnim = crimild::alloc< Animation >(
					crimild::alloc< Clip >(
						"up",
						crimild::alloc< Quaternion4fChannel >(
							"astroBoy_newSkeleton_neck01[r]",
							containers::Array< crimild::Real32 > { 2.0 },
							containers::Array< Quaternion4f > { Quaternion4f::createFromAxisAngle( Vector3f::UNIT_Y, -0.45 * Numericf::PI ) }
							)
						)
					);
				
				auto downAnim = crimild::alloc< Animation >(
					crimild::alloc< Clip >(
						"down",
						crimild::alloc< Quaternion4fChannel >(
							"astroBoy_newSkeleton_neck01[r]",
							containers::Array< crimild::Real32 > { 2.0 },
							containers::Array< Quaternion4f > { Quaternion4f::createFromAxisAngle( Vector3f::UNIT_Y, 0.45 * Numericf::PI ) }
							)
						)
					);

				model->attachComponent< LambdaComponent >( [ animation, skeleton, centerAnim, leftAnim, rightAnim, upAnim, downAnim ]( Node *node, const Clock &c ) {
					auto mousePos = Input::getInstance()->getNormalizedMousePosition();

					auto xLeft = Numericf::clamp( 2.0f * mousePos.x(), 0.0f, 1.0f );
					auto xRight = Numericf::clamp( 2.0f * ( mousePos.x() - 0.5f ), 0.0f, 1.0f );
					leftAnim->update( c )->lerp( centerAnim, xLeft )->lerp( rightAnim, xRight );

					auto yUp = Numericf::clamp( mousePos.y() / 0.3f, 0.0f, 1.0f );
					auto yDown = Numericf::clamp( ( mousePos.y() - 0.3f ) / 0.7f, 0.0f, 1.0f );
					upAnim->update( c )->lerp( centerAnim, yUp )->lerp( downAnim, yDown );
					
					leftAnim->lerp( upAnim, 0.5f, false );

					
					animation->update( c )->add( leftAnim, 1.0f );
					skeleton->animate( crimild::get_ptr( animation ) );
					node->perform( UpdateWorldState() );
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

