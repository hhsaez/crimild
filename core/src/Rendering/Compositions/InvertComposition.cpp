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

#include "Rendering/Compositions/InvertComposition.hpp"

#include "Rendering/CommandBuffer.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/Programs/Compositions/InvertCompositionShaderProgram.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/Texture.hpp"
#include "Simulation/AssetManager.hpp"

using namespace crimild;
using namespace crimild::compositions;

Composition crimild::compositions::invert( Composition cmp ) noexcept
{
    auto renderPass = cmp.create< RenderPass >();
    renderPass->attachments = {
        [ & ] {
            auto att = cmp.createAttachment( "invert" );
            att->usage = Attachment::Usage::COLOR_ATTACHMENT;
            att->format = Format::R8G8B8A8_UNORM;
            att->imageView = crimild::alloc< ImageView >();
            att->imageView->image = crimild::alloc< Image >();
            return crimild::retain( att );
        }(),
    };

    renderPass->setDescriptors(
        [ & ] {
            auto descriptorSet = crimild::alloc< DescriptorSet >();
            descriptorSet->descriptors = {
                {
                    .descriptorType = DescriptorType::TEXTURE,
                    .obj = [ & ] {
                        if ( cmp.getOutputTexture() != nullptr ) {
                            return crimild::retain( cmp.getOutputTexture() );
                        }
                        auto texture = crimild::alloc< Texture >();
                        texture->imageView = cmp.getOutput()->imageView;
                        texture->sampler = [] {
                            auto sampler = crimild::alloc< Sampler >();
                            sampler->setMinFilter( Sampler::Filter::NEAREST );
                            sampler->setMagFilter( Sampler::Filter::NEAREST );
                            return sampler;
                        }();
                        return texture;
                    }(),
                },
            };
            return descriptorSet;
        }() );

    renderPass->setGraphicsPipeline(
        [] {
            auto pipeline = crimild::alloc< GraphicsPipeline >();
            pipeline->setProgram(
                crimild::retain(
                    AssetManager::getInstance()->get< InvertCompositionShaderProgram >() ) );
            return pipeline;
        }() );

    renderPass->commands = [ & ] {
        auto commandBuffer = crimild::alloc< CommandBuffer >();
        commandBuffer->bindGraphicsPipeline( renderPass->getGraphicsPipeline() );
        commandBuffer->bindDescriptorSet( renderPass->getDescriptors() );
        commandBuffer->draw( 6 );
        return commandBuffer;
    }();

    cmp.setOutput( crimild::get_ptr( renderPass->attachments[ 0 ] ) );

    return cmp;
}
