/*
 * Copyright (c) 2002-present, H. Hernan Saez
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

#ifndef CRIMILD_CORE_RENDERING_PROGRAMS_PHONG_SPECULAR_
#define CRIMILD_CORE_RENDERING_PROGRAMS_PHONG_SPECULAR_

#include "Rendering/ShaderProgram.hpp"
#include "Rendering/ShaderUniformImpl.hpp"

namespace crimild {

	class PhongSpecularShaderProgram : public ShaderProgram {
	public:
		PhongSpecularShaderProgram( void );
		virtual ~PhongSpecularShaderProgram( void );

	public:
		void bindLightColor( const RGBAColorf &value ) { _lightColor->setValue( value); }

		/**
		   \remarks Light direction must be provided in view space
		   (premultiplied by V matrix).

		   It is stored as inverted since it's the direction from 
		   the fragment to the light source.
		 */
		void bindLightDirection( const Vector3f &value ) { _lightDirection->setValue( ( -value ).getNormalized() ); }

		void bindInvProjMatrix( const Matrix4f &value ) { _invProjMatrix->setValue( value ); }
		
		void bindNormalTexture( Texture *value ) { _normalTexture->setValue( value ); }
		void bindDepthTexture( Texture *value) { _depthTexture->setValue( value ); }

	private:
		SharedPointer< RGBAColorfUniform > _lightColor;
		SharedPointer< Vector3fUniform > _lightDirection;
		SharedPointer< Matrix4fUniform > _invProjMatrix;
		SharedPointer< TextureUniform > _normalTexture;
		SharedPointer< TextureUniform > _depthTexture;
		
	private:
		void createVertexShader( void );
		void createFragmentShader( void );
	};

}

#endif
