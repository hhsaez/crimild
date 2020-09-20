/*
 * Copyright (c) 2002 - present, H. Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CRIMILD_CORE_RENDERING_MATERIALS_SIMPLE_LIT_
#define CRIMILD_CORE_RENDERING_MATERIALS_SIMPLE_LIT_

#include "Rendering/Material.hpp"

namespace crimild {

    /**
       \brief A simple material for aproximating lit objects using Gouraud or Phong shading

       Should be combined with SimpleLitPipeline or compatible pipelines.
     */
    class SimpleLitMaterial : public Material {
    public:
        struct Props {
            RGBAColorf ambient = RGBAColorf::ONE;
            RGBAColorf diffuse = RGBAColorf::ONE;
            RGBAColorf specular = RGBAColorf::ONE;
            Real32 shininess = 32.0f;
            Real32 padding[ 3 ];
        };

    public:
        SimpleLitMaterial( void ) noexcept;
        explicit SimpleLitMaterial( const Props &props ) noexcept;
        virtual ~SimpleLitMaterial( void ) = default;

        inline void setAmbient( const RGBAColorf &ambient ) noexcept override { getProps().ambient = ambient; };
        inline const RGBAColorf &getAmbient( void ) const noexcept override { return getProps().ambient; }

        inline void setDiffuse( const RGBAColorf &diffuse ) noexcept override { getProps().diffuse = diffuse; };
        inline const RGBAColorf &getDiffuse( void ) const noexcept override { return getProps().diffuse; }

        void setDiffuseMap( SharedPointer< Texture > const &diffuseMap ) noexcept;
        const Texture *getDiffuseMap( void ) const noexcept;
        Texture *getDiffuseMap( void ) noexcept;

        inline void setSpecular( const RGBAColorf &specular ) noexcept override { getProps().specular = specular; };
        inline const RGBAColorf &getSpecular( void ) const noexcept override { return getProps().specular; }

        void setSpecularMap( SharedPointer< Texture > const &specularMap ) noexcept override;
        const Texture *getSpecularMap( void ) const noexcept;
        Texture *getSpecularMap( void ) noexcept override;

        inline void setShininess( Real32 shininess ) noexcept override { getProps().shininess = shininess; }
        inline Real32 getShininess( void ) const noexcept override { return getProps().shininess; }

        void setNormalMap( SharedPointer< Texture > const &normalMap ) noexcept override;
        const Texture *getNormalMap( void ) const noexcept;
        Texture *getNormalMap( void ) noexcept override;

    private:
        Props &getProps( void ) noexcept;
        const Props &getProps( void ) const noexcept;
    };

}

#endif
