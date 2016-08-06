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

const char *dof_generic_vs = R"(
    CRIMILD_GLSL_ATTRIBUTE vec3 aPosition;
    CRIMILD_GLSL_ATTRIBUTE vec2 aTextureCoord;

    CRIMILD_GLSL_VARYING_OUT vec2 vTextureCoord;

    void main()
    {
        vTextureCoord = aTextureCoord;
        CRIMILD_GLSL_VERTEX_OUTPUT = vec4( aPosition, 1.0 );
    }    
)";

const char *dof_generic_fs = R"(
    CRIMILD_GLSL_PRECISION_FLOAT_HIGH

    CRIMILD_GLSL_VARYING_IN vec2 vTextureCoord;

    uniform sampler2D uColorMap;

    CRIMILD_GLSL_DECLARE_FRAGMENT_OUTPUT

    void main( void ) 
    {
        CRIMILD_GLSL_FRAGMENT_OUTPUT = CRIMILD_GLSL_FN_TEXTURE_2D( uColorMap, vTextureCoord );
    }
)";

const char *dof_blur_fs = R"(
    CRIMILD_GLSL_PRECISION_FLOAT_HIGH

    CRIMILD_GLSL_VARYING_IN vec2 vTextureCoord;

    uniform vec2 uScale;
    uniform sampler2D uColorMap;

    CRIMILD_GLSL_DECLARE_FRAGMENT_OUTPUT

    const vec2 GAUSS_FILTER[ 7 ] = vec2[](
        vec2( -3.0, 0.015625 ),
        vec2( -2.0, 0.09375 ),
        vec2( -1.0, 0.234375 ),
        vec2( 0.0, 0.3125 ),
        vec2( 1.0, 0.234375 ),
        vec2( 2.0, 0.09375 ),
        vec2( 3.0, 0.015625 )
    );
     
    void main( void ) 
    {
        vec4 color = vec4( 0.0 );

        for ( int i = 0; i < 7; i++ ) {
            color += 2.0 * GAUSS_FILTER[ i ].y * CRIMILD_GLSL_FN_TEXTURE_2D( 
                uColorMap, 
                vec2( vTextureCoord.x + GAUSS_FILTER[ i ].x * uScale.x, vTextureCoord.y + GAUSS_FILTER[ i ].x * uScale.y )
            );
        }
     
        CRIMILD_GLSL_FRAGMENT_OUTPUT = color;
    }
)";

const char *dof_composite_fs = R"(
    CRIMILD_GLSL_PRECISION_FLOAT_HIGH

    CRIMILD_GLSL_VARYING_IN vec2 vTextureCoord;

    uniform sampler2D uColorMap;
    uniform sampler2D uDepthMap;
    uniform sampler2D uBlurMap;

    CRIMILD_GLSL_DECLARE_FRAGMENT_OUTPUT

    void main( void ) 
    {
        float depth = CRIMILD_GLSL_FN_TEXTURE_2D( uDepthMap, vTextureCoord ).r;
        if ( depth < 1.0 - 0.009 ) {
            CRIMILD_GLSL_FRAGMENT_OUTPUT = CRIMILD_GLSL_FN_TEXTURE_2D( uColorMap, vTextureCoord );
        }
        else {
            CRIMILD_GLSL_FRAGMENT_OUTPUT = CRIMILD_GLSL_FN_TEXTURE_2D( uBlurMap, vTextureCoord );
        }
    }
)";

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
    auto sceneFBO = renderer->getFrameBuffer( RenderPass::S_BUFFER_NAME );
    if ( sceneFBO == nullptr ) {
        Log::Error << "Cannot find FBO named '" << RenderPass::S_BUFFER_NAME << "'" << Log::End;
        return;
    }
    
    auto colorTarget = sceneFBO->getRenderTargets().get( RenderPass::S_BUFFER_COLOR_TARGET_NAME );
    if ( colorTarget == nullptr ) {
        Log::Error << "Cannot get color target from scene" << Log::End;
        return;
    }

    // STEP 1: Downscale
    auto downscaleFBO = renderer->getFrameBuffer( Renderer::FBO_AUX_512 );
    if ( downscaleFBO == nullptr ) {
        Log::Error << "Cannot find FBO named '" << Renderer::FBO_AUX_512 << "'" << Log::End;
        return;
    }

    auto dofDownscaleProgram = renderer->getShaderProgram( "dof_downscale" );
    if ( dofDownscaleProgram == nullptr ) {
        auto tmp = crimild::alloc< ShaderProgram >( OpenGLUtils::getVertexShaderInstance( dof_generic_vs ), OpenGLUtils::getFragmentShaderInstance( dof_generic_fs ) );
        renderer->setShaderProgram( "dof_downscale", tmp );
        dofDownscaleProgram = crimild::get_ptr( tmp );

        dofDownscaleProgram->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
        dofDownscaleProgram->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );
        
        dofDownscaleProgram->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM, "uColorMap" );
    }

    renderer->bindFrameBuffer( downscaleFBO );
    renderer->bindProgram( dofDownscaleProgram );
    renderer->bindTexture( dofDownscaleProgram->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), colorTarget->getTexture() );
    renderer->drawScreenPrimitive( dofDownscaleProgram );
    renderer->unbindTexture( dofDownscaleProgram->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), colorTarget->getTexture() );
    renderer->unbindProgram( dofDownscaleProgram );
    renderer->unbindFrameBuffer( downscaleFBO );   

    // STEP 2: Blur
    auto blurFBO = renderer->getFrameBuffer( Renderer::FBO_AUX_1024 );
    if ( blurFBO == nullptr ) {
        Log::Error << "Cannot find FBO named '" << Renderer::FBO_AUX_1024 << "'" << Log::End;
        return;
    }

    colorTarget = downscaleFBO->getRenderTargets().get( Renderer::FBO_AUX_COLOR_TARGET_NAME );
    if ( colorTarget->getTexture() == nullptr ) {
        Log::Error << "Color texture is null" << Log::End;
        return;
    }

    auto dofBlurProgram = renderer->getShaderProgram( "dof_blur" );
    if ( dofBlurProgram == nullptr ) {
        auto tmp = crimild::alloc< ShaderProgram >( OpenGLUtils::getVertexShaderInstance( dof_generic_vs ), OpenGLUtils::getFragmentShaderInstance( dof_blur_fs ) );
        renderer->setShaderProgram( "dof_blur", tmp );
        dofBlurProgram = crimild::get_ptr( tmp );

        dofBlurProgram->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
        dofBlurProgram->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );
        
        dofBlurProgram->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM, "uColorMap" );
        
        dofBlurProgram->attachUniform( crimild::alloc< Vector2fUniform >( "uScale", Vector2f( 1.0f / blurFBO->getWidth(), 1.0f / blurFBO->getHeight() ) ) );
    }

    renderer->bindFrameBuffer( blurFBO );
    renderer->bindProgram( dofBlurProgram );
    renderer->bindTexture( dofBlurProgram->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), colorTarget->getTexture() );
        
    renderer->drawScreenPrimitive( dofBlurProgram );
        
    renderer->unbindTexture( dofBlurProgram->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), colorTarget->getTexture() );
    renderer->unbindProgram( dofBlurProgram );
    renderer->unbindFrameBuffer( blurFBO );   
}

void DepthOfFieldImageEffect::apply( crimild::Renderer *renderer, crimild::Camera * )
{
    auto sceneFBO = renderer->getFrameBuffer( RenderPass::S_BUFFER_NAME );
	if ( sceneFBO == nullptr ) {
        Log::Error << "Cannot find FBO named '" << RenderPass::S_BUFFER_NAME << "'" << Log::End;
		return;
	}
    
    auto colorTarget = sceneFBO->getRenderTargets().get( RenderPass::S_BUFFER_COLOR_TARGET_NAME );
    if ( colorTarget->getTexture() == nullptr ) {
        Log::Error << "Color texture is null" << Log::End;
        return;
    }

    auto depthTarget = sceneFBO->getRenderTargets().get( RenderPass::S_BUFFER_DEPTH_TARGET_NAME );
    if ( depthTarget->getTexture() == nullptr ) {
        Log::Error << "Depth texture is null" << Log::End;
        return;
    }

    auto blurFBO = renderer->getFrameBuffer( Renderer::FBO_AUX_1024 );
    if ( blurFBO == nullptr ) {
        Log::Error << "Cannot find FBO named '" << Renderer::FBO_AUX_1024 << "'" << Log::End;
        return;
    }

    auto blurTarget = blurFBO->getRenderTargets().get( Renderer::FBO_AUX_COLOR_TARGET_NAME );
    if ( blurTarget->getTexture() == nullptr ) {
        Log::Error << "Blur texture is null" << Log::End;
        return;
    }

    auto dofCompositeProgram = renderer->getShaderProgram( "dof_composite" );
	if ( dofCompositeProgram == nullptr ) {
		auto tmp = crimild::alloc< ShaderProgram >( OpenGLUtils::getVertexShaderInstance( dof_generic_vs ), OpenGLUtils::getFragmentShaderInstance( dof_composite_fs ) );
        renderer->setShaderProgram( "dof_composite", tmp );
        dofCompositeProgram = crimild::get_ptr( tmp );

		dofCompositeProgram->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
	    dofCompositeProgram->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );
	    
	    dofCompositeProgram->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM, "uColorMap" );
	    dofCompositeProgram->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::DEPTH_MAP_UNIFORM, "uDepthMap" );
        dofCompositeProgram->registerLocation( crimild::alloc< ShaderLocation >( ShaderLocation::Type::UNIFORM, "uBlurMap" ) );
        
        // dofCompositeProgram->attachUniform( _focus );
        // dofCompositeProgram->attachUniform( _aperture );
        // dofCompositeProgram->attachUniform( crimild::alloc< Vector2fUniform >( "uScale", Vector2f( 1.0f / 1024.0f, 1.0f / 1024.0f ) ) );
	}

    renderer->bindProgram( dofCompositeProgram );
    
    renderer->bindTexture( dofCompositeProgram->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), colorTarget->getTexture() );
    renderer->bindTexture( dofCompositeProgram->getStandardLocation( ShaderProgram::StandardLocation::DEPTH_MAP_UNIFORM ), depthTarget->getTexture() );
    renderer->bindTexture( dofCompositeProgram->getLocation( "uBlurMap" ), blurTarget->getTexture() );
		
    renderer->drawScreenPrimitive( dofCompositeProgram );
		
    renderer->unbindTexture( dofCompositeProgram->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), colorTarget->getTexture() );
    renderer->unbindTexture( dofCompositeProgram->getStandardLocation( ShaderProgram::StandardLocation::DEPTH_MAP_UNIFORM ), depthTarget->getTexture() );
    renderer->unbindTexture( dofCompositeProgram->getLocation( "uBlurMap" ), blurTarget->getTexture() );
    
    renderer->unbindProgram( dofCompositeProgram );
}

