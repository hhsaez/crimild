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

#include "ColorTintImageEffect.hpp"

#include "Foundation/Log.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/FrameBufferObject.hpp"
#include "Rendering/RenderPasses/RenderPass.hpp"

using namespace crimild;

constexpr const char *ColorTintImageEffect::COLOR_TINT_PROGRAM_NAME;
constexpr const char *ColorTintImageEffect::COLOR_TINT_UNIFORM_TINT;
constexpr const char *ColorTintImageEffect::COLOR_TINT_UNIFORM_TINT_VALUE;

const RGBAColorf ColorTintImageEffect::TINT_RED = RGBAColorf( 1.0f, 0.0f, 0.0f, 1.0f );
const RGBAColorf ColorTintImageEffect::TINT_GREEN = RGBAColorf( 0.0f, 1.0f, 0.0f, 1.0f );
const RGBAColorf ColorTintImageEffect::TINT_BLUE = RGBAColorf( 0.0f, 0.0f, 1.0f, 1.0f );
const RGBAColorf ColorTintImageEffect::TINT_SEPIA = RGBAColorf( 0.4392156863f, 0.2588235294f, 0.07843137255f, 1.0f );

ColorTintImageEffect::ColorTintImageEffect( const RGBAColorf &tint, float value )
    : _tint( crimild::alloc< RGBAColorfUniform >( ColorTintImageEffect::COLOR_TINT_UNIFORM_TINT, tint ) ),
      _tintValue( crimild::alloc< FloatUniform >( ColorTintImageEffect::COLOR_TINT_UNIFORM_TINT_VALUE, value ) )
{
    
}

ColorTintImageEffect::~ColorTintImageEffect( void )
{

}

void ColorTintImageEffect::compute( Renderer *renderer, Camera *camera )
{
    // do nothing
}

void ColorTintImageEffect::apply( crimild::Renderer *renderer, crimild::Camera * )
{
	if ( _colorTintProgram == nullptr ) {
		_colorTintProgram = renderer->getShaderProgram( COLOR_TINT_PROGRAM_NAME );
		if ( _colorTintProgram == nullptr ) {
            Log::warning( CRIMILD_CURRENT_CLASS_NAME, "No shader program found with name ", COLOR_TINT_PROGRAM_NAME );
			setEnabled( false );
			return;
		}

		_colorTintProgram->attachUniform( _tint );
        _colorTintProgram->attachUniform( _tintValue );
	}

    auto sceneFBO = renderer->getFrameBuffer( RenderPass::S_BUFFER_NAME );
	if ( sceneFBO == nullptr ) {
        Log::warning( CRIMILD_CURRENT_CLASS_NAME, "Cannot find FBO named '", RenderPass::S_BUFFER_NAME, "'" );
		setEnabled( false );
		return;
	}
    
    auto colorTarget = sceneFBO->getRenderTargets()[ RenderPass::S_BUFFER_COLOR_TARGET_NAME ];
	if ( colorTarget == nullptr ) {
        Log::warning( CRIMILD_CURRENT_CLASS_NAME, "Cannot find render target named '", RenderPass::S_BUFFER_COLOR_TARGET_NAME, "'" );
		setEnabled( false );
		return;		
	}

    renderer->bindProgram( _colorTintProgram );
    renderer->bindTexture( _colorTintProgram->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), colorTarget->getTexture() );
		
    renderer->drawScreenPrimitive( _colorTintProgram );
		
    renderer->unbindTexture( _colorTintProgram->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), colorTarget->getTexture() );
    renderer->unbindProgram( _colorTintProgram );
}

