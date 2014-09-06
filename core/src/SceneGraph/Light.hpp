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

#ifndef CRIMILD_SCENEGRAPH_LIGHT_
#define CRIMILD_SCENEGRAPH_LIGHT_

#include "Node.hpp"

namespace crimild {

	class Light : public Node {
	public:
		enum class Type {
			POINT,
			DIRECTIONAL,
			SPOT,
		};

	public:
		Light( Type type = Type::POINT );
		virtual ~Light( void );

		const Type &getType( void ) { return _type; }

		Vector3f getPosition( void ) const { return getWorld().getTranslate(); }
		Vector3f getDirection( void ) const { return ( _type == Type::POINT ? Vector3f( 0.0f, 0.0f, 0.0f ) : getWorld().computeDirection() ); }

		void setAttenuation( const Vector3f &attenuation ) { _attenuation = attenuation; }
		const Vector3f &getAttenuation( void ) const { return _attenuation; }

		void setColor( const RGBAColorf &color ) { _color = color; }
		const RGBAColorf &getColor( void ) const { return _color; }

		void setOuterCutoff( float value ) { _outerCutoff = value; }
		float getOuterCutoff( void ) const { return _outerCutoff; }

		void setInnerCutoff( float value ) { _innerCutoff = value; }
		float getInnerCutoff( void ) const { return _innerCutoff; }

		void setExponent( float value ) { _exponent = value; }
		float getExponent( void ) const { return _exponent; }
        
        Matrix4f computeProjectionMatrix( void ) const;
        Matrix4f computeViewMatrix( void ) const;
        
        bool shouldCastShadows( void ) const { return _castShadows; }
        void setCastShadows( bool value ) { _castShadows = value; }
        
        float getShadowNearCoeff( void ) const { return _shadowNearCoeff; }
        void setShadowNearCoeff( float value ) { _shadowNearCoeff = value; }
        
        float getShadowFarCoeff( void ) const { return _shadowFarCoeff; }
        void setShadowFarCoeff( float value ) { _shadowFarCoeff = value; }
        
        const RGBAColorf &getAmbient( void ) const { return _ambient; }
        void setAmbient( const RGBAColorf &ambient ) { _ambient = ambient; }

	private:
		Type _type;
		Vector3f _attenuation;
		RGBAColorf _color;
		float _outerCutoff;
		float _innerCutoff;
		float _exponent;
        bool _castShadows;
        float _shadowNearCoeff;
        float _shadowFarCoeff;
        RGBAColorf _ambient;

	public:
		virtual void accept( NodeVisitor &visitor ) override;

	private:
		Light( const Light & ) { }
		Light &operator=( const Light & ) { return *this; }
	};

}

#endif

