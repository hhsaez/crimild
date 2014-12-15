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

void ImageEffect::render( std::shared_ptr< Renderer > const &renderer, std::shared_ptr< FrameBufferObject > const &output, std::shared_ptr< Texture > const &texture, std::shared_ptr< ShaderProgram > const &program, std::shared_ptr< Primitive > const &primitive )
{
    if ( program == nullptr ) {
        Log::Error << "Invalid program object" << Log::End;
        return;
    }
    
    if ( texture == nullptr ) {
        Log::Error << "Invalid texture object" << Log::End;
        return;
    }
    
    renderer->bindFrameBuffer( output );
    renderer->bindProgram( program );
    renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_COLOR_MAP_UNIFORM ), texture );
    renderer->bindVertexBuffer( program, primitive->getVertexBuffer() );
    renderer->bindIndexBuffer( program, primitive->getIndexBuffer() );
    
    renderer->drawPrimitive( program, primitive );
    
    renderer->unbindVertexBuffer( program, primitive->getVertexBuffer() );
    renderer->unbindIndexBuffer( program, primitive->getIndexBuffer() );
    renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_COLOR_MAP_UNIFORM ), texture );
    renderer->unbindProgram( program );
    renderer->unbindFrameBuffer( output );
}

