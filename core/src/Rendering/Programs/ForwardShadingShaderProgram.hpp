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

#ifndef CRIMILD_CORE_RENDERING_PROGRAMS_FORWARD_SHADING_
#define CRIMILD_CORE_RENDERING_PROGRAMS_FORWARD_SHADING_

#include "Rendering/ShaderProgram.hpp"
#include "Rendering/ShaderUniformImpl.hpp"

namespace crimild {

	class Material;
	class Light;

	class ForwardShadingShaderProgram : public ShaderProgram {
	private:
		struct LightUniforms {
			SharedPointer< RGBAColorfUniform > ambientColor;
			SharedPointer< RGBAColorfUniform > diffuseColor;
			SharedPointer< Vector3fUniform > direction;
		};

		using LightArray = containers::Array< Light * >;
		using LightUniformArray = containers::Array< LightUniforms >;
		
	public:
		ForwardShadingShaderProgram( crimild::Size maxLights = 4 );
		virtual ~ForwardShadingShaderProgram( void );

		void bindModelMatrix( const Matrix4f &value ) { _modelMatrix->setValue( value ); }
		void bindNormalMatrix( const Matrix3f &value ) { _normalMatrix->setValue( value ); }
		void bindViewMatrix( const Matrix4f &value ) { _viewMatrix->setValue( value ); }
		void bindProjMatrix( const Matrix4f &value ) { _projMatrix->setValue( value ); }

		void bindMaterial( Material *material );

		void bindLight( Light *light, crimild::Size index );

	private:
		void createVertexShader( void );
		void createFragmentShader( void );

	private:
		SharedPointer< Matrix4fUniform > _modelMatrix;
		SharedPointer< Matrix3fUniform > _normalMatrix;
		SharedPointer< Matrix4fUniform > _viewMatrix;
		SharedPointer< Matrix4fUniform > _projMatrix;

		SharedPointer< RGBAColorfUniform > _matAmbient;

		SharedPointer< RGBAColorfUniform > _matDiffuse;
		SharedPointer< TextureUniform > _matDiffuseMap;
		
		SharedPointer< RGBAColorfUniform > _matSpecular;
		SharedPointer< TextureUniform > _matSpecularMap;
		SharedPointer< FloatUniform > _matShininess;

		containers::Array< LightUniforms > _lights;
	};

}

#endif

