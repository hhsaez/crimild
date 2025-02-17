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

#include "Rendering/Materials/UnlitMaterial.hpp"

#include "Crimild_Coding.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/Programs/UnlitShaderProgram.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Simulation/AssetManager.hpp"

using namespace crimild;

UnlitMaterial::UnlitMaterial( void ) noexcept
{
    setGraphicsPipeline(
        [] {
            auto pipeline = crimild::alloc< GraphicsPipeline >();
            if ( auto assets = AssetManager::getInstance() ) {
                pipeline->setProgram( crimild::retain( AssetManager::getInstance()->get< UnlitShaderProgram >() ) );
            }
            return pipeline;
        }()
    );

    setDescriptors(
        [ & ] {
            auto descriptors = crimild::alloc< DescriptorSet >();
            descriptors->descriptors = {
                {
                    .descriptorType = DescriptorType::UNIFORM_BUFFER,
                    .obj = crimild::alloc< UniformBuffer >( ColorRGBA::Constants::WHITE ),
                },
                {
                    .descriptorType = DescriptorType::TEXTURE,
                    .obj = Texture::ONE,
                },
            };
            return descriptors;
        }()
    );
}

void UnlitMaterial::setColor( const ColorRGBA &color ) noexcept
{
    getDescriptors()->descriptors[ 0 ].get< UniformBuffer >()->setValue( color );
}

ColorRGBA UnlitMaterial::getColor( void ) const noexcept
{
    return getDescriptors()->descriptors[ 0 ].get< UniformBuffer >()->getValue< ColorRGBA >();
}

void UnlitMaterial::setTexture( SharedPointer< Texture > const &texture ) noexcept
{
    getDescriptors()->descriptors[ 1 ].obj = texture;
}

const Texture *UnlitMaterial::getTexture( void ) const noexcept
{
    return getDescriptors()->descriptors[ 1 ].get< Texture >();
}

Texture *UnlitMaterial::getTexture( void ) noexcept
{
    return getDescriptors()->descriptors[ 1 ].get< Texture >();
}

void UnlitMaterial::encode( coding::Encoder &encoder )
{
    Material::encode( encoder );

    encoder.encode( "color", getColor() );
    encoder.encode( "colorMap", getTexture() );
}

void UnlitMaterial::decode( coding::Decoder &decoder )
{
    Material::decode( decoder );

    ColorRGBA color;
    decoder.decode( "color", color );
    setColor( color );

    SharedPointer< Texture > colorMap;
    decoder.decode( "colorMap", colorMap );
    if ( colorMap != nullptr ) {
        setTexture( colorMap );
    }
}
