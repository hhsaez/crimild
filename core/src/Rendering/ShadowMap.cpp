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
#include "FrameBufferObject.hpp"
#include "RenderTarget.hpp"
#include "Texture.hpp"

#include "Simulation/Simulation.hpp"

using namespace crimild;

ShadowMap::ShadowMap( void )
    : ShadowMap( nullptr )
{
    
}

ShadowMap::ShadowMap( SharedPointer< FrameBufferObject > const &fbo )
    : _buffer( fbo ),
	  _viewport( Vector4f::ZERO )
{
    if ( _buffer == nullptr ) {
        auto width = Simulation::getInstance()->getSettings()->get< crimild::Int16 >( Settings::SETTINGS_RENDERING_SHADOWS_RESOLUTION_WIDTH, 1024 );
        auto height = Simulation::getInstance()->getSettings()->get< crimild::Int16 >( Settings::SETTINGS_RENDERING_SHADOWS_RESOLUTION_HEIGHT, 1024 );

        _buffer = crimild::alloc< FrameBufferObject >( width, height );
        _buffer->setClearColor( RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ) );
        _buffer->getRenderTargets().insert( "depth", crimild::alloc< RenderTarget >( RenderTarget::Type::DEPTH_24, RenderTarget::Output::RENDER_AND_TEXTURE, width, height, true ) );
    }
    
    _buffer->getRenderTargets().each( [ this ]( const std::string &name, SharedPointer< RenderTarget > &target ) {
        if ( target->getOutput() == RenderTarget::Output::RENDER_AND_TEXTURE ) {
            _texture = target->getTexture();
        }
    });
}

ShadowMap::~ShadowMap( void )
{
    
}

