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

#include "ImageEffect.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/FrameBufferObject.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Foundation/Log.hpp"
#include "Primitives/Primitive.hpp"

using namespace crimild;

ImageEffect::ImageEffect( void )
{

}

ImageEffect::~ImageEffect( void )
{

}

FrameBufferObject *ImageEffect::getFrameBuffer( Renderer *renderer, std::string name )
{
    auto fbo = renderer->getFrameBuffer( name );
    if ( fbo == nullptr ) {
        int width = renderer->getScreenBuffer()->getWidth();
        int height = renderer->getScreenBuffer()->getHeight();
        
        if ( name == FBO_AUX_HALF_RES_1 || name == FBO_AUX_HALF_RES_2 ) {
            width /= 2;
            height /= 2;
        }
        else if ( name == FBO_AUX_QUARTER_RES_1 || name == FBO_AUX_QUARTER_RES_2 ) {
            width /= 4;
            height /= 4;
        }
        
        auto newFBO = std::move( crimild::alloc< FrameBufferObject >( width, height ) );
        renderer->addFrameBuffer( name, newFBO );
        fbo = crimild::get_ptr( newFBO );
        
        fbo->getRenderTargets().add( "depth", crimild::alloc< RenderTarget >( RenderTarget::Type::DEPTH_24, RenderTarget::Output::RENDER, width, height ) );
        fbo->getRenderTargets().add( "color", crimild::alloc< RenderTarget >( RenderTarget::Type::COLOR_RGBA, RenderTarget::Output::TEXTURE, width, height ) );
    }
    
    return fbo;
}

void ImageEffect::renderScreen( Renderer *renderer, Texture *texture )
{
    auto program = renderer->getShaderProgram( "screen" );
    if ( program == nullptr ) {
        Log::Error << "No shader program provided with name 'texture'" << Log::End;
        return;
    }
    
    renderer->bindProgram( program );
    renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_COLOR_MAP_UNIFORM ), texture );
    
    Matrix4f mMatrix;
    mMatrix.makeIdentity();
    renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM ), mMatrix );
    
    renderer->drawScreenPrimitive( program );
    
    renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_COLOR_MAP_UNIFORM ), texture );
    renderer->unbindProgram( program );
}

