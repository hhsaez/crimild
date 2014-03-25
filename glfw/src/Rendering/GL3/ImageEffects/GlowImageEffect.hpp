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

#ifndef CRIMILD_GL3_RENDERING_IMAGE_EFFECT_GLOW_
#define CRIMILD_GL3_RENDERING_IMAGE_EFFECT_GLOW_

#include <Crimild.hpp>

namespace crimild {
    
    namespace gl3 {
        
        class GlowImageEffect : public ImageEffect {
        public:
            GlowImageEffect( void );
            virtual ~GlowImageEffect( void );
            
            virtual void apply( crimild::Renderer *renderer, int inputCount, Texture **inputs, Primitive *primitive, FrameBufferObject *output ) override;
            
            void setGlowMapSize( int size ) { _glowMapSize = size; }
            int getGlowMapSize( void ) const { return _glowMapSize; }
            
            void setAmount( int value ) { _amount = value; }
            int getAmount( void ) const { return _amount; }
            
        private:
            void buildGlowBuffer( int width, int height );
            void computeGlow( crimild::Renderer *renderer, Texture *srcImage, Primitive *primitive );
            void applyGlow( crimild::Renderer *renderer, Texture *srcImage, Texture *glowMap, Primitive *primitive, FrameBufferObject *output );
            
            int _amount;
            int _glowMapSize;
            
            Pointer< FrameBufferObject > _glowMapBuffer;
            Pointer< Texture > _glowMap;
            
            Pointer< AlphaState > _alphaState;
            Pointer< DepthState > _depthState;
        };
        
    }
    
}

#endif

