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

#include "Rendering/FrameGraph/VulkanComputeImageFromChannels.hpp"

#include "Rendering/ShaderProgram.hpp"
#include "Rendering/VulkanCommandBuffer.hpp"
#include "Rendering/VulkanComputePipeline.hpp"
#include "Rendering/VulkanDescriptor.hpp"
#include "Rendering/VulkanDescriptorPool.hpp"
#include "Rendering/VulkanDescriptorSet.hpp"
#include "Rendering/VulkanDescriptorSetLayout.hpp"
#include "Rendering/VulkanImage.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanRenderTarget.hpp"

using namespace crimild::vulkan::framegraph;

ComputeImageFromChannels::ComputeImageFromChannels(
    RenderDevice *device,
    std::shared_ptr< RenderTarget > const &input,
    std::string channels,
    SyncOptions const &options
) noexcept
    : ComputeImageFromChannels(
        device,
        input->getName(),
        input,
        channels,
        options
    )
{
    // no-op
}

ComputeImageFromChannels::ComputeImageFromChannels(
    RenderDevice *device,
    std::string name,
    std::shared_ptr< RenderTarget > const &input,
    std::string channels,
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
      m_output(
          crimild::alloc< RenderTarget >(
              device,
              name,
              input->getImage()->getFormat(),
              [ & ] {
                  auto imageExtent = input->getImage()->getExtent();
                  return VkExtent2D {
                      .width = imageExtent.width,
                      .height = imageExtent.height,
                  };
              }()
          )
      ),
      m_syncOptions( options )
{
    std::vector< Descriptor > descriptors;
    descriptors.push_back(
        Descriptor {
            .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .imageView = m_output->getImageView(),
            .stage = VK_SHADER_STAGE_COMPUTE_BIT,
            .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
        }
    );
    descriptors.push_back(
        Descriptor {
            .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .imageView = input->getImageView(),
            .stage = VK_SHADER_STAGE_COMPUTE_BIT,
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        }
    );

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

                        layout ( binding = 0, rgba32f ) uniform writeonly image2D outputImage;

                        layout ( binding = 1, rgba32f ) uniform readonly image2D gBufferImage0;
                        layout ( binding = 2, rgba32f ) uniform readonly image2D gBufferImage1;
                        layout ( binding = 3, rgba32f ) uniform readonly image2D gBufferImage2;
                        layout ( binding = 4, rgba32f ) uniform readonly image2D gBufferImage3;
                    
                        void main()
                        {
                            ivec2 uv = ivec2( gl_GlobalInvocationID.xy );
                            vec3 pixel = imageLoad( gBufferImage0, uv ).rgb;
                            imageStore( outputImage, uv, vec4( pixel, 1 ) );
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

void ComputeImageFromChannels::execute( void ) noexcept
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
            .imageView = m_output->getImageView(),
        }
    );

    cmds->bindPipeline( m_pipeline );
    cmds->bindDescriptorSet( 0, m_descriptorSet );
    cmds->dispatch( m_output->getExtent().width / 32, m_output->getExtent().height / 32, 1 );

    cmds->pipelineBarrier(
        vulkan::ImageMemoryBarrier {
            .srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
            .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .imageView = m_output->getImageView(),
        }
    );

    cmds->end();

    getRenderDevice()->submitComputeCommands( getCommandBuffer(), m_syncOptions.wait, m_syncOptions.signal );
}