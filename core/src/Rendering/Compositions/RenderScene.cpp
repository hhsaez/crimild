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
#include "Rendering/Compositions/ComputeShadowComposition.hpp"

#include "Components/MaterialComponent.hpp"
#include "Rendering/CommandBuffer.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/Uniforms/CameraViewProjectionUniformBuffer.hpp"
#include "Rendering/Uniforms/LightingUniform.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Node.hpp"
#include "Visitors/ApplyToGeometries.hpp"
#include "Visitors/FetchCameras.hpp"
#include "Visitors/FetchLights.hpp"

using namespace crimild;
using namespace crimild::compositions;

Composition crimild::compositions::renderScene( SharedPointer< Node > const &scene ) noexcept
{
    return renderScene( crimild::get_ptr( scene ) );
}

Composition crimild::compositions::renderScene( Node *scene ) noexcept
{
    Composition cmp;

    cmp = computeShadow( cmp, scene );

    auto shadowAtlas = [&] {
        auto texture = cmp.create< Texture >();
        texture->imageView = [&] {
            auto att = cmp.getOutput();
            if ( att == nullptr || att->imageView == nullptr ) {
                auto imageView = crimild::alloc< ImageView >();
                imageView->image = Image::ONE;
                return imageView;
            }
            return att->imageView;
        }();
        texture->sampler = [] {
            auto sampler = crimild::alloc< Sampler >();
            // In order to avoid darking objects outside of the view frustum, we set
            // the wrap mode to either CLAMP_TO_BORDER, with a WHITE border color
            // (all ones). That way, any object outside the light's frustum will have
            // a depth of 1.0. Then, when comparing that depth with the current
            // fragment's one, the later one will never be in shadow.
            // For some types of lights, we might want to use a different approach though.
            // For example, point lights or spot lights should make objects outside of
            // the view frustum to be in shadow.
            // TODO (hernan): Maybe it will be best to split the shadow atlas into two after all
            sampler->setWrapMode( Sampler::WrapMode::CLAMP_TO_BORDER );
            sampler->setBorderColor( Sampler::BorderColor::INT_OPAQUE_WHITE );
            return sampler;
        }();
        return texture;
    }();

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
            //att->imageView = crimild::alloc< ImageView >();
            //att->imageView->image = crimild::alloc< Image >();
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
                Descriptor {
                    .descriptorType = DescriptorType::TEXTURE,
                    .obj = crimild::retain( shadowAtlas ),
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
