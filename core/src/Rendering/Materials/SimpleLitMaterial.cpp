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

#include "Rendering/Materials/SimpleLitMaterial.hpp"
#include "Rendering/Programs/PhongLitShaderProgram.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Simulation/AssetManager.hpp"

using namespace crimild;

SimpleLitMaterial::SimpleLitMaterial( void ) noexcept
    : SimpleLitMaterial( Props { } )
{
    // no-op
}

SimpleLitMaterial::SimpleLitMaterial( const Props &props ) noexcept
{
    // Use a default pipeline
    setPipeline(
        [] {
            auto pipeline = crimild::alloc< Pipeline >();
            pipeline->program = crimild::retain( AssetManager::getInstance()->get< PhongLitShaderProgram >() );
            return pipeline;
        }()
    );

    setDescriptors(
        [&] {
            auto descriptors = crimild::alloc< DescriptorSet >();
            descriptors->descriptors = {
                Descriptor {
                    .descriptorType = DescriptorType::UNIFORM_BUFFER,
                    .obj = crimild::alloc< UniformBuffer >( props ),
                },
                {
                    // Diffuse map
                    .descriptorType = DescriptorType::TEXTURE,
                    .obj = Texture::ONE,
                },
                {
                    // Specular map
                    .descriptorType = DescriptorType::TEXTURE,
                    .obj = Texture::ONE,
                },
            };
            return descriptors;
        }()
    );
}

SimpleLitMaterial::Props &SimpleLitMaterial::getProps( void ) noexcept
{
    return getDescriptors()->descriptors[ 0 ].get< UniformBuffer >()->getValue< Props >();
}

const SimpleLitMaterial::Props &SimpleLitMaterial::getProps( void ) const noexcept
{
    return getDescriptors()->descriptors[ 0 ].get< UniformBuffer >()->getValue< Props >();
}

void SimpleLitMaterial::setDiffuseMap( SharedPointer< Texture > const &diffuseMap ) noexcept
{
    getDescriptors()->descriptors[ 1 ].obj = diffuseMap;
}

const Texture *SimpleLitMaterial::getDiffuseMap( void ) const noexcept
{
    return getDescriptors()->descriptors[ 1 ].get< Texture >();
}

Texture *SimpleLitMaterial::getDiffuseMap( void ) noexcept
{
    return getDescriptors()->descriptors[ 1 ].get< Texture >();
}

void SimpleLitMaterial::setSpecularMap( SharedPointer< Texture > const &specularMap ) noexcept
{
    getDescriptors()->descriptors[ 2 ].obj = specularMap;
}

const Texture *SimpleLitMaterial::getSpecularMap( void ) const noexcept
{
    return getDescriptors()->descriptors[ 2 ].get< Texture >();
}

Texture *SimpleLitMaterial::getSpecularMap( void ) noexcept
{
    return getDescriptors()->descriptors[ 2 ].get< Texture >();
}