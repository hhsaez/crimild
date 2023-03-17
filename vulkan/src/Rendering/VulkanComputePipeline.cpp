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
 *     * Neither the name of the copyright holders nor the
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

#include "Rendering/VulkanComputePipeline.hpp"

#include "Rendering/ShaderProgram.hpp"
#include "Rendering/VulkanDescriptorSetLayout.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanShaderModule.hpp"

using namespace crimild::vulkan;

ComputePipeline::ComputePipeline(
    RenderDevice *device,
    std::string name,
    std::shared_ptr< ShaderProgram > const &program,
    const std::vector< std::shared_ptr< DescriptorSetLayout > > &inDescriptorSetLayouts
) noexcept
    : WithRenderDevice( device ),
      Named( name ),
      m_program( program ),
      m_descriptorSetLayouts( inDescriptorSetLayouts )
{
    auto shaderModule = crimild::alloc< ShaderModule >( device, program->getShaders().first() );
    auto shaderStageInfo = shaderModule->getShaderStageCreateInfo();

    std::vector< VkDescriptorSetLayout > descriptorSetLayouts;
    std::transform(
        m_descriptorSetLayouts.begin(),
        m_descriptorSetLayouts.end(),
        std::back_inserter( descriptorSetLayouts ),
        []( auto &dsl ) {
            return dsl->getHandle();
        }
    );

    VkPipelineLayoutCreateInfo layoutInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = uint32_t( descriptorSetLayouts.size() ),
        .pSetLayouts = descriptorSetLayouts.data(),
    };
    CRIMILD_VULKAN_CHECK(
        vkCreatePipelineLayout(
            getRenderDevice()->getHandle(),
            &layoutInfo,
            getRenderDevice()->getAllocator(),
            &m_pipelineLayout
        )
    );

    VkComputePipelineCreateInfo info {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .layout = m_pipelineLayout,
        .flags = 0,
        .stage = shaderStageInfo,
    };

    VkPipeline handle;
    CRIMILD_VULKAN_CHECK(
        vkCreateComputePipelines(
            getRenderDevice()->getHandle(),
            VK_NULL_HANDLE,
            1,
            &info,
            getRenderDevice()->getAllocator(),
            &handle
        )
    );
    setHandle( handle );
}

ComputePipeline::~ComputePipeline( void ) noexcept
{
    vkDestroyPipeline( getRenderDevice()->getHandle(), getHandle(), getRenderDevice()->getAllocator() );
    setHandle( VK_NULL_HANDLE );

    vkDestroyPipelineLayout( getRenderDevice()->getHandle(), m_pipelineLayout, getRenderDevice()->getAllocator() );

    m_descriptorSetLayouts.clear();
}
