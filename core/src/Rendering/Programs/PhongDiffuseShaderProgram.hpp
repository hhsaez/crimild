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

#ifndef CRIMILD_CORE_RENDERING_PROGRAMS_PHONG_DIFFUSE_
#define CRIMILD_CORE_RENDERING_PROGRAMS_PHONG_DIFFUSE_

#include "Rendering/ShaderProgram.hpp"
#include "Rendering/ShaderUniformImpl.hpp"

namespace crimild {

	/**
	   \brief Computes diffuse term for Phong shading model in view space

	   \remarks Works in View-space
	 */
	class PhongDiffuseShaderProgram : public ShaderProgram {
	public:
		PhongDiffuseShaderProgram( void );
		virtual ~PhongDiffuseShaderProgram( void );

	public:
		void bindLightColor( const RGBAColorf &value ) { _lightColor->setValue( value); }

		/**
		   \remarks Light direction must be provided in view space
		   (premultiplied by V matrix).

		   It is stored as inverted since it's the direction from 
		   the fragment to the light source.
		 */
		void bindLightDirection( const Vector3f &value ) { _lightDirection->setValue( ( -value ).getNormalized() ); }
		void bindNormals( Texture *value ) { _normals->setValue( value ); }

	private:
		SharedPointer< RGBAColorfUniform > _lightColor;
		SharedPointer< Vector3fUniform > _lightDirection;
		SharedPointer< TextureUniform > _normals;
		
	private:
		void createVertexShader( void );
		void createFragmentShader( void );
	};

}

#endif
