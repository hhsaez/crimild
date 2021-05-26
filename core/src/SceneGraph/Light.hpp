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

#include "Foundation/Containers/Array.hpp"
#include "Mathematics/ColorRGBA.hpp"
#include "Node.hpp"
#include "Rendering/Catalog.hpp"

namespace crimild {

    class ShadowMap;
    class DescriptorSet;

    /**
       Attenuation values:
       | Distance | Constant | Linear | Quadratic
       | 7	      | 1.0	     | 0.7    | 1.8
       | 13       | 1.0	     | 0.35   | 0.44
       | 20       | 1.0      | 0.22   | 0.20
       | 32       | 1.0	     | 0.14   | 0.07
       | 50	      | 1.0      | 0.09   | 0.032
       | 65	      | 1.0      | 0.07   | 0.017
       | 100	  | 1.0	     | 0.045  | 0.0075
       | 160	  | 1.0      | 0.027  | 0.0028
       | 200	  | 1.0      | 0.022  | 0.0019
       | 325	  | 1.0      | 0.014  | 0.0007
       | 600	  | 1.0      | 0.007  | 0.0002
       | 3250	  | 1.0      | 0.0014 | 0.000007
     */
    class Light : public Node, public Catalog< Light >::Resource {
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

        const Type &getType( void ) const noexcept { return _type; }

        Point3 getPosition( void ) const { return getWorld()( Point3::Constants::ZERO ); }
        Vector3 getDirection( void ) const { return ( _type == Type::POINT ? Vector3::Constants::ZERO : getWorld()( Vector3::Constants::UNIT_Z ) ); }

        void setAttenuation( const Vector3f &attenuation ) { _attenuation = attenuation; }
        const Vector3f &getAttenuation( void ) const { return _attenuation; }

        void setColor( const ColorRGBA &color ) { _color = color; }
        const ColorRGBA &getColor( void ) const { return _color; }

        void setOuterCutoff( float value ) { _outerCutoff = value; }
        float getOuterCutoff( void ) const { return _outerCutoff; }

        void setInnerCutoff( float value ) { _innerCutoff = value; }
        float getInnerCutoff( void ) const { return _innerCutoff; }

        void setExponent( float value ) { _exponent = value; }
        float getExponent( void ) const { return _exponent; }

        const ColorRGBA &getAmbient( void ) const { return _ambient; }
        void setAmbient( const ColorRGBA &ambient ) { _ambient = ambient; }

        inline Real32 getEnergy( void ) const noexcept { return m_energy; }
        inline void setEnergy( Real32 energy ) noexcept { m_energy = energy; }

        void setRadius( Real32 radius ) noexcept;
        Real32 getRadius( void ) const noexcept;

        DescriptorSet *getDescriptors( void ) noexcept;

    private:
        Type _type;
        Vector3f _attenuation;
        ColorRGBA _color;
        float _outerCutoff;
        float _innerCutoff;
        float _exponent;
        ColorRGBA _ambient;
        SharedPointer< DescriptorSet > m_descriptors;
        Real32 m_energy = 1.0f;
        Real32 m_radius = -1.0f; // compute radius based on energy by default

    public:
        virtual void accept( NodeVisitor &visitor ) override;

    public:
        void setCastShadows( crimild::Bool enabled );
        inline crimild::Bool castShadows( void ) const { return _shadowMap != nullptr; }

        Matrix4f computeLightSpaceMatrix( void ) const noexcept;

        void setShadowMap( SharedPointer< ShadowMap > const &shadowMap ) { _shadowMap = shadowMap; }
        inline ShadowMap *getShadowMap( void ) { return crimild::get_ptr( _shadowMap ); }

        /**
           \brief Get descriptors used for computing the shadow atlas

           For most lights, we will need only one. But point lights will require
           up to six (one for each face of the cubemap)
         */
        Array< SharedPointer< DescriptorSet > > &getShadowAtlasDescriptors( void ) noexcept;

    private:
        SharedPointer< ShadowMap > _shadowMap;
        Array< SharedPointer< DescriptorSet > > m_shadowAtlasDescriptors;

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
