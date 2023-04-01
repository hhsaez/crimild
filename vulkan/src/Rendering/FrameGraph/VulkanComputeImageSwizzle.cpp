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

#include "Rendering/FrameGraph/VulkanComputeImageSwizzle.hpp"

#include "Rendering/ShaderProgram.hpp"
#include "Rendering/VulkanCommandBuffer.hpp"
#include "Rendering/VulkanComputePipeline.hpp"
#include "Rendering/VulkanDescriptor.hpp"
#include "Rendering/VulkanDescriptorPool.hpp"
#include "Rendering/VulkanDescriptorSet.hpp"
#include "Rendering/VulkanDescriptorSetLayout.hpp"
#include "Rendering/VulkanImage.hpp"
#include "Rendering/VulkanImageView.hpp"
#include "Rendering/VulkanRenderDevice.hpp"

using namespace crimild::vulkan::framegraph;

ComputeImageSwizzle::ComputeImageSwizzle(
    RenderDevice *device,
    std::string name,
    std::shared_ptr< vulkan::ImageView > const &input,
    Selector selector,
    std::shared_ptr< vulkan::ImageView > const &output,
    SyncOptions const &options
) noexcept
    : ComputeBase( device, name ),
      WithCommandBuffer(
          crimild::alloc< CommandBuffer >(
              device,
              getName() + "/CommandBuffer",
              VK_COMMAND_BUFFER_LEVEL_PRIMARY
          )
      ),
      m_input( input ),
      m_output( output ),
      m_selector( selector ),
      m_syncOptions( options )
{
    std::vector< Descriptor > descriptors = {
        Descriptor {
            .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .imageView = m_input,
            .stage = VK_SHADER_STAGE_COMPUTE_BIT,
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        },
        Descriptor {
            .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .imageView = m_output,
            .stage = VK_SHADER_STAGE_COMPUTE_BIT,
            .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
        },
    };

    auto descriptorSetLayout = crimild::alloc< DescriptorSetLayout >(
        getRenderDevice(),
        getName() + "/DescriptorSetLayout",
        descriptors
    );

    m_pipeline = [ & ] {
        auto program = crimild::alloc< ShaderProgram >();
        program->setShaders(
            Array< SharedPointer< Shader > > {
                crimild::alloc< Shader >(
                    Shader::Stage::COMPUTE,
                    R"(
                        layout( local_size_x = 32, local_size_y = 32 ) in;

                        layout ( binding = 0, rgba32f ) uniform readonly image2D inputImage;
                        layout ( binding = 1, rgba32f ) uniform writeonly image2D outputImage;
                    
                        layout( push_constant ) uniform Constants {
                            int selector;
                        };

                        void main()
                        {
                            ivec2 uv = ivec2( gl_GlobalInvocationID.xy );
                            vec4 pixel = imageLoad( inputImage, uv );
                            if ( selector == 1 ) {
                                pixel = vec4( pixel.rgb, 1 );
                            } else if ( selector == 2 ) {
                                pixel = vec4( vec3( pixel.r ), 1 );
                            } else if ( selector == 3 ) {
                                pixel = vec4( vec3( pixel.g ), 1 );
                            } else if ( selector == 4 ) {
                                pixel = vec4( vec3( pixel.b ), 1 );
                            } else if ( selector == 6 ) {
                                pixel = vec4( normalize( pixel.rgb ), 1 );
                            }
                            imageStore( outputImage, uv, pixel );
                        }
                    )"
                ),
            }
        );

        return std::make_unique< ComputePipeline >(
            getRenderDevice(),
            getName() + "/Pipeline",
            program,
            std::vector< std::shared_ptr< DescriptorSetLayout > > {
                descriptorSetLayout,
            },
            std::vector< VkPushConstantRange > {
                {
                    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
                    .offset = 0,
                    .size = sizeof( PushConstantsData ),
                },
            }
        );
    }();

    m_descriptorSet = crimild::alloc< DescriptorSet >(
        getRenderDevice(),
        getName() + "/DescriptorSet",
        crimild::alloc< DescriptorPool >( getRenderDevice(), getName() + "/DescriptorPool", descriptors ),
        descriptorSetLayout,
        descriptors
    );
}

void ComputeImageSwizzle::execute( void ) noexcept
{
    auto &cmds = getCommandBuffer();
    cmds->reset();
    cmds->begin();

    cmds->pipelineBarrier(
        vulkan::ImageMemoryBarrier {
            .srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            .srcAccessMask = VK_ACCESS_SHADER_READ_BIT,
            .dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_GENERAL,
            .imageView = m_output,
        }
    );

    cmds->bindPipeline( m_pipeline );
    cmds->bindDescriptorSet( 0, m_descriptorSet );

    cmds->pushConstants(
        VK_SHADER_STAGE_COMPUTE_BIT,
        0,
        PushConstantsData {
            .selector = static_cast< uint32_t >( m_selector ),
        }
    );

    cmds->dispatch( m_output->getExtent().width / 32, m_output->getExtent().height / 32, 1 );

    cmds->pipelineBarrier(
        vulkan::ImageMemoryBarrier {
            .srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
            .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .imageView = m_output,
        }
    );

    cmds->end();

    getRenderDevice()->submitComputeCommands(
        getCommandBuffer(),
        m_syncOptions.wait,
        m_syncOptions.signal
    );
}
