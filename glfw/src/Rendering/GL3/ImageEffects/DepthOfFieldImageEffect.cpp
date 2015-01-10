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
#include "Rendering/GL3/Utils.hpp"

using namespace crimild;

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

    uniform sampler2D uColorMap; 		// Colour texture
    uniform sampler2D uDepthMap; 		// Depth texture
    
    uniform vec2 uTexelSize; 			// Size of one texel (1 / width, 1 / height)
    uniform int uOrientation; 			// 0 = horizontal, 1 = vertical
    uniform float uBlurCoefficient; 	// Calculated from the blur equation, b = ( f * ms / N )
    uniform float uFocusDistance; 		// The distance to the subject in perfect focus (= Ds)
    uniform float uNear; 				// Near clipping plane
    uniform float uFar; 				// Far clipping plane
    uniform float uPPM; 				// Pixels per millimetre

    out vec4 vFragColor;

    /// <summary>
    /// Calculate the blur diameter to apply on the image.
    /// b = (f ms / N) (xd / (Ds +- xd))
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
        float depth = texture( uDepthMap, vTextureCoord ).r;
        float blurAmount = GetBlurDiameter( depth );
        blurAmount = min( floor( blurAmount ), MAX_BLUR_RADIUS );

        // Apply the blur
        float count = 0.0;
        vec4 color = vec4( 0.0 );
        vec2 texelOffset = uOrientation == 0 ? vec2( uTexelSize.x, 0.0 ) : vec2( 0.0, uTexelSize.y );

        if ( blurAmount >= 1.0 ) {
            float halfBlur = 0.5 * blurAmount;
            for ( float i = 0.0; i < MAX_BLUR_RADIUS; i++ ) {
                if ( i >= blurAmount ) break;

                float offset = i - halfBlur;
                vec2 vOffset = vTextureCoord + ( texelOffset * offset );

                color += texture( uColorMap, vOffset );
                ++count;
            }
        }

        vFragColor = count > 0.0 ? ( color / count ) : texture( uColorMap, vTextureCoord );
        vFragColor.a = 0.5;
    }
)};

const char *dof_blur2_fs = { CRIMILD_TO_STRING(
                                           
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

const char *dof_apply_vs = { CRIMILD_TO_STRING(
    in vec3 aPosition;
    in vec2 aTextureCoord;

    out vec2 vTextureCoord;

    void main()
    {
        vTextureCoord = aTextureCoord;
        gl_Position = vec4( aPosition.x, aPosition.y, 0.0, 1.0 );
    }
)};

const char *dof_apply_fs = { CRIMILD_TO_STRING(
	in vec2 vTextureCoord;

	uniform sampler2D uColorMap; // Colour texture
	uniform sampler2D uDepthMap; // Depth texture
	uniform sampler2D uBlurMap; // Blurred texture

	uniform float uBlurCoefficient; // Calculated from the blur equation, b = ( f * ms / N )
	uniform float uFocusDistance; // The distance to the subject in perfect focus (= Ds)
	uniform float uNear; // Near clipping plane
	uniform float uFar; // Far clipping plane
	uniform float uPPM; // Pixels per millimetre

    out vec4 vFragColor;

	/// <summary>
	/// Calculate the blur diameter to apply on the image.
	/// b = (f ms / N) (xd / (Ds +- xd))
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

		// Get the colour, depth, and blur pixels
		vec4 color = texture( uColorMap, vTextureCoord );
		float depth = texture( uDepthMap, vTextureCoord ).r;
		vec4 blur = texture( uBlurMap, vTextureCoord );

		// Linearly interpolate between the colour and blur pixels based on DOF
		float blurAmount = GetBlurDiameter( depth );
		float lerp = min( blurAmount / MAX_BLUR_RADIUS, 1.0 );

		// Blend
		vFragColor = ( color * ( 1.0 - lerp ) ) + ( blur * lerp );
	}
)};

gl3::DepthOfFieldImageEffect::DepthOfFieldImageEffect( void )
{
    _dofBlurMapSize = Vector2f( 256.0f, 256.0f );
    _blurMapTexelSize = std::make_shared< Vector2fUniform >( "uTexelSize", Vector2f( 1.0f / _dofBlurMapSize[ 0 ], 1.0f / _dofBlurMapSize[ 1 ] ) );
    _blurOrientation = std::make_shared< IntUniform >( "uOrientation", 0 );
    _blurCoefficient = std::make_shared< FloatUniform >( "uBlurCoefficient", 1.0f );
    _focusDistance = std::make_shared< FloatUniform >( "uFocusDistance", 0.5f );
    _near = std::make_shared< FloatUniform >( "uNear", 1.0f );
    _far = std::make_shared< FloatUniform >( "uFar", 1.0f );
    _ppm = std::make_shared< FloatUniform >( "uPPM", 1.0f );
    
    _focus = std::make_shared< FloatUniform >( "uFocus", 0.875f );
    _aperture = std::make_shared< FloatUniform >( "uAperture", 0.1f );
    
    _alphaState = std::make_shared< AlphaState >( true, AlphaState::SrcBlendFunc::ONE, AlphaState::DstBlendFunc::ZERO );
}

gl3::DepthOfFieldImageEffect::~DepthOfFieldImageEffect( void )
{

}

void gl3::DepthOfFieldImageEffect::apply( crimild::RendererPtr const &renderer )
{
    const float blurCoefficient = 1.0f;
    const float focusDistance = 0.5f;
    const float near = 1.0f;
    const float far = 1.0f;
    const float ppm = 1.0f;
    
	auto sceneFBO = renderer->getFrameBuffer( "scene" );
	if ( sceneFBO == nullptr ) {
		Log::Error << "Cannot find FBO named 'scene'" << Log::End;
		return;
	}

    auto dofBlurProgram = renderer->getShaderProgram( "dof_blur" );
	if ( dofBlurProgram == nullptr ) {
		dofBlurProgram = std::make_shared< ShaderProgram >( Utils::getVertexShaderInstance( dof_blur_vs ), Utils::getFragmentShaderInstance( dof_blur2_fs ) );

		dofBlurProgram->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
	    dofBlurProgram->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );
	    
	    dofBlurProgram->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM, "uColorMap" );
	    dofBlurProgram->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::DEPTH_MAP_UNIFORM, "uDepthMap" );
        
        dofBlurProgram->attachUniform( _focus );
        dofBlurProgram->attachUniform( _aperture );

//	    dofBlurProgram->attachUniform( std::make_shared< Vector2fUniform >( "uTexelSize", Vector2f( 1.0f / _dofBlurMapSize[ 0 ], 1.0f / _dofBlurMapSize[ 1 ] ) ) );
//	    dofBlurProgram->attachUniform( _blurOrientation );
//	    dofBlurProgram->attachUniform( std::make_shared< FloatUniform >( "uBlurCoefficient", blurCoefficient ) );
//	    dofBlurProgram->attachUniform( std::make_shared< FloatUniform >( "uFocusDistance", focusDistance ) );
//	    dofBlurProgram->attachUniform( std::make_shared< FloatUniform >( "uNear", near ) );
//	    dofBlurProgram->attachUniform( std::make_shared< FloatUniform >( "uFar", far ) );
//	    dofBlurProgram->attachUniform( std::make_shared< FloatUniform >( "uPPM", ppm ) );
		
		renderer->addShaderProgram( "dof_blur", dofBlurProgram );
	}

    auto dofApplyProgram = renderer->getShaderProgram( "dof_apply" );
	if ( dofApplyProgram == nullptr ) {
		dofApplyProgram = std::make_shared< ShaderProgram >( Utils::getVertexShaderInstance( dof_apply_vs ), Utils::getFragmentShaderInstance( dof_apply_fs ) );

		dofApplyProgram->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
	    dofApplyProgram->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );

	    dofApplyProgram->registerLocation( std::make_shared< ShaderLocation >( ShaderLocation::Type::UNIFORM, "uColorMap" ) );
	    dofApplyProgram->registerLocation( std::make_shared< ShaderLocation >( ShaderLocation::Type::UNIFORM, "uDepthMap" ) );
	    dofApplyProgram->registerLocation( std::make_shared< ShaderLocation >( ShaderLocation::Type::UNIFORM, "uBlurMap" ) );

	    dofApplyProgram->attachUniform( std::make_shared< FloatUniform >( "uBlurCoefficient", blurCoefficient ) );
	    dofApplyProgram->attachUniform( std::make_shared< FloatUniform >( "uFocusDistance", focusDistance ) );
	    dofApplyProgram->attachUniform( std::make_shared< FloatUniform >( "uNear", near ) );
	    dofApplyProgram->attachUniform( std::make_shared< FloatUniform >( "uFar", far ) );
	    dofApplyProgram->attachUniform( std::make_shared< FloatUniform >( "uPPM", ppm ) );
		
		renderer->addShaderProgram( "dof_apply", dofApplyProgram );
	}

    auto dofBlurFBO = renderer->getFrameBuffer( "dof_blur" );
	if ( dofBlurFBO == nullptr ) {
		int width = _dofBlurMapSize[ 0 ];
		int height = _dofBlurMapSize[ 1 ];

	    dofBlurFBO = std::make_shared< FrameBufferObject >( width, height );
	    dofBlurFBO->getRenderTargets()->add( "blur", std::make_shared< RenderTarget >( RenderTarget::Type::COLOR_RGBA, RenderTarget::Output::TEXTURE, width, height ) );
	    dofBlurFBO->getRenderTargets()->add( "depth", std::make_shared< RenderTarget >( RenderTarget::Type::DEPTH_24, RenderTarget::Output::RENDER, width, height ) );

	    renderer->addFrameBuffer( "dof_blur", dofBlurFBO );
	}

	auto colorTarget = sceneFBO->getRenderTargets()->get( "color" );
	auto depthTarget = sceneFBO->getRenderTargets()->get( "depth" );
	auto blurTarget = dofBlurFBO->getRenderTargets()->get( "blur" );

//	renderer->bindFrameBuffer( dofBlurFBO );
		renderer->bindProgram( dofBlurProgram );
		renderer->bindTexture( dofBlurProgram->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), colorTarget->getTexture() );
		renderer->bindTexture( dofBlurProgram->getStandardLocation( ShaderProgram::StandardLocation::DEPTH_MAP_UNIFORM ), depthTarget->getTexture() );
		
//		_blurOrientation->setValue( 0 );
		renderer->drawScreenPrimitive( dofBlurProgram );
		
//		renderer->setAlphaState( _alphaState );
    
//		_blurOrientation->setValue( 0 );
//		renderer->drawScreenPrimitive( dofBlurProgram );
    
//        renderer->setAlphaState( AlphaState::DISABLED );
		
		renderer->unbindTexture( dofBlurProgram->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), colorTarget->getTexture() );
		renderer->unbindTexture( dofBlurProgram->getStandardLocation( ShaderProgram::StandardLocation::DEPTH_MAP_UNIFORM ), depthTarget->getTexture() );
		renderer->unbindProgram( dofBlurProgram );
//	renderer->unbindFrameBuffer( dofBlurFBO );

//	renderer->bindProgram( dofApplyProgram );
//    
//	renderer->bindTexture( dofApplyProgram->getLocation( "uColorMap" ), colorTarget->getTexture() );
//	renderer->bindTexture( dofApplyProgram->getLocation( "uDepthMap" ), depthTarget->getTexture() );
//	renderer->bindTexture( dofApplyProgram->getLocation( "uBlurMap" ), blurTarget->getTexture() );
	
//	renderer->setAlphaState( AlphaState::DISABLED );
//	renderer->drawScreenPrimitive( dofApplyProgram );
	
//	renderer->unbindTexture( dofApplyProgram->getLocation( "uColorMap" ), colorTarget->getTexture() );
//	renderer->unbindTexture( dofApplyProgram->getLocation( "uDepthMap" ), depthTarget->getTexture() );
//	renderer->unbindTexture( dofApplyProgram->getLocation( "uBlurMap" ), blurTarget->getTexture() );
//
//	renderer->unbindProgram( dofApplyProgram );
}

