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

#ifndef CRIMILD_RENDERING_MATERIAL_
#define CRIMILD_RENDERING_MATERIAL_

#include "ColorMaskState.hpp"
#include "Crimild_Foundation.hpp"
#include "Crimild_Mathematics.hpp"
#include "Entity/Entity.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"

namespace crimild {

   class DescriptorSet;
   class GraphicsPipeline;

   class Light;

   class Material : public NamedObject, public Entity {
      CRIMILD_IMPLEMENT_RTTI( crimild::Material )

   public:
      Material( void ) noexcept;
      virtual ~Material( void ) = default;

      inline void setDescriptors( SharedPointer< DescriptorSet > const &descriptors ) noexcept { m_descriptors = descriptors; }
      inline const DescriptorSet *getDescriptors( void ) const noexcept { return get_ptr( m_descriptors ); }
      inline DescriptorSet *getDescriptors( void ) noexcept { return get_ptr( m_descriptors ); }

      inline void setGraphicsPipeline( SharedPointer< GraphicsPipeline > const &graphicsPipeline ) noexcept { m_graphicsPipeline = graphicsPipeline; }
      inline const GraphicsPipeline *getGraphicsPipeline( void ) const noexcept { return get_ptr( m_graphicsPipeline ); }
      inline GraphicsPipeline *getGraphicsPipeline( void ) noexcept { return get_ptr( m_graphicsPipeline ); }

   private:
      SharedPointer< DescriptorSet > m_descriptors;
      SharedPointer< GraphicsPipeline > m_graphicsPipeline;

      /**
         \deprecated The following fields are no longer relevant
      */
      //@{

   public:
      virtual void setProgram( ShaderProgram *program ) { _program = crimild::retain( program ); }
      virtual void setProgram( SharedPointer< ShaderProgram > const &program ) { _program = program; }
      virtual ShaderProgram *getProgram( void ) { return crimild::get_ptr( _program ); }
      virtual const ShaderProgram *getProgram( void ) const { return crimild::get_ptr( _program ); }

      virtual void setAmbient( const ColorRGBA &ambient ) { _ambient = ambient; }
      virtual const ColorRGBA &getAmbient( void ) const { return _ambient; }

      virtual void setDiffuse( const ColorRGBA &color ) { _diffuse = color; }
      virtual const ColorRGBA &getDiffuse( void ) const { return _diffuse; }

      virtual void setSpecular( const ColorRGBA &color ) { _specular = color; }
      virtual const ColorRGBA &getSpecular( void ) const { return _specular; }

      // virtual void setEmissive( float value ) { _emissive = value; }
      // virtual float getEmissive( void ) const { return _emissive; }

      virtual void setShininess( float value ) { _shininess = value; }
      virtual float getShininess( void ) const { return _shininess; }

      virtual void setColorMap( Texture *texture ) { _colorMap = crimild::retain( texture ); }
      virtual void setColorMap( SharedPointer< Texture > const &texture ) { _colorMap = texture; }
      virtual Texture *getColorMap( void ) { return crimild::get_ptr( _colorMap ); }

      virtual void setNormalMap( Texture *texture ) { _normalMap = crimild::retain( texture ); }
      virtual void setNormalMap( SharedPointer< Texture > const &texture ) { _normalMap = texture; }
      virtual Texture *getNormalMap( void ) { return crimild::get_ptr( _normalMap ); }

      virtual void setSpecularMap( Texture *texture ) { _specularMap = crimild::retain( texture ); }
      virtual void setSpecularMap( SharedPointer< Texture > const &texture ) { _specularMap = texture; }
      virtual Texture *getSpecularMap( void ) { return crimild::get_ptr( _specularMap ); }

      virtual void setEmissiveMap( Texture *texture ) { _emissiveMap = crimild::retain( texture ); }
      virtual void setEmissiveMap( SharedPointer< Texture > const &texture ) { _emissiveMap = texture; }
      virtual Texture *getEmissiveMap( void ) { return crimild::get_ptr( _emissiveMap ); }

      virtual void setColorMaskState( SharedPointer< ColorMaskState > const &colorMaskState ) { _colorMaskState = colorMaskState; }
      virtual ColorMaskState *getColorMaskState( void ) { return crimild::get_ptr( _colorMaskState ); }

      virtual bool castShadows( void ) const { return _castShadows; }
      virtual void setCastShadows( bool value ) { _castShadows = value; }

      virtual bool receiveShadows( void ) const { return _receiveShadows; }
      virtual void setReceiveShadows( bool value ) { _receiveShadows = value; }

   private:
      SharedPointer< ShaderProgram > _program;

      ColorRGBA _ambient;
      ColorRGBA _diffuse;
      ColorRGBA _specular;
      float _shininess;
      float _emissive;

      SharedPointer< Texture > _colorMap;
      SharedPointer< Texture > _normalMap;
      SharedPointer< Texture > _specularMap;
      SharedPointer< Texture > _emissiveMap;

      SharedPointer< ColorMaskState > _colorMaskState;

      bool _castShadows = true;
      bool _receiveShadows = true;

      //@}

      /**
                 \name Reflection
         \deprectated

                 In order for the reflection map to work, set reflection value to 1.0
              */
      //@{

   public:
      void setReflection( crimild::Real32 value ) { _reflection = value; }
      crimild::Real32 getReflection( void ) const { return _reflection; }

      void setReflectionMap( SharedPointer< Texture > const &map ) { _reflectionMap = map; }
      Texture *getReflectionMap( void ) { return crimild::get_ptr( _reflectionMap ); }

   private:
      crimild::Real32 _reflection = 0.0f;
      SharedPointer< Texture > _reflectionMap;

      //@}

      /**
                 \name Refraction
         \deprectated

                 In order for the refraction map to work, set refraction value to 1.0
              */
      //@{

   public:
      void setRefraction( crimild::Real32 value ) { _refraction = value; }
      crimild::Real32 getRefraction( void ) const { return _refraction; }

      void setRefractionMap( SharedPointer< Texture > const &map ) { _refractionMap = map; }
      Texture *getRefractionMap( void ) { return crimild::get_ptr( _refractionMap ); }

   private:
      crimild::Real32 _refraction = 0.0f;
      SharedPointer< Texture > _refractionMap;

      //@}

      /**
          \name Coding support
       */
      //@{

   public:
      virtual void encode( coding::Encoder &encoder ) override;
      virtual void decode( coding::Decoder &decoder ) override;

      //@}
   };

   using MaterialPtr = SharedPointer< Material >;

}

#endif
