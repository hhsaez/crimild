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

#ifndef CRIMILD_GLES_RENDERING_RENDERER_
#define CRIMILD_GLES_RENDERING_RENDERER_

#include <Crimild.hpp>

#include <map>
#include <string>

namespace crimild {
    
    namespace gles {
    
        class Renderer : public crimild::Renderer {
        public:
            Renderer( crimild::FrameBufferObject *screenBuffer );
            virtual ~Renderer( void );
            
            virtual void configure( void ) override;
            
            virtual void beginRender( void ) override;
            
            virtual void endRender( void ) override;
            
            virtual void clearBuffers( void ) override;
            
        public:
            virtual void bindUniform( crimild::ShaderLocation *location, int value ) override;
            virtual void bindUniform( crimild::ShaderLocation *location, float value ) override;
            virtual void bindUniform( crimild::ShaderLocation *location, const crimild::Vector3f &vector ) override;
            virtual void bindUniform( crimild::ShaderLocation *location, const crimild::RGBAColorf &color ) override;
            virtual void bindUniform( crimild::ShaderLocation *location, const crimild::Matrix4f &matrix ) override;
            
            virtual void setDepthState( DepthState *state );
            virtual void setAlphaState( AlphaState *state );
            
            virtual void drawPrimitive( crimild::ShaderProgram *program, crimild::Primitive *primitive ) override;
            
            virtual crimild::ShaderProgram *getForwardPassProgram( void ) override;
            
            virtual crimild::ShaderProgram *getFallbackProgram( crimild::Material *material, Geometry *geometry, crimild::Primitive *primitive ) override;
            
        private:
            std::map< std::string, Pointer< crimild::ShaderProgram > > _fallbackPrograms;
        };
        
        typedef SharedPointer< Renderer > RendererPtr;
        
    }
    
}

#endif

