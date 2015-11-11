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

#include "DepthOfFieldImageEffect.hpp"

#include "Rendering/OpenGLUtils.hpp"

using namespace crimild;
using namespace crimild::opengl;

const char *dof_blur_vs = { CRIMILD_TO_STRING(
    in vec3 aPosition;
    in vec2 aTextureCoord;

    out vec2 vTextureCoord;

    void main()
    {
        vTextureCoord = aTextureCoord;
        gl_Position = vec4( aPosition.x, aPosition.y, 0.0, 1.0 );
    }
)};

const char *dof_blur_fs = { CRIMILD_TO_STRING(
                                           
    in vec2 vTextureCoord;
                                               
    uniform sampler2D uColorMap;
    uniform sampler2D uDepthMap;
                                               
    out vec4 vFragColor;

    const float blurClamp = 10.0;  // max blur amount
                    
    uniform float uAperture;
    uniform float uFocus;

    void main()
    {
        float aspectRatio = 16.0 / 9.0;
        vec2 aspectCorrect = vec2( 1.0, aspectRatio );
        
        vec4 depth1 = texture( uDepthMap, vTextureCoord );
        
        float factor = ( depth1.x - uFocus );
        
        vec2 dofblur = vec2( clamp( factor * uAperture, -blurClamp, blurClamp ) );
        
        vec4 col = vec4( 0.0 );
        
        col += texture( uColorMap, vTextureCoord );
        col += texture( uColorMap, vTextureCoord  + (vec2( 0.0,0.4 )*aspectCorrect ) * dofblur);
        col += texture( uColorMap, vTextureCoord  + (vec2( 0.15,0.37 )*aspectCorrect ) * dofblur);
        col += texture( uColorMap, vTextureCoord  + (vec2( 0.29,0.29 )*aspectCorrect ) * dofblur);
        col += texture( uColorMap, vTextureCoord  + (vec2( -0.37,0.15 )*aspectCorrect ) * dofblur);
        col += texture( uColorMap, vTextureCoord  + (vec2( 0.4,0.0 )*aspectCorrect ) * dofblur);
        col += texture( uColorMap, vTextureCoord  + (vec2( 0.37,-0.15 )*aspectCorrect ) * dofblur);
        col += texture( uColorMap, vTextureCoord  + (vec2( 0.29,-0.29 )*aspectCorrect ) * dofblur);
        col += texture( uColorMap, vTextureCoord  + (vec2( -0.15,-0.37 )*aspectCorrect ) * dofblur);
        col += texture( uColorMap, vTextureCoord  + (vec2( 0.0,-0.4 )*aspectCorrect ) * dofblur);
        col += texture( uColorMap, vTextureCoord  + (vec2( -0.15,0.37 )*aspectCorrect ) * dofblur);
        col += texture( uColorMap, vTextureCoord  + (vec2( -0.29,0.29 )*aspectCorrect ) * dofblur);
        col += texture( uColorMap, vTextureCoord  + (vec2( 0.37,0.15 )*aspectCorrect ) * dofblur);
        col += texture( uColorMap, vTextureCoord  + (vec2( -0.4,0.0 )*aspectCorrect ) * dofblur);
        col += texture( uColorMap, vTextureCoord  + (vec2( -0.37,-0.15 )*aspectCorrect ) * dofblur);
        col += texture( uColorMap, vTextureCoord  + (vec2( -0.29,-0.29 )*aspectCorrect ) * dofblur);
        col += texture( uColorMap, vTextureCoord  + (vec2( 0.15,-0.37 )*aspectCorrect ) * dofblur);
        
        col += texture( uColorMap, vTextureCoord  + (vec2( 0.15,0.37 )*aspectCorrect ) * dofblur*0.9);
        col += texture( uColorMap, vTextureCoord  + (vec2( -0.37,0.15 )*aspectCorrect ) * dofblur*0.9);
        col += texture( uColorMap, vTextureCoord  + (vec2( 0.37,-0.15 )*aspectCorrect ) * dofblur*0.9);
        col += texture( uColorMap, vTextureCoord  + (vec2( -0.15,-0.37 )*aspectCorrect ) * dofblur*0.9);
        col += texture( uColorMap, vTextureCoord  + (vec2( -0.15,0.37 )*aspectCorrect ) * dofblur*0.9);
        col += texture( uColorMap, vTextureCoord  + (vec2( 0.37,0.15 )*aspectCorrect ) * dofblur*0.9);
        col += texture( uColorMap, vTextureCoord  + (vec2( -0.37,-0.15 )*aspectCorrect ) * dofblur*0.9);
        col += texture( uColorMap, vTextureCoord  + (vec2( 0.15,-0.37 )*aspectCorrect ) * dofblur*0.9);
        
        col += texture( uColorMap, vTextureCoord  + (vec2( 0.29,0.29 )*aspectCorrect ) * dofblur*0.7);
        col += texture( uColorMap, vTextureCoord  + (vec2( 0.4,0.0 )*aspectCorrect ) * dofblur*0.7);
        col += texture( uColorMap, vTextureCoord  + (vec2( 0.29,-0.29 )*aspectCorrect ) * dofblur*0.7);
        col += texture( uColorMap, vTextureCoord  + (vec2( 0.0,-0.4 )*aspectCorrect ) * dofblur*0.7);
        col += texture( uColorMap, vTextureCoord  + (vec2( -0.29,0.29 )*aspectCorrect ) * dofblur*0.7);
        col += texture( uColorMap, vTextureCoord  + (vec2( -0.4,0.0 )*aspectCorrect ) * dofblur*0.7);
        col += texture( uColorMap, vTextureCoord  + (vec2( -0.29,-0.29 )*aspectCorrect ) * dofblur*0.7);
        col += texture( uColorMap, vTextureCoord  + (vec2( 0.0,0.4 )*aspectCorrect ) * dofblur*0.7);
        
        col += texture( uColorMap, vTextureCoord  + (vec2( 0.29,0.29 )*aspectCorrect ) * dofblur*0.4);
        col += texture( uColorMap, vTextureCoord  + (vec2( 0.4,0.0 )*aspectCorrect ) * dofblur*0.4);
        col += texture( uColorMap, vTextureCoord  + (vec2( 0.29,-0.29 )*aspectCorrect ) * dofblur*0.4);
        col += texture( uColorMap, vTextureCoord  + (vec2( 0.0,-0.4 )*aspectCorrect ) * dofblur*0.4);
        col += texture( uColorMap, vTextureCoord  + (vec2( -0.29,0.29 )*aspectCorrect ) * dofblur*0.4);
        col += texture( uColorMap, vTextureCoord  + (vec2( -0.4,0.0 )*aspectCorrect ) * dofblur*0.4);
        col += texture( uColorMap, vTextureCoord  + (vec2( -0.29,-0.29 )*aspectCorrect ) * dofblur*0.4);
        col += texture( uColorMap, vTextureCoord  + (vec2( 0.0,0.4 )*aspectCorrect ) * dofblur*0.4);
        
        vFragColor = col / 41.0;
        vFragColor.a = 1.0;
    }
)};

DepthOfFieldImageEffect::DepthOfFieldImageEffect( void )
    : _focus( crimild::alloc< FloatUniform >( "uFocus", 0.875f ) ),
      _aperture( crimild::alloc< FloatUniform >( "uAperture", 0.1f ) )
{

}

DepthOfFieldImageEffect::~DepthOfFieldImageEffect( void )
{

}

void DepthOfFieldImageEffect::compute( Renderer *renderer, Camera *camera )
{
    
}

void DepthOfFieldImageEffect::apply( crimild::Renderer *renderer, crimild::Camera * )
{
    auto sceneFBO = renderer->getFrameBuffer( RenderPass::S_BUFFER_NAME );
	if ( sceneFBO == nullptr ) {
        Log::Error << "Cannot find FBO named '" << RenderPass::S_BUFFER_NAME << "'" << Log::End;
		return;
	}
    
    auto colorTarget = sceneFBO->getRenderTargets().get( RenderPass::S_BUFFER_COLOR_TARGET_NAME );

    auto gBuffer = renderer->getFrameBuffer( RenderPass::G_BUFFER_NAME );
    if ( gBuffer == nullptr ) {
        Log::Error << "Cannot find FBO named '" << RenderPass::G_BUFFER_NAME << "'" << Log::End;
        return;
    }
    
    auto depthTarget = gBuffer->getRenderTargets().get( RenderPass::G_BUFFER_DEPTH_TARGET_NAME );

    auto dofBlurProgram = renderer->getShaderProgram( "dof_blur" );
	if ( dofBlurProgram == nullptr ) {
		auto tmp = crimild::alloc< ShaderProgram >( OpenGLUtils::getVertexShaderInstance( dof_blur_vs ), OpenGLUtils::getFragmentShaderInstance( dof_blur_fs ) );
        renderer->setShaderProgram( "dof_blur", tmp );
        dofBlurProgram = crimild::get_ptr( tmp );

		dofBlurProgram->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
	    dofBlurProgram->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );
	    
	    dofBlurProgram->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM, "uColorMap" );
	    dofBlurProgram->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::DEPTH_MAP_UNIFORM, "uDepthMap" );
        
        dofBlurProgram->attachUniform( _focus );
        dofBlurProgram->attachUniform( _aperture );
	}

    renderer->bindProgram( dofBlurProgram );
    renderer->bindTexture( dofBlurProgram->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), colorTarget->getTexture() );
    renderer->bindTexture( dofBlurProgram->getStandardLocation( ShaderProgram::StandardLocation::DEPTH_MAP_UNIFORM ), depthTarget->getTexture() );
		
    renderer->drawScreenPrimitive( dofBlurProgram );
		
    renderer->unbindTexture( dofBlurProgram->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), colorTarget->getTexture() );
    renderer->unbindTexture( dofBlurProgram->getStandardLocation( ShaderProgram::StandardLocation::DEPTH_MAP_UNIFORM ), depthTarget->getTexture() );
    renderer->unbindProgram( dofBlurProgram );
}

