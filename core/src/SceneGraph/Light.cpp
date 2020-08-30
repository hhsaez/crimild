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

#include "Light.hpp"
#include "Mathematics/Frustum.hpp"
#include "Coding/Encoder.hpp"
#include "Coding/Decoder.hpp"
#include "Rendering/ShadowMap.hpp"

using namespace crimild;

Light::Light( Type type )
	: _type( type ),
	  _attenuation( 1.0f, 0.0f, 0.0f ),
	  _color( 1.0f, 1.0f, 1.0f, 1.0f ),
	  _outerCutoff( 0.0f ),
	  _innerCutoff( 0.0f ),
	  _exponent( 0.0f ),
      _ambient( 0.0f, 0.0f, 0.0f, 0.0f )
{

}

Light::~Light( void )
{
	unload();
}

void Light::accept( NodeVisitor &visitor )
{
	visitor.visitLight( this );
}

void Light::setCastShadows( crimild::Bool enabled )
{
	if ( enabled ) {
		setShadowMap( crimild::alloc< ShadowMap >() );
	}
	else {
		setShadowMap( nullptr );
	}
}

Matrix4f Light::computeLightSpaceMatrix( void ) const noexcept
{
    Matrix4f proj;

    if ( getType() == Type::DIRECTIONAL ) {
        auto ortho = []( float left, float right, float bottom, float top, float near, float far ) {
            return Matrix4f(
                2.0f / ( right - left ), 0.0f, 0.0f, 0.0f,
                0.0f, 2.0f / ( bottom - top ), 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f / ( near - far ), 0.0f,

                -( right + left ) / ( right - left ),
                -( bottom + top ) / ( bottom - top ),
                near / ( near - far ),
                1.0f
            );
        };
        proj = ortho( -20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 200.0f );
    }
    else if ( getType() == Type::POINT ) {
        proj = Frustumf( 90.0f, 1.0f, 1.0f, 200.0f ).computeProjectionMatrix();
    }
    else {
        proj = Frustumf( 45.0f, 1.0f, 1.0f, 200.0f ).computeProjectionMatrix();
    }

    //Transformation lightTransform = getWorld();
    //auto view = lightTransform.computeModelMatrix().getInverse();

    // matrices are transposed in GLSL...
    //return view * proj;
    return proj;
}

void Light::encode( coding::Encoder &encoder )
{
	Node::encode( encoder );

	std::string lightType;
	switch ( _type ) {
		case Light::Type::POINT:
			lightType = "point";
			break;

		case Light::Type::DIRECTIONAL:
			lightType = "directional";
			break;

		case Light::Type::SPOT:
			lightType = "spot";
			break;

		case Light::Type::AMBIENT:
			lightType = "ambient";
			break;
	}
	encoder.encode( "lightType", lightType );

	encoder.encode( "attenuation", _attenuation );
	encoder.encode( "color", _color );
	encoder.encode( "ambient", _ambient );
}

void Light::decode( coding::Decoder &decoder )
{
	Node::decode( decoder );

	std::string lightType;
	decoder.decode( "lightType", lightType );
	if ( lightType == "ambient" ) {
		_type = Light::Type::AMBIENT;
	}
	else if ( lightType == "directional" ) {
		_type = Light::Type::DIRECTIONAL;
	}
	else if ( lightType == "spot" ) {
		_type = Light::Type::SPOT;
	}
	else {
		_type = Light::Type::POINT;
	}

	decoder.decode( "attenuation", _attenuation );
	decoder.decode( "color", _color );
	decoder.decode( "ambient", _ambient );

	crimild::Bool shadows = false;
	decoder.decode( "shadows", shadows );
	setCastShadows( shadows );
}
