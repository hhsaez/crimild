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

#include "Components/MaterialComponent.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Operations/OperationUtils.hpp"
#include "Rendering/Operations/Operations.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/RenderableSet.hpp"
#include "Rendering/Uniforms/CameraViewProjectionUniformBuffer.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Geometry.hpp"

using namespace crimild;

SharedPointer< FrameGraphOperation > crimild::framegraph::computeReflectionMap( SharedPointer< FrameGraphResource > const renderables ) noexcept
{
    auto renderPass = crimild::alloc< RenderPass >();
    renderPass->setName( "computeReflectionMap" );

    auto color = useColorAttachment( "computeReflectionMap/color", Format::R32G32B32A32_SFLOAT );
    auto depth = useDepthAttachment( "computeReflectionMap/depth" );

    renderPass->attachments = { color, depth };

    // TODO: when creating the probe atlas, we can compute
    // a different layout here
    auto viewportLayout = Array< ViewportDimensions > {
        {
            .scalingMode = ScalingMode::RELATIVE,
            .dimensions = Rectf( 0, 0, 1, 1 ),
        },
    };

    renderPass->extent = {
        .scalingMode = ScalingMode::FIXED,
        .width = 256.0f,
        .height = 256.0f,
    };

    renderPass->reads( { renderables } );
    renderPass->writes( { color, depth } );
    renderPass->produces( { color, depth } );

    auto pMatrix = Frustumf( 90.0f, 1.0f, 0.1f, 200.0f ).computeProjectionMatrix();
    auto descriptors = Array< SharedPointer< DescriptorSet > >( 6 );
    for ( auto face = 0l; face < 6; ++face ) {
        auto ds = crimild::alloc< DescriptorSet >();
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
        descriptors[ face ] = ds;
    }

    return withDynamicGraphicsCommands(
        renderPass,
        [ descriptors,
          viewportLayout,
          renderables = crimild::cast_ptr< RenderableSet >( renderables ) ]( auto commandBuffer ) {
            renderables->eachGeometry(
                [ & ]( Geometry *geometry ) {
                    auto primitive = geometry->anyPrimitive();
                    auto material = geometry->getComponent< MaterialComponent >()->first();

                    auto viewports = ViewportDimensions::cubeViewportsFrom( viewportLayout[ 0 ] );

                    for ( auto face = 0l; face < 6; ++face ) {
                        auto viewport = viewports[ face ];

                        commandBuffer->setViewport( viewport );
                        commandBuffer->setScissor( viewport );
                        commandBuffer->bindGraphicsPipeline( material->getGraphicsPipeline() );
                        commandBuffer->bindDescriptorSet( crimild::get_ptr( descriptors[ face ] ) );
                        commandBuffer->bindDescriptorSet( material->getDescriptors() );
                        commandBuffer->bindDescriptorSet( geometry->getDescriptors() );
                        commandBuffer->drawPrimitive( crimild::get_ptr( primitive ) );
                    }
                } );
        } );
}
