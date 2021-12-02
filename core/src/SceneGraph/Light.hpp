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

	class ShadowMap;

	class Light : public Node {
		CRIMILD_IMPLEMENT_RTTI( crimild::Light )
		
	public:
		enum class Type {
			AMBIENT,
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
        
        const RGBAColorf &getAmbient( void ) const { return _ambient; }
        void setAmbient( const RGBAColorf &ambient ) { _ambient = ambient; }

	private:
		Type _type;
		Vector3f _attenuation;
		RGBAColorf _color;
		float _outerCutoff;
		float _innerCutoff;
		float _exponent;
        RGBAColorf _ambient;

	public:
		virtual void accept( NodeVisitor &visitor ) override;

	public:
		void setCastShadows( crimild::Bool enabled );
		inline crimild::Bool castShadows( void ) const { return _shadowMap != nullptr; }

		void setShadowMap( SharedPointer< ShadowMap > const &shadowMap ) { _shadowMap = shadowMap; }
		inline ShadowMap *getShadowMap( void ) { return crimild::get_ptr( _shadowMap ); }

	private:
		SharedPointer< ShadowMap > _shadowMap;

        /**
            \name Coding
         */
        //@{
		
    public:
        virtual void encode( coding::Encoder &encoder ) override;
        virtual void decode( coding::Decoder &decoder ) override;
        
        //@}

	};

}

#endif

