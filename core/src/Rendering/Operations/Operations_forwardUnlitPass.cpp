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

SharedPointer< FrameGraphOperation > crimild::framegraph::forwardUnlitPass(
    SharedPointer< FrameGraphResource > const &renderables,
    SharedPointer< FrameGraphResource > const &colorInput,
    SharedPointer< FrameGraphResource > const &depthInput ) noexcept
{
    auto renderPass = crimild::alloc< RenderPass >();
    renderPass->setName( "forwardUnlit" );

    auto color = colorInput != nullptr ? crimild::cast_ptr< Attachment >( colorInput ) : useColorAttachment( "scene/forward/color" );
    auto depth = depthInput != nullptr ? crimild::cast_ptr< Attachment >( depthInput ) : useDepthAttachment( "scene/forward/depth" );

    renderPass->attachments = { color, depth };

    auto descriptors = [ & ] {
        auto descriptorSet = crimild::alloc< DescriptorSet >();
        // descriptorSet->descriptors = {
        //     Descriptor {
        //         .descriptorType = DescriptorType::UNIFORM_BUFFER,
        //         .obj = crimild::alloc< CameraViewProjectionUniform >( Camera::getMainCamera() ),
        //     },
        // };
        return descriptorSet;
    }();

    auto viewport = ViewportDimensions {
        .scalingMode = ScalingMode::SWAPCHAIN_RELATIVE,
        .dimensions = Rectf { { 0, 0 }, { 1, 1 } },
    };

    renderPass->reads( { renderables, colorInput, depthInput } );
    renderPass->writes( { color, depth } );
    renderPass->produces( { color, depth } );

    return withDynamicGraphicsCommands(
        renderPass,
        [ descriptors,
          viewport,
          renderables = crimild::cast_ptr< RenderableSet >( renderables ) ]( auto commandBuffer ) {
            commandBuffer->setViewport( viewport );
            commandBuffer->setScissor( viewport );

            renderables->eachGeometry(
                [ & ]( Geometry *geometry ) {
                    if ( auto ms = geometry->getComponent< MaterialComponent >() ) {
                        if ( auto material = ms->first() ) {
                            commandBuffer->bindGraphicsPipeline( material->getGraphicsPipeline() );
                            commandBuffer->bindDescriptorSet( crimild::get_ptr( descriptors ) );
                            commandBuffer->bindDescriptorSet( material->getDescriptors() );
                            commandBuffer->bindDescriptorSet( geometry->getDescriptors() );
                            commandBuffer->drawPrimitive( geometry->anyPrimitive() );
                        }
                    }
                } );
        } );
}
