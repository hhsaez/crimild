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
using namespace crimild::animation;
using namespace crimild::sdl;

/*
  \brief Renders a triangle in a texture and then use that texture on quads
 */
int main( int argc, char **argv )
{
    crimild::init();
    
    auto sim = crimild::alloc< SDLSimulation >( "RenderToTexture", crimild::alloc< Settings >( argc, argv ) );
	sim->getRenderer()->getScreenBuffer()->setClearColor( RGBAColorf( 0.5f, 0.5f, 0.5f, 1.0f ) );

    auto scene = crimild::alloc< Group >();

	auto offscreenScene = crimild::alloc< Group >();
	offscreenScene->local().setTranslate( 9999.0f, 9999.0f, 9999.0f ); // send it away
	scene->attachNode( offscreenScene );
    
    coding::FileDecoder decoder;
    if ( decoder.read( FileSystem::getInstance().pathForResource( "model.crimild" ) ) ) {
        if ( decoder.getObjectCount() > 0 ) {
            auto model = decoder.getObjectAt< Node >( 0 );
			if ( auto skeleton = model->getComponent< Skeleton >() ) {
				if ( skeleton->getClips().size() > 0 ) {
					auto animation = crimild::alloc< Animation >( skeleton->getClips().values().first() );
					model->attachComponent< LambdaComponent >( [ animation, skeleton ]( Node *, const Clock &c ) {
						animation->update( c );
						skeleton->animate( animation );
					});
				}
			}
            model->perform( UpdateWorldState() );
            model->local().setScale( 1.0f / model->getWorldBound()->getRadius() );
            model->local().setTranslate( 0.0f, -0.5f, 0.0f ); // center model
            offscreenScene->attachNode( model );
        }
    }
    
    auto light = crimild::alloc< Light >();
    light->local().setTranslate( 0.0f, 0.0f, 5.0f );
    offscreenScene->attachNode( light );
    
	auto offscreenCamera = crimild::alloc< Camera >();
    offscreenCamera->setName( "offscreen camera" );
	auto offscreenFBO = crimild::alloc< StandardFrameBufferObject >( 128, 128 );
    offscreenFBO->setClearColor( RGBAColorf( 0.0f, 0.0f, 0.0f, 0.0f ) );
	auto offscreenPass = crimild::alloc< OffscreenRenderPass >( offscreenFBO );
	offscreenCamera->setRenderPass( offscreenPass );
    offscreenCamera->setAspectRatio( offscreenFBO->getAspectRatio() );
	offscreenCamera->local().setTranslate( 0.0f, 0.0f, 1.5f );
	offscreenScene->attachNode( offscreenCamera );

	auto quadMaterial = crimild::alloc< Material >();
    quadMaterial->getCullFaceState()->setEnabled( false );
    quadMaterial->getAlphaState()->setEnabled( true );
    quadMaterial->setProgram( Renderer::getInstance()->getShaderProgram( Renderer::SHADER_PROGRAM_UNLIT_TEXTURE ) );
	auto texture = offscreenFBO->getRenderTargets()[ RenderTarget::RENDER_TARGET_NAME_COLOR ]->getTexture();
	quadMaterial->setColorMap( texture );
    
    VertexPrecision quadVertices[] = {
        -1.0f, +1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        +1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        +1.0f, +1.0f, 0.0f, 1.0f, 1.0f,
    };
    
    IndexPrecision quadIndices[] = {
        0, 1, 2, 0, 2, 3,
    };
    
    auto quadPrimitive = crimild::alloc< Primitive >( Primitive::Type::TRIANGLES );
    quadPrimitive->setVertexBuffer( crimild::alloc< VertexBufferObject >( VertexFormat::VF_P3_UV2, 4, quadVertices ) );
    quadPrimitive->setIndexBuffer( crimild::alloc< IndexBufferObject >( 6, quadIndices ) );

	for ( float x = -2.0f; x <= 2.0f; x += 1.0f ) {
        for ( float y = -2.0f; y <= 2.0f; y += 1.0f ) {
            for ( float z = 0.0f; z <= 2.0f; z += 1.0f ) {
                auto quad = crimild::alloc< Geometry >();
                quad->attachPrimitive( quadPrimitive );
                quad->local().setScale( 0.75f );
                quad->local().setTranslate( 2.0f * x, 2.0f * y, -2.0f * z );
                quad->getComponent< MaterialComponent >()->attachMaterial( quadMaterial );
                //quad->attachComponent< RotationComponent >( Vector3f( 0.0f, 1.0f, 0.0f ), -0.1f * Numericf::HALF_PI );
                scene->attachNode( quad );
            }
        }
	}

    auto camera = crimild::alloc< Camera >();
    camera->setName( "camera" );
    camera->local().setTranslate( Vector3f( 0.0f, 0.0f, 5.0f ) );
	Camera::setMainCamera( camera ); // set main camera in advance
    scene->attachNode( camera );

    sim->setScene( scene );

	return sim->run();
}

