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

#include "Rendering/Operations/Operations_computeImage.hpp"

#include "Rendering/CommandBuffer.hpp"
#include "Rendering/ComputePass.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Image.hpp"
#include "Rendering/ImageView.hpp"
#include "Rendering/Operations/OperationUtils.hpp"
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
    Array< SharedPointer< DescriptorSet > > descriptorSets ) noexcept
{
    return computeImage(
        extent,
        shader,
        format,
        DispatchWorkgroup {
            .x = UInt32( extent.width / DispatchWorkgroup::DEFAULT_WORGROUP_SIZE ),
            .y = UInt32( extent.height / DispatchWorkgroup::DEFAULT_WORGROUP_SIZE ),
            .z = 1 },
        descriptorSets );
}

SharedPointer< FrameGraphOperation > crimild::framegraph::computeImage(
    Extent2D extent,
    SharedPointer< Shader > shader,
    Format format,
    const DispatchWorkgroup &workgroup,
    Array< SharedPointer< DescriptorSet > > descriptorSets ) noexcept
{
    auto computePass = crimild::alloc< ComputePass >();
    computePass->setName( "computeImage" );

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
            image->setName( computePass->getName() + "/image" );
            return image;
        }();
        return imageView;
    }();
    texture->sampler = crimild::alloc< Sampler >();

    auto pipeline = [ & ] {
        auto pipeline = crimild::alloc< ComputePipeline >();
        pipeline->setName( computePass->getName() + "/pipeline" );
        pipeline->setProgram( [ & ] {
            auto program = crimild::alloc< ShaderProgram >(
                Array< SharedPointer< Shader > > {
                    shader,
                } );
            program->descriptorSetLayouts = [ & ] {
                auto layouts = Array< SharedPointer< DescriptorSetLayout > > {};
                layouts.add(
                    [] {
                        auto layout = crimild::alloc< DescriptorSetLayout >();
                        layout->bindings = {
                            {
                                .descriptorType = DescriptorType::STORAGE_IMAGE,
                                .stage = Shader::Stage::COMPUTE,
                            },
                        };
                        return layout;
                    }() );
                descriptorSets.each(
                    [ & ]( auto &ds ) {
                        auto layout = crimild::alloc< DescriptorSetLayout >();
                        ds->descriptors.each(
                            [ & ]( auto descriptor ) {
                                layout->bindings.add(
                                    {
                                        .descriptorType = descriptor.descriptorType,
                                        .stage = Shader::Stage::COMPUTE,
                                    } );
                            } );
                        layouts.add( layout );
                    } );
                return layouts;
            }();
            return program;
        }() );
        return pipeline;
    }();

    auto imageDescriptors = [ & ] {
        auto ds = crimild::alloc< DescriptorSet >();
        ds->descriptors = {
            Descriptor {
                .descriptorType = DescriptorType::STORAGE_IMAGE,
                .obj = crimild::retain( texture ),
            },
        };
        return ds;
    }();

    computePass->writes( { texture } );
    computePass->produces( { texture } );

    return withComputeCommands(
        computePass,
        [ pipeline, imageDescriptors, descriptorSets, workgroup ]( auto commands ) {
            commands->bindComputePipeline( crimild::get_ptr( pipeline ) );
            commands->bindDescriptorSet( crimild::get_ptr( imageDescriptors ) );
            descriptorSets.each(
                [ & ]( auto descriptors ) {
                    commands->bindDescriptorSet( crimild::get_ptr( descriptors ) );
                } );
            commands->dispatch( workgroup );
        } );
}
