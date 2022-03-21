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

#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"

#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/Programs/LitShaderProgram.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Simulation/AssetManager.hpp"

using namespace crimild;
using namespace crimild::materials;

PrincipledBSDF::PrincipledBSDF( void ) noexcept
{
    // Use a default pipeline
    setGraphicsPipeline(
        [] {
            auto pipeline = crimild::alloc< GraphicsPipeline >();
            pipeline->setProgram(
                []() -> SharedPointer< ShaderProgram > {
                    if ( auto assets = AssetManager::getInstance() ) {
                        return crimild::retain( AssetManager::getInstance()->get< LitShaderProgram >() );
                    }
                    return nullptr;
                }() );
            return pipeline;
        }() );

    setDescriptors(
        [ & ] {
            auto descriptors = crimild::alloc< DescriptorSet >();
            descriptors->descriptors = {
                Descriptor {
                    .descriptorType = DescriptorType::UNIFORM_BUFFER,
                    .obj = crimild::alloc< UniformBuffer >( Props {} ),
                },
                {
                    // Albedo map
                    .descriptorType = DescriptorType::ALBEDO_MAP,
                    .obj = Texture::ONE,
                },
                {
                    // Metallic map
                    .descriptorType = DescriptorType::METALLIC_MAP,
                    .obj = Texture::ONE,
                },
                {
                    // Roughness map
                    .descriptorType = DescriptorType::ROUGHNESS_MAP,
                    .obj = Texture::ONE,
                },
                {
                    // Ambient Occlusion map
                    .descriptorType = DescriptorType::AMBIENT_OCCLUSION_MAP,
                    .obj = Texture::ONE,
                },
                {
                    // Ambient Roughness/Metallic map
                    .descriptorType = DescriptorType::COMBINED_ROUGHNESS_METALLIC_MAP,
                    .obj = Texture::ONE,
                },
                {
                    // Normal map
                    .descriptorType = DescriptorType::NORMAL_MAP,
                    .obj = Texture::ZERO,
                },
            };
            return descriptors;
        }() );
}

PrincipledBSDF::Props &PrincipledBSDF::getProps( void ) noexcept
{
    return getDescriptors()->descriptors[ 0 ].get< UniformBuffer >()->getValue< Props >();
}

const PrincipledBSDF::Props &PrincipledBSDF::getProps( void ) const noexcept
{
    return getDescriptors()->descriptors[ 0 ].get< UniformBuffer >()->getValue< Props >();
}

void PrincipledBSDF::setAlbedoMap( SharedPointer< Texture > const &albedoMap ) noexcept
{
    getDescriptors()->descriptors[ 1 ].obj = albedoMap;
}

const Texture *PrincipledBSDF::getAlbedoMap( void ) const noexcept
{
    return getDescriptors()->descriptors[ 1 ].get< Texture >();
}

Texture *PrincipledBSDF::getAlbedoMap( void ) noexcept
{
    return getDescriptors()->descriptors[ 1 ].get< Texture >();
}

void PrincipledBSDF::setMetallicMap( SharedPointer< Texture > const &metallicMap ) noexcept
{
    getDescriptors()->descriptors[ 2 ].obj = metallicMap;
}

const Texture *PrincipledBSDF::getMetallicMap( void ) const noexcept
{
    return getDescriptors()->descriptors[ 2 ].get< Texture >();
}

Texture *PrincipledBSDF::getMetallicMap( void ) noexcept
{
    return getDescriptors()->descriptors[ 2 ].get< Texture >();
}

void PrincipledBSDF::setRoughnessMap( SharedPointer< Texture > const &roughnessMap ) noexcept
{
    getDescriptors()->descriptors[ 3 ].obj = roughnessMap;
}

const Texture *PrincipledBSDF::getRoughnessMap( void ) const noexcept
{
    return getDescriptors()->descriptors[ 3 ].get< Texture >();
}

Texture *PrincipledBSDF::getRoughnessMap( void ) noexcept
{
    return getDescriptors()->descriptors[ 3 ].get< Texture >();
}

void PrincipledBSDF::setAmbientOcclusionMap( SharedPointer< Texture > const &ambientOcclusionMap ) noexcept
{
    getDescriptors()->descriptors[ 4 ].obj = ambientOcclusionMap;
}

const Texture *PrincipledBSDF::getAmbientOcclusionMap( void ) const noexcept
{
    return getDescriptors()->descriptors[ 4 ].get< Texture >();
}

Texture *PrincipledBSDF::getAmbientOcclusionMap( void ) noexcept
{
    return getDescriptors()->descriptors[ 4 ].get< Texture >();
}

void PrincipledBSDF::setCombinedRoughnessMetallicMap( SharedPointer< Texture > const &rm ) noexcept
{
    getDescriptors()->descriptors[ 5 ].obj = rm;
}

const Texture *PrincipledBSDF::getCombinedRoughnessMetallicMap( void ) const noexcept
{
    return getDescriptors()->descriptors[ 5 ].get< Texture >();
}

Texture *PrincipledBSDF::getCombinedRoughnessMetallicMap( void ) noexcept
{
    return getDescriptors()->descriptors[ 5 ].get< Texture >();
}

void PrincipledBSDF::setNormalMap( SharedPointer< Texture > const &normalMap ) noexcept
{
    getDescriptors()->descriptors[ 6 ].obj = normalMap;
}

const Texture *PrincipledBSDF::getNormalMap( void ) const noexcept
{
    return getDescriptors()->descriptors[ 6 ].get< Texture >();
}

Texture *PrincipledBSDF::getNormalMap( void ) noexcept
{
    return getDescriptors()->descriptors[ 6 ].get< Texture >();
}

void PrincipledBSDF::encode( coding::Encoder &encoder )
{
    Material::encode( encoder );

    encoder.encode( "albedo", getAlbedo() );
    encoder.encode( "metallic", getMetallic() );
    encoder.encode( "roughness", getRoughness() );
    encoder.encode( "ambientOcclusion", getAmbientOcclusion() );
    encoder.encode( "transmission", getTransmission() );
    encoder.encode( "indexOfRefraction", getIndexOfRefraction() );
    encoder.encode( "emissive", getEmissive() );

    {
        auto texture = crimild::retain( getAlbedoMap() );
        encoder.encode( "albedoMap", texture );
    }

    {
        auto texture = crimild::retain( getMetallicMap() );
        encoder.encode( "metallicMap", texture );
    }

    {
        auto texture = crimild::retain( getRoughnessMap() );
        encoder.encode( "roughnessMap", texture );
    }

    {
        auto texture = crimild::retain( getAmbientOcclusionMap() );
        encoder.encode( "ambientOcclusionMap", texture );
    }

    {
        auto texture = crimild::retain( getCombinedRoughnessMetallicMap() );
        encoder.encode( "combinedRoughnessMetallicMap", texture );
    }

    {
        auto texture = crimild::retain( getNormalMap() );
        encoder.encode( "normalMap", texture );
    }
}

void PrincipledBSDF::decode( coding::Decoder &decoder )
{
    Material::decode( decoder );

    decoder.decode( "albedo", getProps().albedo );
    decoder.decode( "metallic", getProps().metallic );
    decoder.decode( "roughness", getProps().roughness );
    decoder.decode( "ambientOcclusion", getProps().ambientOcclusion );
    decoder.decode( "transmission", getProps().transmission );
    decoder.decode( "indexOfRefraction", getProps().indexOfRefraction );
    decoder.decode( "emissive", getProps().emissive );

    {
        SharedPointer< Texture > texture;
        decoder.decode( "albedoMap", texture );
        setAlbedoMap( texture );
    }

    {
        SharedPointer< Texture > texture;
        decoder.decode( "metallicMap", texture );
        setMetallicMap( texture );
    }

    {
        SharedPointer< Texture > texture;
        decoder.decode( "roughnessMap", texture );
        setRoughnessMap( texture );
    }

    {
        SharedPointer< Texture > texture;
        decoder.decode( "ambientOcclusionMap", texture );
        setAmbientOcclusionMap( texture );
    }

    {
        SharedPointer< Texture > texture;
        decoder.decode( "combinedRoughnessMetallicMap", texture );
        setCombinedRoughnessMetallicMap( texture );
    }

    {
        SharedPointer< Texture > texture;
        decoder.decode( "normalMap", texture );
        setNormalMap( texture );
    }
}
