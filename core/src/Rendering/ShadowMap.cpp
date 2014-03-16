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

#include "ShadowMap.hpp"
#include "SceneGraph/Light.hpp"

using namespace crimild;

ShadowMap::ShadowMap( Light *source, FrameBufferObject *fbo )
    : _source( source ),
      _buffer( fbo )
{
    if ( _buffer == nullptr ) {
        int width = 1024;
        int height = 1024;
        _buffer.set( new FrameBufferObject( width, height ) );
        _buffer->getRenderTargets().add( new RenderTarget( RenderTarget::Type::COLOR_RGBA, RenderTarget::Output::TEXTURE, width, height ) );
        _buffer->getRenderTargets().add( new RenderTarget( RenderTarget::Type::DEPTH_16, RenderTarget::Output::RENDER, width, height ) );
    }
    
    _buffer->getRenderTargets().each( [&]( RenderTarget *target, int ) {
        if ( target->getOutput() == RenderTarget::Output::TEXTURE || target->getOutput() == RenderTarget::Output::RENDER_AND_TEXTURE ) {
            _texture.set( target->getTexture() );
        }
    });
    
    computeLinearDepthConstant( source->getShadowNearCoeff(), source->getShadowFarCoeff() );
}

ShadowMap::~ShadowMap( void )
{
    
}

