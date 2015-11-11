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

#ifndef CRIMILD_RENDERING_SHADER_UNIFORM_IMPL_
#define CRIMILD_RENDERING_SHADER_UNIFORM_IMPL_

#include "ShaderUniform.hpp"
#include "Renderer.hpp"
#include "Mathematics/Matrix.hpp"
#include "Mathematics/Vector.hpp"

namespace crimild {
    
	template< typename T >
	class ShaderUniformImpl : public ShaderUniform {
		CRIMILD_DISALLOW_COPY_AND_ASSIGN( ShaderUniformImpl< T > );
		
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

	private:
		T _value;
	};

	typedef ShaderUniformImpl< bool > BoolUniform;
	typedef ShaderUniformImpl< int > IntUniform;
	typedef ShaderUniformImpl< float > FloatUniform;
	typedef ShaderUniformImpl< Vector3f > Vector3fUniform;
	typedef ShaderUniformImpl< Vector2f > Vector2fUniform;
	typedef ShaderUniformImpl< RGBAColorf > RGBAColorfUniform;
	typedef ShaderUniformImpl< Matrix3f > Matrix3fUniform;

}

#endif

