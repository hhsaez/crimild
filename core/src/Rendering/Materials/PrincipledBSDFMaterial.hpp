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

#ifndef CRIMILD_CORE_RENDERING_PRINCIPLED_BSDF_
#define CRIMILD_CORE_RENDERING_PRINCIPLED_BSDF_

#include "Mathematics/ColorRGB.hpp"
#include "Rendering/Material.hpp"

namespace crimild {

    namespace materials {

        /**
            \brief A PBR-based material

            Based on Disney/Unreal PBR system
        */
        class PrincipledBSDF : public Material {
        public:
            struct Props {
                alignas( 16 ) ColorRGB albedo = ColorRGB::Constants::WHITE;
                alignas( 4 ) Real32 metallic = 0;
                alignas( 4 ) Real32 roughness = 0;
                alignas( 4 ) Real32 ambientOcclusion = 1;
                alignas( 4 ) Real32 transmission = 0;
                alignas( 4 ) Real32 indexOfRefraction = 0;
                alignas( 16 ) ColorRGB emissive = ColorRGB::Constants::BLACK;
            };

        public:
            PrincipledBSDF( void ) noexcept;
            virtual ~PrincipledBSDF( void ) = default;

            inline void setAlbedo( const ColorRGB &albedo ) noexcept { getProps().albedo = albedo; };
            inline ColorRGB getAlbedo( void ) const noexcept { return getProps().albedo; }
            void setAlbedoMap( SharedPointer< Texture > const &albedoMap ) noexcept;
            const Texture *getAlbedoMap( void ) const noexcept;
            Texture *getAlbedoMap( void ) noexcept;

            inline void setMetallic( const Real32 &metallic ) noexcept { getProps().metallic = metallic; };
            inline Real32 getMetallic( void ) const noexcept { return getProps().metallic; }
            void setMetallicMap( SharedPointer< Texture > const &metallicMap ) noexcept;
            const Texture *getMetallicMap( void ) const noexcept;
            Texture *getMetallicMap( void ) noexcept;

            inline void setRoughness( const Real32 &roughness ) noexcept { getProps().roughness = roughness; };
            inline Real32 getRoughness( void ) const noexcept { return getProps().roughness; }
            void setRoughnessMap( SharedPointer< Texture > const &roughnessMap ) noexcept;
            const Texture *getRoughnessMap( void ) const noexcept;
            Texture *getRoughnessMap( void ) noexcept;

            inline void setAmbientOcclusion( const Real32 &ambientOcclusion ) noexcept { getProps().ambientOcclusion = ambientOcclusion; };
            inline Real32 getAmbientOcclusion( void ) const noexcept { return getProps().ambientOcclusion; }
            void setAmbientOcclusionMap( SharedPointer< Texture > const &ambientOcclusionMap ) noexcept;
            const Texture *getAmbientOcclusionMap( void ) const noexcept;
            Texture *getAmbientOcclusionMap( void ) noexcept;

            void setCombinedRoughnessMetallicMap( SharedPointer< Texture > const &roughnessMetallicMap ) noexcept;
            const Texture *getCombinedRoughnessMetallicMap( void ) const noexcept;
            Texture *getCombinedRoughnessMetallicMap( void ) noexcept;

            void setNormalMap( SharedPointer< Texture > const &normalMap ) noexcept override;
            const Texture *getNormalMap( void ) const noexcept;
            Texture *getNormalMap( void ) noexcept override;

            inline void setTransmission( const Real32 &transmission ) noexcept { getProps().transmission = transmission; };
            inline Real32 getTransmission( void ) const noexcept { return getProps().transmission; }

            inline void setIndexOfRefraction( const Real32 &indexOfRefraction ) noexcept { getProps().indexOfRefraction = indexOfRefraction; };
            inline Real32 getIndexOfRefraction( void ) const noexcept { return getProps().indexOfRefraction; }

            inline void setEmissive( const ColorRGB &emissive ) noexcept { getProps().emissive = emissive; };
            inline ColorRGB getEmissive( void ) const noexcept { return getProps().emissive; }

            Props &getProps( void ) noexcept;
            const Props &getProps( void ) const noexcept;
        };

    }

}

#endif
