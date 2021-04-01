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

#include "Rendering/CommandBuffer.hpp"
#include "Rendering/ComputePass.hpp"
#include "Rendering/Image.hpp"
#include "Rendering/ImageView.hpp"
#include "Rendering/Operations/OperationUtils.hpp"
#include "Rendering/Operations/Operations.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/Texture.hpp"
#include "Simulation/Settings.hpp"
#include "Simulation/Simulation.hpp"

using namespace crimild;

SharedPointer< FrameGraphOperation > crimild::framegraph::computeImage(
    Extent2D extent,
    SharedPointer< Shader > shader,
    Format format,
    Array< Descriptor > descriptors ) noexcept
{
    if ( extent.scalingMode == ScalingMode::SWAPCHAIN_RELATIVE ) {
        // convert to fixed extent
        auto settings = Simulation::getInstance()->getSettings();
        extent.scalingMode = ScalingMode::FIXED;
        extent.width *= settings->get< Int32 >( "video.width", 1024 );
        extent.height *= settings->get< Int32 >( "video.height", 768 );
    }

    auto texture = crimild::alloc< Texture >();
    texture->imageView = [ & ] {
        auto imageView = crimild::alloc< ImageView >();
        imageView->image = [ & ] {
            auto image = crimild::alloc< Image >();
            image->format = format;
            image->extent = {
                .width = Real32( extent.width ),
                .height = Real32( extent.height ),
            };
            image->setMipLevels( 1 );
            image->setName( "Compute Image" );
            return image;
        }();
        return imageView;
    }();
    texture->sampler = crimild::alloc< Sampler >();

    auto pipeline = [ & ] {
        auto pipeline = crimild::alloc< ComputePipeline >();
        pipeline->setName( "Compute Pipeline" );
        pipeline->setProgram( [ & ] {
            auto program = crimild::alloc< ShaderProgram >(
                Array< SharedPointer< Shader > > {
                    shader,
                } );
            program->descriptorSetLayouts = {
                [ & ] {
                    auto layout = crimild::alloc< DescriptorSetLayout >();
                    layout->bindings = {
                        {
                            .descriptorType = DescriptorType::STORAGE_IMAGE,
                            .stage = Shader::Stage::COMPUTE,
                        },
                    };
                    descriptors.each(
                        [ & ]( auto descriptor ) {
                            layout->bindings.add(
                                {
                                    .descriptorType = descriptor.descriptorType,
                                    .stage = Shader::Stage::COMPUTE,
                                } );
                        } );
                    return layout;
                }(),
            };
            return program;
        }() );
        return pipeline;
    }();

    auto descriptorSet = [ & ] {
        auto ds = crimild::alloc< DescriptorSet >();
        ds->descriptors = {
            Descriptor {
                .descriptorType = DescriptorType::STORAGE_IMAGE,
                .obj = crimild::retain( texture ),
            },
        };
        descriptors.each(
            [ & ]( auto descriptor ) {
                ds->descriptors.add( descriptor );
            } );

        return ds;
    }();

    auto computePass = crimild::alloc< ComputePass >();
    computePass->setName( "computeImage" );
    computePass->writes( { texture } );
    computePass->produces( { texture } );

    return withComputeCommands(
        computePass,
        [ pipeline, descriptorSet, extent ]( auto commands ) {
            commands->bindComputePipeline( crimild::get_ptr( pipeline ) );
            commands->bindDescriptorSet( crimild::get_ptr( descriptorSet ) );
            commands->dispatch( DispatchWorkgroup {
                .x = UInt32( extent.width / DispatchWorkgroup::DEFAULT_WORGROUP_SIZE ),
                .y = UInt32( extent.height / DispatchWorkgroup::DEFAULT_WORGROUP_SIZE ),
                .z = 1 } );
        } );

    /*
    auto commands = cmp.create< CommandBuffer >();
    commands->begin( CommandBuffer::Usage::SIMULTANEOUS_USE );
    commands->bindComputePipeline( pipeline );
    commands->bindDescriptorSet( descriptors );
    commands->dispatch( DispatchWorkgroup {
        .x = UInt32( extent.width / DispatchWorkgroup::DEFAULT_WORGROUP_SIZE ),
        .y = UInt32( extent.height / DispatchWorkgroup::DEFAULT_WORGROUP_SIZE ),
        .z = 1 } );
    commands->end();
    computePass->setCommandRecorder( [ commands ]( Size ) { return commands; } );
    computePass->setConditional( props.isConditional );
    */

    //computePass->reads( { } );
}
