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

#include "Rendering/Compositions/ComputeReflectionMapComposition.hpp"

#include "Components/MaterialComponent.hpp"
#include "Mathematics/Frustum.hpp"
#include "Rendering/CommandBuffer.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/ImageView.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Uniforms/CallbackUniformBuffer.hpp"
#include "SceneGraph/Skybox.hpp"
#include "Visitors/ApplyToGeometries.hpp"

using namespace crimild;
using namespace crimild::compositions;

Composition crimild::compositions::computeReflectionMap( Composition cmp, SharedPointer< Node > const &scene ) noexcept
{
    return computeReflectionMap( cmp, crimild::get_ptr( scene ) );
}

Composition crimild::compositions::computeReflectionMap( Composition cmp, Node *scene ) noexcept
{
    auto useHDR = cmp.isHDREnabled();

    // Computes a reflection map using only the skybox
    Geometry *skybox = nullptr;
    scene->perform(
        ApplyToGeometries(
            [ &skybox ]( auto geometry ) {
                if ( geometry->getLayer() == Node::Layer::SKYBOX ) {
                    skybox = geometry;
                }
            } ) );

    if ( skybox == nullptr ) {
        CRIMILD_LOG_WARNING( "No skybox found. Reflection map cannot be computed" );
        return cmp;
    }

    auto recordProbeCommands = []( compositions::Composition &cmp, CommandBuffer *commandBuffer, Array< ViewportDimensions > &layout, Size offset, Geometry *skybox ) {
        auto viewports = ViewportDimensions::cubeViewportsFrom( layout[ offset++ ] );

        auto pMatrix = Frustumf( 90.0f, 1.0f, 0.1f, 200.0f ).computeProjectionMatrix();
        auto primitive = skybox->anyPrimitive();
        auto material = skybox->getComponent< MaterialComponent >()->first();

        for ( auto face = 0l; face < 6; ++face ) {
            auto viewport = viewports[ face ];

            commandBuffer->setViewport( viewport );
            commandBuffer->setScissor( viewport );
            commandBuffer->bindGraphicsPipeline( material->getGraphicsPipeline() );
            commandBuffer->bindDescriptorSet(
                [ & ] {
                    auto ds = cmp.create< DescriptorSet >();
                    ds->descriptors = {
                        {
                            .descriptorType = DescriptorType::UNIFORM_BUFFER,
                            .obj = [ & ] {
                                struct Props {
                                    Matrix4f view;
                                    Matrix4f proj;
                                };

                                Transformation t;
                                switch ( face ) {
                                    case 0: // positive x
                                        t.rotate().fromAxisAngle( Vector3f::UNIT_Y, Numericf::HALF_PI );
                                        break;

                                    case 1: // negative x
                                        t.rotate().fromAxisAngle( Vector3f::UNIT_Y, -Numericf::HALF_PI );
                                        break;

                                    case 2: // positive y
                                        t.rotate().fromAxisAngle( Vector3f::UNIT_X, Numericf::HALF_PI );
                                        break;

                                    case 3: // negative y
                                        t.rotate().fromAxisAngle( Vector3f::UNIT_X, -Numericf::HALF_PI );
                                        break;

                                    case 4: // positive z
                                        t.rotate().fromAxisAngle( Vector3f::UNIT_Y, Numericf::PI );
                                        break;

                                    case 5: // negative z
                                        t.rotate().fromAxisAngle( Vector3f::UNIT_Y, 0 );
                                        break;
                                }

                                t.setTranslate( Vector3f::ZERO ); // TODO (hernan): use probe's position
                                auto vMatrix = t.computeModelMatrix().getInverse();

                                return crimild::alloc< UniformBuffer >(
                                    Props {
                                        .view = vMatrix,
                                        .proj = pMatrix,
                                    } );
                            }(),
                        },
                    };
                    return ds;
                }() );
            commandBuffer->bindDescriptorSet( material->getDescriptors() );
            commandBuffer->bindDescriptorSet( skybox->getDescriptors() );
            commandBuffer->drawPrimitive( crimild::get_ptr( primitive ) );
        }

        return offset;
    };

    // TODO: when creating the probe atlas, we can compute
    // a different layout here
    auto viewportLayout = Array< ViewportDimensions > {
        {
            .scalingMode = ScalingMode::RELATIVE,
            .dimensions = Rectf( 0, 0, 1, 1 ),
        },
    };

    auto renderPass = cmp.create< RenderPass >();
    renderPass->attachments = {
        [ & ] {
            auto att = cmp.createAttachment( "reflectionMapColor" );
            att->usage = Attachment::Usage::COLOR_ATTACHMENT;
            if ( useHDR ) {
                att->format = Format::R32G32B32A32_SFLOAT;
            } else {
                att->format = Format::R8G8B8A8_UNORM;
            }
            att->imageView = crimild::alloc< ImageView >();
            att->imageView->image = crimild::alloc< Image >();
            return crimild::retain( att );
        }(),
        [ & ] {
            auto att = cmp.createAttachment( "reflectionMapDepth" );
            att->format = Format::DEPTH_STENCIL_DEVICE_OPTIMAL;
            return crimild::retain( att );
        }()
    };

    renderPass->extent = {
        .scalingMode = ScalingMode::FIXED,
        .width = 256.0f,
        .height = 256.0f,
    };

    auto commandBuffer = cmp.create< CommandBuffer >();
    commandBuffer->begin( CommandBuffer::Usage::SIMULTANEOUS_USE );
    commandBuffer->beginRenderPass( renderPass, nullptr );
    auto offset = 0l;
    offset = recordProbeCommands(
        cmp,
        crimild::get_ptr( commandBuffer ),
        viewportLayout,
        offset,
        skybox );
    commandBuffer->endRenderPass( renderPass );
    commandBuffer->end();

    renderPass->setCommandRecorder(
        [ commandBuffer ]() {
            return commandBuffer;
        } );

    renderPass->setConditional( true );

    cmp.setOutput( crimild::get_ptr( renderPass->attachments[ 0 ] ) );

    return cmp;
}
