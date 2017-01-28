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

#include "BlurImageEffect.hpp"

#include "Rendering/OpenGLUtils.hpp"

using namespace crimild;
using namespace crimild::opengl;
        
const char *blur_vs = R"(
    CRIMILD_GLSL_ATTRIBUTE vec3 aPosition;
    CRIMILD_GLSL_ATTRIBUTE vec2 aTextureCoord;

    CRIMILD_GLSL_VARYING_OUT vec2 vTextureCoord;

    void main()
    {
        vTextureCoord = aTextureCoord;
        CRIMILD_GLSL_VERTEX_OUTPUT = vec4( aPosition, 1.0 );
    }    
)";

const char *blur_fs = R"(
    CRIMILD_GLSL_PRECISION_FLOAT_HIGH

    CRIMILD_GLSL_VARYING_IN vec2 vTextureCoord;

    uniform sampler2D uColorMap;
    uniform int uResolution;
    uniform float uRadius;
    uniform vec2 uDirection;

    CRIMILD_GLSL_DECLARE_FRAGMENT_OUTPUT

    void main( void ) 
    {
        //CRIMILD_GLSL_FRAGMENT_OUTPUT = CRIMILD_GLSL_FN_TEXTURE_2D( uColorMap, vTextureCoord );

        vec4 sum = vec4(0.0);

        vec2 tc = vTextureCoord;

        float blur = uRadius / float( uResolution );

        float hstep = uDirection.x;
        float vstep = uDirection.y;

        sum += CRIMILD_GLSL_FN_TEXTURE_2D( uColorMap, vec2( tc.x - 4.0 * blur * hstep, tc.y - 4.0 * blur * vstep ) ) * 0.0162162162;
        sum += CRIMILD_GLSL_FN_TEXTURE_2D( uColorMap, vec2( tc.x - 3.0 * blur * hstep, tc.y - 3.0 * blur * vstep ) ) * 0.0540540541;
        sum += CRIMILD_GLSL_FN_TEXTURE_2D( uColorMap, vec2( tc.x - 2.0 * blur * hstep, tc.y - 2.0 * blur * vstep ) ) * 0.1216216216;
        sum += CRIMILD_GLSL_FN_TEXTURE_2D( uColorMap, vec2( tc.x - 1.0 * blur * hstep, tc.y - 1.0 * blur * vstep ) ) * 0.1945945946;
        sum += CRIMILD_GLSL_FN_TEXTURE_2D( uColorMap, vec2( tc.x, tc.y ) ) * 0.2270270270;
        sum += CRIMILD_GLSL_FN_TEXTURE_2D( uColorMap, vec2( tc.x + 1.0 * blur * hstep, tc.y + 1.0 * blur * vstep ) ) * 0.1945945946;
        sum += CRIMILD_GLSL_FN_TEXTURE_2D( uColorMap, vec2( tc.x + 2.0 * blur * hstep, tc.y + 2.0 * blur * vstep ) ) * 0.1216216216;
        sum += CRIMILD_GLSL_FN_TEXTURE_2D( uColorMap, vec2( tc.x + 3.0 * blur * hstep, tc.y + 3.0 * blur * vstep ) ) * 0.0540540541;
        sum += CRIMILD_GLSL_FN_TEXTURE_2D( uColorMap, vec2( tc.x + 4.0 * blur * hstep, tc.y + 4.0 * blur * vstep ) ) * 0.0162162162;

        CRIMILD_GLSL_FRAGMENT_OUTPUT = vec4( sum.rgb, 1.0 );
    }
)";

BlurImageEffect::BlurImageEffect( float radius, size_t resolution )
    : _radius( crimild::alloc< FloatUniform >( "uRadius", radius ) ),
      _resolution( crimild::alloc< IntUniform >( "uResolution", resolution ) ),
      _direction( crimild::alloc< Vector2fUniform >( "uDirection", Vector2f::ZERO ) )
{

}

BlurImageEffect::~BlurImageEffect( void )
{

}

void BlurImageEffect::compute( crimild::Renderer *renderer, Camera *camera )
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

    auto program = renderer->getShaderProgram( "shaders/blur" );
    if ( program == nullptr ) {
        auto tmp = crimild::alloc< ShaderProgram >( OpenGLUtils::getVertexShaderInstance( blur_vs ), OpenGLUtils::getFragmentShaderInstance( blur_fs ) );
        renderer->setShaderProgram( "shaders/blur", tmp );
        program = crimild::get_ptr( tmp );

        program->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
        program->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );
        
        program->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM, "uColorMap" );

        program->attachUniform( _radius );
        program->attachUniform( _resolution );
        program->attachUniform( _direction );
    }

    auto fbo = renderer->getFrameBuffer( "fbos/blur" );
    if ( fbo == nullptr ) {
        int width = getResolution();
        int height = getResolution();

        auto tmp = crimild::alloc< FrameBufferObject >( width, height );
        renderer->setFrameBuffer( "fbos/blur", tmp );
        fbo = crimild::get_ptr( tmp );

        fbo->getRenderTargets().add( Renderer::FBO_AUX_DEPTH_TARGET_NAME, crimild::alloc< RenderTarget >( RenderTarget::Type::DEPTH_24, RenderTarget::Output::RENDER, width, height ) );
        fbo->getRenderTargets().add( Renderer::FBO_AUX_COLOR_TARGET_NAME, crimild::alloc< RenderTarget >( RenderTarget::Type::COLOR_RGBA, RenderTarget::Output::TEXTURE, width, height ) );
    }

    _direction->setValue( Vector2f::UNIT_X );

    renderer->bindFrameBuffer( fbo );
    renderer->bindProgram( program );
    renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), colorTarget->getTexture() );
    renderer->drawScreenPrimitive( program );
    renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), colorTarget->getTexture() );
    renderer->unbindProgram( program );
    renderer->unbindFrameBuffer( fbo );   
}

void BlurImageEffect::apply( crimild::Renderer *renderer, crimild::Camera *camera )
{
    _direction->setValue( Vector2f::UNIT_Y );

    auto fbo = renderer->getFrameBuffer( "fbos/blur" );
    if ( fbo == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot find FBO named 'fbos/blur'" );
        return;
    }

    auto colorTarget = fbo->getRenderTargets().get( Renderer::FBO_AUX_COLOR_TARGET_NAME );
    if ( colorTarget == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot get color target from scene" );
        return;
    }

    auto program = renderer->getShaderProgram( "shaders/blur" );
    if ( program == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot get blur shader program" );
        return;
    }

    renderer->bindProgram( program );
    renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), colorTarget->getTexture() );
    renderer->drawScreenPrimitive( program );
    renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), colorTarget->getTexture() );
    renderer->unbindProgram( program );
}

