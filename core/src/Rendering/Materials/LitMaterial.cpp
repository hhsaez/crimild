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

#include "Rendering/Materials/LitMaterial.hpp"

#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/Programs/LitShaderProgram.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Simulation/AssetManager.hpp"

using namespace crimild;

LitMaterial::LitMaterial( void ) noexcept
{
    // Use a default pipeline
    setGraphicsPipeline(
        [] {
            auto pipeline = crimild::alloc< GraphicsPipeline >();
            pipeline->setProgram( crimild::retain( AssetManager::getInstance()->get< LitShaderProgram >() ) );
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
                    // Normal map
                    .descriptorType = DescriptorType::NORMAL_MAP,
                    .obj = Texture::ZERO,
                },
            };
            return descriptors;
        }() );
}

LitMaterial::Props &LitMaterial::getProps( void ) noexcept
{
    return getDescriptors()->descriptors[ 0 ].get< UniformBuffer >()->getValue< Props >();
}

const LitMaterial::Props &LitMaterial::getProps( void ) const noexcept
{
    return getDescriptors()->descriptors[ 0 ].get< UniformBuffer >()->getValue< Props >();
}

void LitMaterial::setAlbedoMap( SharedPointer< Texture > const &albedoMap ) noexcept
{
    getDescriptors()->descriptors[ 1 ].obj = albedoMap;
}

const Texture *LitMaterial::getAlbedoMap( void ) const noexcept
{
    return getDescriptors()->descriptors[ 1 ].get< Texture >();
}

Texture *LitMaterial::getAlbedoMap( void ) noexcept
{
    return getDescriptors()->descriptors[ 1 ].get< Texture >();
}

void LitMaterial::setMetallicMap( SharedPointer< Texture > const &metallicMap ) noexcept
{
    getDescriptors()->descriptors[ 2 ].obj = metallicMap;
}

const Texture *LitMaterial::getMetallicMap( void ) const noexcept
{
    return getDescriptors()->descriptors[ 2 ].get< Texture >();
}

Texture *LitMaterial::getMetallicMap( void ) noexcept
{
    return getDescriptors()->descriptors[ 2 ].get< Texture >();
}

void LitMaterial::setRoughnessMap( SharedPointer< Texture > const &roughnessMap ) noexcept
{
    getDescriptors()->descriptors[ 3 ].obj = roughnessMap;
}

const Texture *LitMaterial::getRoughnessMap( void ) const noexcept
{
    return getDescriptors()->descriptors[ 3 ].get< Texture >();
}

Texture *LitMaterial::getRoughnessMap( void ) noexcept
{
    return getDescriptors()->descriptors[ 3 ].get< Texture >();
}

void LitMaterial::setAmbientOcclusionMap( SharedPointer< Texture > const &ambientOcclusionMap ) noexcept
{
    getDescriptors()->descriptors[ 4 ].obj = ambientOcclusionMap;
}

const Texture *LitMaterial::getAmbientOcclusionMap( void ) const noexcept
{
    return getDescriptors()->descriptors[ 4 ].get< Texture >();
}

Texture *LitMaterial::getAmbientOcclusionMap( void ) noexcept
{
    return getDescriptors()->descriptors[ 4 ].get< Texture >();
}

void LitMaterial::setNormalMap( SharedPointer< Texture > const &normalMap ) noexcept
{
    getDescriptors()->descriptors[ 5 ].obj = normalMap;
}

const Texture *LitMaterial::getNormalMap( void ) const noexcept
{
    return getDescriptors()->descriptors[ 5 ].get< Texture >();
}

Texture *LitMaterial::getNormalMap( void ) noexcept
{
    return getDescriptors()->descriptors[ 5 ].get< Texture >();
}
