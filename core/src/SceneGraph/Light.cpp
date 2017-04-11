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

using namespace crimild;

Light::Light( Type type )
	: _type( type ),
	  _attenuation( 1.0f, 0.0f, 0.0f ),
	  _color( 1.0f, 1.0f, 1.0f, 1.0f ),
	  _outerCutoff( 0.0f ),
	  _innerCutoff( 0.0f ),
	  _exponent( 0.0f ),
      _castShadows( false ),
      _shadowNearCoeff( 1.0f ),
      _shadowFarCoeff( 512.0f ),
      _ambient( 0.0f, 0.0f, 0.0f, 0.0f )
{
    
}

Light::~Light( void )
{

}

void Light::accept( NodeVisitor &visitor )
{
	visitor.visitLight( this );
}

Matrix4f Light::computeProjectionMatrix( void ) const
{
	const float top = 10.0f;
	const float bottom = -10.0f;
	const float left = -10.0f;
	const float right = 10.0f;
	const float far = getShadowFarCoeff();
	const float near = getShadowNearCoeff();

	return Matrix4f(
		2.0f / ( right - left ), 0.0f, 0.0f, - ( right + left ) / ( right - left ),
		0.0f, 2.0f / ( top - bottom ), 0.0f, - ( top + bottom ) / ( top - bottom ),
		0.0f, 0.0f, -2.0f / ( far - near ), - ( far + near ) / ( far - near ),
		0.0f, 0.0f, 0.0f, 1.0f
	);

    // Frustumf f( 45.0f, 1.0f, getShadowNearCoeff(), getShadowFarCoeff() );
    // return f.computeProjectionMatrix();
}

Matrix4f Light::computeViewMatrix( void ) const
{
    return getWorld().computeModelMatrix().getInverse();
}

