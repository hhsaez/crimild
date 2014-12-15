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

#ifndef CRIMILD_GL3_RENDERER_RENDERER_
#define CRIMILD_GL3_RENDERER_RENDERER_

#include <Crimild.hpp>

namespace crimild {

	namespace gl3 {

		class Renderer : public crimild::Renderer {
		public:
			Renderer( FrameBufferObjectPtr const &screenBuffer );
			virtual ~Renderer( void );

			virtual void configure( void ) override;

			virtual void beginRender( void ) override;
			
			virtual void endRender( void ) override;

			virtual void clearBuffers( void ) override;

		public:
			virtual void bindUniform( ShaderLocationPtr const &location, int value ) override;
			virtual void bindUniform( ShaderLocationPtr const &location, float value ) override;
			virtual void bindUniform( ShaderLocationPtr const &location, const Vector3f &vector ) override;
			virtual void bindUniform( ShaderLocationPtr const &location, const RGBAColorf &color ) override;
			virtual void bindUniform( ShaderLocationPtr const &location, const Matrix4f &matrix ) override;

			virtual void setDepthState( DepthStatePtr const &state ) override;
			virtual void setAlphaState( AlphaStatePtr const &state ) override;

			virtual void drawPrimitive( ShaderProgramPtr const &program, PrimitivePtr const &primitive ) override;
			virtual void drawBuffers( ShaderProgramPtr const &program, Primitive::Type type, VertexBufferObjectPtr const &vbo, unsigned int count ) override;

            virtual ShaderProgramPtr getDepthProgram( void ) override;
            virtual ShaderProgramPtr getForwardPassProgram( void ) override;
            virtual ShaderProgramPtr getDeferredPassProgram( void ) override;
            virtual ShaderProgramPtr getShaderProgram( const char *name ) override;
			virtual ShaderProgramPtr getFallbackProgram( MaterialPtr const &material, GeometryPtr const &geometry, PrimitivePtr const &primitive ) override;

		private:
			std::map< std::string, ShaderProgramPtr > _programs;
		};

	}

}

#endif

