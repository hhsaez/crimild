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

    CRIMILD_GLSL_DECLARE_FRAGMENT_OUTPUT

    uniform vec2 uTexelSize;
    uniform sampler2D uColorMap;
    uniform sampler2D uDepthMap;

    uniform int uOrientation;
    uniform float uBlurCoefficient;  // Calculated from the blur equation, b = ( f * ms / N )
    uniform float uFocusDistance;    // The distance to the subject in perfect focus (= Ds)
    uniform float uNear;     // Near clipping plane
    uniform float uFar;      // Far clipping plane
    uniform float uPPM;      // Pixels per millimetre

    float unpack( vec4 c )
    {
        const vec4 bitShifts = vec4( 
            1.0,
            1.0 / 255.0,
            1.0 / (255.0 * 255.0),
            1.0 / (255.0 * 255.0 * 255.0));
        return dot( c, bitShifts );
    }

    /// <summary>
    /// Calculate the blur diameter to apply on the image.
    /// b = (f * ms / N) * (xd / (Ds +- xd))
    /// Where:
    /// (Ds + xd) for background objects
    /// (Ds - xd) for foreground objects
    /// </summary>
    /// <param name="d">Depth of the fragment.</param>
    float GetBlurDiameter( float d )
    {
        // Convert from linear depth to metres
        float Dd = d * ( uFar - uNear );
        
        float xd = abs( Dd - uFocusDistance );
        float xdd = ( Dd < uFocusDistance ) ? ( uFocusDistance - xd ) : ( uFocusDistance + xd );
        float b = uBlurCoefficient * ( xd / xdd );
        
        return b * uPPM;
    }

    /// <summary>
    /// Fragment shader entry.
    /// <summary>
    void main ()
    {
        // Maximum blur radius to limit hardware requirements.
        // Cannot #define this due to a driver issue with some setups
        const float MAX_BLUR_RADIUS = 10.0;

        // Pass the linear depth values recorded in the depth map to the blur
        // equation to find out how much each pixel should be blurred with the
        // given camera settings.
        float depth = unpack( CRIMILD_GLSL_FN_TEXTURE_2D( uDepthMap, vTextureCoord ) );
        float blurAmount = GetBlurDiameter( depth );
        blurAmount = min( floor( blurAmount ), MAX_BLUR_RADIUS );

        // Apply the blur
        float count = 0.0;
        vec4 colour = vec4(0.0);
        vec2 texelOffset;
        if ( uOrientation == 0 )
            texelOffset = vec2( uTexelSize.x, 0.0);
        else
            texelOffset = vec2(0.0, uTexelSize.y);
        
        if ( blurAmount >= 1.0 ) {
            float halfBlur = blurAmount * 0.5;
            for (float i = 0.0; i < MAX_BLUR_RADIUS; ++i)
            {
                if ( i >= blurAmount )
                    break;
                
                float offset = i - halfBlur;
                vec2 vOffset = vTextureCoord + (texelOffset * offset);

                colour += CRIMILD_GLSL_FN_TEXTURE_2D( uColorMap, vOffset);
                ++count;
            }
        }
        
        if ( count > 0.0 ) {
            CRIMILD_GLSL_FRAGMENT_OUTPUT = colour / count;
        }
        else {
            CRIMILD_GLSL_FRAGMENT_OUTPUT = CRIMILD_GLSL_FN_TEXTURE_2D( uColorMap, vTextureCoord );
        }
    }    
)";

const char *dof_composite_fs = R"(
    CRIMILD_GLSL_PRECISION_FLOAT_HIGH

    CRIMILD_GLSL_VARYING_IN vec2 vTextureCoord;

    CRIMILD_GLSL_DECLARE_FRAGMENT_OUTPUT

    uniform sampler2D uColorMap;
    uniform sampler2D uDepthMap;
    uniform sampler2D uBlurMap;

    uniform float uBlurCoefficient;
    uniform float uFocusDistance;
    uniform float uNear;
    uniform float uFar;
    uniform float uPPM;

    /// <summary>
    /// Calculate the blur diameter to apply on the image.
    /// b = (f * ms / N) * (xd / (Ds +- xd))
    /// Where:
    /// (Ds + xd) for background objects
    /// (Ds - xd) for foreground objects
    /// </summary>
    /// <param name="d">Depth of the fragment.</param>
    float GetBlurDiameter (float d)
    {
        // Convert from linear depth to metres
        float Dd = d * ( uFar - uNear );

        float xd = abs( Dd - uFocusDistance );
        float xdd = (Dd < uFocusDistance) ? (uFocusDistance - xd) : (uFocusDistance + xd);
        float b = uBlurCoefficient * (xd / xdd);
        
        return b * uPPM;
    }

    /// <summary>
    /// Fragment shader entry.
    /// <summary>
    void main ()
    {
        // Maximum blur radius to limit hardware requirements.
        // Cannot #define this due to a driver issue with some setups
        const float MAX_BLUR_RADIUS = 10.0;
            
        // Get the colour, depth, and blur pixels
        vec4 colour = CRIMILD_GLSL_FN_TEXTURE_2D( uColorMap, vTextureCoord );
        float depth = CRIMILD_GLSL_FN_TEXTURE_2D( uDepthMap, vTextureCoord ).r;
        vec4 blur = CRIMILD_GLSL_FN_TEXTURE_2D( uBlurMap, vTextureCoord );
        
        // Linearly interpolate between the colour and blur pixels based on DOF
        float blurAmount = GetBlurDiameter(depth);
        float lerp = min(blurAmount / MAX_BLUR_RADIUS, 1.0);

        CRIMILD_GLSL_FRAGMENT_OUTPUT = (colour * (1.0 - lerp)) + (blur * lerp);
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
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot find FBO named '", RenderPass::S_BUFFER_NAME, "'" );
        return;
    }
    
    auto colorTarget = sceneFBO->getRenderTargets().get( RenderPass::S_BUFFER_COLOR_TARGET_NAME );
    if ( colorTarget == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot get color target from scene" );
        return;
    }

    auto depthTarget = sceneFBO->getRenderTargets().get( RenderPass::S_BUFFER_DEPTH_TARGET_NAME );
    if ( depthTarget->getTexture() == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Depth texture is null" );
        return;
    }

    auto blurFBO = renderer->getFrameBuffer( Renderer::FBO_AUX_1024 );
    if ( blurFBO == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot find FBO named '", Renderer::FBO_AUX_1024, "'" );
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
        dofBlurProgram->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::DEPTH_MAP_UNIFORM, "uDepthMap" );

        auto uTexelSize = crimild::alloc< Vector2fUniform >( "uTexelSize", Vector2f( 1.0f / blurFBO->getWidth(), 1.0f / blurFBO->getHeight() ) );
        AssetManager::getInstance()->set( "dof/uniforms/uTexelSize", uTexelSize );
        dofBlurProgram->attachUniform( uTexelSize );

        auto uOrientation = crimild::alloc< IntUniform >( "uOrientation", 0 );
        AssetManager::getInstance()->set( "dof/uniforms/uOrientation", uOrientation );
        dofBlurProgram->attachUniform( uOrientation );

        float f = 200;
        float Ds = 10000;
        float ms = f / ( Ds - f );
        float N = 1.4f;
        float PPM = Numericf::sqrt( ( sceneFBO->getWidth() * sceneFBO->getWidth() ) + sceneFBO->getHeight() * sceneFBO->getHeight() ) / 35.0f;

        auto uBlurCoefficient = crimild::alloc< FloatUniform >( "uBlurCoefficient", ( f * ms / N ) ); // f * ms / N
        AssetManager::getInstance()->set( "dof/uniforms/uBlurCoefficient", uBlurCoefficient );
        dofBlurProgram->attachUniform( uBlurCoefficient );

        auto uFocusDistance = crimild::alloc< FloatUniform >( "uFocusDistance", Ds ); // Ds
        AssetManager::getInstance()->set( "dof/uniforms/uFocusDistance", uFocusDistance );
        dofBlurProgram->attachUniform( uFocusDistance );

        auto uNear = crimild::alloc< FloatUniform >( "uNear", 0.1f );
        AssetManager::getInstance()->set( "dof/uniforms/uNear", uNear );
        dofBlurProgram->attachUniform( uNear );

        auto uFar = crimild::alloc< FloatUniform >( "uFar", 1024.0f );
        AssetManager::getInstance()->set( "dof/uniforms/uFar", uFar );
        dofBlurProgram->attachUniform( uFar );

        auto uPPM = crimild::alloc< FloatUniform >( "uPPM", PPM );
        AssetManager::getInstance()->set( "dof/uniforms/uPPM", uPPM );
        dofBlurProgram->attachUniform( uPPM );
    }

    int resolution = 256;

    auto fboA = renderer->getFrameBuffer( "dof/fbo/fboA" );
    if ( fboA == nullptr ) {
        renderer->generateAuxFBO( "dof/fbo/fboA", resolution, resolution );
        fboA = renderer->getFrameBuffer( "dof/fbo/fboA" );
    }

    auto fboB = renderer->getFrameBuffer( "dof/fbo/fboB" );
    if ( fboB == nullptr ) {
        renderer->generateAuxFBO( "dof/fbo/fboB", resolution, resolution );
        fboB = renderer->getFrameBuffer( "dof/fbo/fboB" );
    }

    AssetManager::getInstance()->get< IntUniform >( "dof/uniforms/uOrientation" )->setValue( 0 );

    renderer->bindFrameBuffer( fboA );
    renderer->bindProgram( dofBlurProgram );
    renderer->bindTexture( dofBlurProgram->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), colorTarget->getTexture() );
    renderer->bindTexture( dofBlurProgram->getStandardLocation( ShaderProgram::StandardLocation::DEPTH_MAP_UNIFORM ), depthTarget->getTexture() );
        
    renderer->drawScreenPrimitive( dofBlurProgram );
        
    renderer->unbindTexture( dofBlurProgram->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), colorTarget->getTexture() );
    renderer->unbindTexture( dofBlurProgram->getStandardLocation( ShaderProgram::StandardLocation::DEPTH_MAP_UNIFORM ), depthTarget->getTexture() );
    renderer->unbindProgram( dofBlurProgram );
    renderer->unbindFrameBuffer( fboA );

    auto aColorTarget = fboA->getRenderTargets().get( Renderer::FBO_AUX_COLOR_TARGET_NAME );
    if ( aColorTarget == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot get color target from scene" );
        return;
    }

    AssetManager::getInstance()->get< IntUniform >( "dof/uniforms/uOrientation" )->setValue( 1 );

    renderer->bindFrameBuffer( fboB );
    renderer->bindProgram( dofBlurProgram );
    renderer->bindTexture( dofBlurProgram->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), aColorTarget->getTexture() );
    renderer->bindTexture( dofBlurProgram->getStandardLocation( ShaderProgram::StandardLocation::DEPTH_MAP_UNIFORM ), depthTarget->getTexture() );
        
    renderer->drawScreenPrimitive( dofBlurProgram );
        
    renderer->unbindTexture( dofBlurProgram->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), aColorTarget->getTexture() );
    renderer->unbindTexture( dofBlurProgram->getStandardLocation( ShaderProgram::StandardLocation::DEPTH_MAP_UNIFORM ), depthTarget->getTexture() );
    renderer->unbindProgram( dofBlurProgram );
    renderer->unbindFrameBuffer( fboB );   

    /*
    AssetManager::getInstance()->get< IntUniform >( "dof/uniforms/uOrientation" )->setValue( 1 );

    renderer->bindFrameBuffer( fboA );
    renderer->bindProgram( dofBlurProgram );
    renderer->bindTexture( dofBlurProgram->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), colorTarget->getTexture() );
        
    renderer->drawScreenPrimitive( dofBlurProgram );
        
    renderer->unbindTexture( dofBlurProgram->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), colorTarget->getTexture() );
    renderer->unbindProgram( dofBlurProgram );
    renderer->unbindFrameBuffer( fboA );   
    */

    /*
    // STEP 1: Downscale
    auto downscaleFBO = renderer->getFrameBuffer( Renderer::FBO_AUX_512 );
    if ( downscaleFBO == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot find FBO named '", Renderer::FBO_AUX_512, "'" );
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
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot find FBO named '", Renderer::FBO_AUX_1024, "'" );
        return;
    }

    colorTarget = downscaleFBO->getRenderTargets().get( Renderer::FBO_AUX_COLOR_TARGET_NAME );
    if ( colorTarget->getTexture() == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Color texture is null" );
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
    */
}

void DepthOfFieldImageEffect::apply( crimild::Renderer *renderer, crimild::Camera * )
{
    auto sceneFBO = renderer->getFrameBuffer( RenderPass::S_BUFFER_NAME );
    if ( sceneFBO == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot find FBO named '", RenderPass::S_BUFFER_NAME, "'" );
        return;
    }
    
    auto colorTarget = sceneFBO->getRenderTargets().get( RenderPass::S_BUFFER_COLOR_TARGET_NAME );
    if ( colorTarget->getTexture() == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Color texture is null" );
        return;
    }

    auto depthTarget = sceneFBO->getRenderTargets().get( RenderPass::S_BUFFER_DEPTH_TARGET_NAME );
    if ( depthTarget->getTexture() == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Depth texture is null" );
        return;
    }

    auto blurFBO = renderer->getFrameBuffer( "dof/fbo/fboB" );
    if ( blurFBO == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot find FBO named 'dof/fbo/fboB'" );
        return;
    }

    auto blurTarget = blurFBO->getRenderTargets().get( Renderer::FBO_AUX_COLOR_TARGET_NAME );
    if ( blurTarget->getTexture() == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Blur texture is null" );
        return;
    }

    auto program = renderer->getShaderProgram( "dof_composite" );
    if ( program == nullptr ) {
        auto tmp = crimild::alloc< ShaderProgram >( OpenGLUtils::getVertexShaderInstance( dof_generic_vs ), OpenGLUtils::getFragmentShaderInstance( dof_composite_fs ) );
        renderer->setShaderProgram( "dof_composite", tmp );
        program = crimild::get_ptr( tmp );

        program->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
        program->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );
        
        program->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM, "uColorMap" );
        program->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::DEPTH_MAP_UNIFORM, "uDepthMap" );
        program->registerLocation( crimild::alloc< ShaderLocation >( ShaderLocation::Type::UNIFORM, "uBlurMap" ) );
        
        float f = 0.2;
        float Ds = 10;
        float ms = f / ( Ds - f );
        float N = 1.4f;
        float PPM = Numericf::sqrt( ( sceneFBO->getWidth() * sceneFBO->getWidth() ) + sceneFBO->getHeight() * sceneFBO->getHeight() ) / 35.0f;

        auto uBlurCoefficient = crimild::alloc< FloatUniform >( "uBlurCoefficient", ( f * ms / N ) ); // f * ms / N
        AssetManager::getInstance()->set( "dof/uniforms/uBlurCoefficient", uBlurCoefficient );
        program->attachUniform( uBlurCoefficient );

        auto uFocusDistance = crimild::alloc< FloatUniform >( "uFocusDistance", Ds ); // Ds
        AssetManager::getInstance()->set( "dof/uniforms/uFocusDistance", uFocusDistance );
        program->attachUniform( uFocusDistance );

        auto uNear = crimild::alloc< FloatUniform >( "uNear", 0.1f );
        AssetManager::getInstance()->set( "dof/uniforms/uNear", uNear );
        program->attachUniform( uNear );

        auto uFar = crimild::alloc< FloatUniform >( "uFar", 1024.0f );
        AssetManager::getInstance()->set( "dof/uniforms/uFar", uFar );
        program->attachUniform( uFar );

        auto uPPM = crimild::alloc< FloatUniform >( "uPPM", PPM );
        AssetManager::getInstance()->set( "dof/uniforms/uPPM", uPPM );
        program->attachUniform( uPPM );
    }
    
    renderer->bindProgram( program );
    renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), colorTarget->getTexture() );
    renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::DEPTH_MAP_UNIFORM ), depthTarget->getTexture() );
    renderer->bindTexture( program->getLocation( "uBlurMap" ), blurTarget->getTexture() );

    renderer->drawScreenPrimitive( program );

    renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), colorTarget->getTexture() );
    renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::DEPTH_MAP_UNIFORM ), depthTarget->getTexture() );
    renderer->unbindTexture( program->getLocation( "uBlurMap" ), blurTarget->getTexture() );
    renderer->unbindProgram( program );

    /*
    auto blurFBO = renderer->getFrameBuffer( Renderer::FBO_AUX_1024 );
    if ( blurFBO == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot find FBO named '", Renderer::FBO_AUX_1024, "'" );
        return;
    }

    auto blurTarget = blurFBO->getRenderTargets().get( Renderer::FBO_AUX_COLOR_TARGET_NAME );
    if ( blurTarget->getTexture() == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Blur texture is null" );
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
    */
}

