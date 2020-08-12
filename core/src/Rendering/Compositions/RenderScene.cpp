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

#include "Rendering/Compositions/RenderScene.hpp"

#include "Components/MaterialComponent.hpp"
#include "Rendering/CommandBuffer.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Uniforms/CameraViewProjectionUniformBuffer.hpp"
#include "Rendering/Uniforms/LightingUniform.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Node.hpp"
#include "Visitors/ApplyToGeometries.hpp"
#include "Visitors/FetchCameras.hpp"
#include "Visitors/FetchLights.hpp"

using namespace crimild;
using namespace crimild::compositions;

Composition crimild::compositions::renderScene( Node *scene ) noexcept
{
    Composition cmp;
    auto renderPass = cmp.create< RenderPass >();
    renderPass->attachments = {
        [&] {
            auto att = cmp.createAttachment( "gBufferColor" );
            att->usage = Attachment::Usage::COLOR_ATTACHMENT;
            att->format = Format::R8G8B8A8_UNORM;
            att->imageView = crimild::alloc< ImageView >();
            att->imageView->image = crimild::alloc< Image >();
            return crimild::retain( att );
        }(),
        [&] {
            auto att = cmp.createAttachment( "gBufferDepth" );
            att->format = Format::DEPTH_STENCIL_DEVICE_OPTIMAL;
            return crimild::retain( att );
        }()
    };

    renderPass->setDescriptors(
        [&] {
            auto descriptorSet = crimild::alloc< DescriptorSet >();
            descriptorSet->descriptors = {
                Descriptor {
                    .descriptorType = DescriptorType::UNIFORM_BUFFER,
                    .obj = [&] {
                        FetchCameras fetch;
                        scene->perform( fetch );
                        auto camera = fetch.anyCamera();
                        return crimild::alloc< CameraViewProjectionUniform >( camera );
                    }(),
                },
                Descriptor {
                    .descriptorType = DescriptorType::UNIFORM_BUFFER,
                    .obj = crimild::alloc< LightingUniform >(
                        [&] {
                            FetchLights fetch;
                            Array< Light * > lights;
                            scene->perform( fetch );
                            fetch.forEachLight(
                                [&]( auto light ) {
                                    lights.add( light );
                                }
                            );
                            return lights;
                        }()
                    ),
                },
            };
            return descriptorSet;
        }()
    );

    renderPass->commands = [&] {
        auto commandBuffer = crimild::alloc< CommandBuffer >();
        scene->perform(
            ApplyToGeometries(
                [&]( Geometry *g ) {
                    if ( auto ms = g->getComponent< MaterialComponent >() ) {
                        if ( auto material = ms->first() ) {
                            commandBuffer->bindGraphicsPipeline( material->getPipeline() );
                            commandBuffer->bindDescriptorSet( renderPass->getDescriptors() );
                            commandBuffer->bindDescriptorSet( material->getDescriptors() );
                            commandBuffer->bindDescriptorSet( g->getDescriptors() );
                            commandBuffer->drawPrimitive( g->anyPrimitive() );
                        }
                    }
                }
            )
        );
        return commandBuffer;
    }();

    cmp.setOutput( crimild::get_ptr( renderPass->attachments[ 0 ] ) );

    return cmp;
}
