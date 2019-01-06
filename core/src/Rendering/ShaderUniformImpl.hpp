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

#ifndef CRIMILD_RENDERING_SHADER_UNIFORM_IMPL_
#define CRIMILD_RENDERING_SHADER_UNIFORM_IMPL_

#include "ShaderUniform.hpp"
#include "Renderer.hpp"
#include "Mathematics/Matrix.hpp"
#include "Mathematics/Vector.hpp"

namespace crimild {

	class Texture;
	
	template< typename T >
	class ShaderUniformImpl : public ShaderUniform {
	public:
		ShaderUniformImpl( std::string name, const T &value ) 
			: ShaderUniform( name ) 
		{
			setValue( value );
		}

		virtual ~ShaderUniformImpl( void ) 
		{ 
		}

		void setValue( const T &value ) { _value = value; }
		const T &getValue( void ) const { return _value; }

		virtual void onBind( Renderer *renderer ) override
		{
			renderer->bindUniform( getLocation(), getValue() );
		}

		virtual void onUnbind( Renderer *renderer ) override
		{
			// no-op
		}

	private:
		T _value;
	};

	typedef ShaderUniformImpl< bool > BoolUniform;
	typedef ShaderUniformImpl< int > IntUniform;
	typedef ShaderUniformImpl< float > FloatUniform;
	typedef ShaderUniformImpl< Vector4f > Vector4fUniform;
	typedef ShaderUniformImpl< Vector3f > Vector3fUniform;
	typedef ShaderUniformImpl< Vector2f > Vector2fUniform;
	typedef ShaderUniformImpl< RGBAColorf > RGBAColorfUniform;
	typedef ShaderUniformImpl< Matrix3f > Matrix3fUniform;
	typedef ShaderUniformImpl< Matrix4f > Matrix4fUniform;

	class TextureUniform : public ShaderUniform {
	public:
		TextureUniform( std::string name, Texture *value );
		TextureUniform( std::string name, SharedPointer< Texture > const &value );
		virtual ~TextureUniform( void );

		void setValue( Texture *texture );
		Texture *getValue( void );

	private:
		SharedPointer< Texture > _texture;

	public:
		virtual void onBind( Renderer *renderer );
		virtual void onUnbind( Renderer *renderer );
	};

}

#endif

