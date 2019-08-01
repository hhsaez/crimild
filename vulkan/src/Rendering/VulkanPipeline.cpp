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

#include "VulkanPipeline.hpp"
#include "VulkanRenderDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanRenderPass.hpp"
#include "Exceptions/VulkanException.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Foundation/Log.hpp"

using namespace crimild::vulkan;

Pipeline::Pipeline( const VulkanRenderDevice *renderDevice, const Descriptor &descriptor )
	: m_renderDevice( renderDevice )
{
	CRIMILD_LOG_TRACE( "Creating pipeline" );

	auto device = m_renderDevice->getDeviceHandler();

	// WARNING: all of these config params are used when creating the pipeline and
	// they must be alive when vkCreatePipeline is called. Beware of scopes!
	auto shaderModules = createShaderModules( crimild::get_ptr( descriptor.program ) );
	auto shaderStages = createShaderStages( shaderModules );
	auto vertexInputInfo = createVertexInput();
	auto inputAssembly = createInputAssemby();
	auto viewport = createViewport();
	auto scissor = createScissor();
	auto viewportState = createViewportState( viewport, scissor );
	auto rasterizer = createRasterizer();
	auto multisampleState = createMultiplesampleState();
	//auto depthStencilState = createDepthStencilState();
	auto colorBlendAttachment = createColorBlendAttachment();
	auto colorBlending = createColorBlending( colorBlendAttachment );

	createPipelineLayout();

	auto createInfo = VkGraphicsPipelineCreateInfo {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = static_cast< uint32_t >( shaderStages.size() ),
		.pStages = shaderStages.data(),
		.pVertexInputState = &vertexInputInfo,
		.pInputAssemblyState = &inputAssembly,
		.pViewportState = &viewportState,
		.pRasterizationState = &rasterizer,
		.pMultisampleState = &multisampleState,
		//.pDepthStencilState = nullptr, // Optional
		.pColorBlendState = &colorBlending,
		//.pDynamicState = nullptr, // Optional
		.layout = m_pipelineLayout,
		.renderPass = descriptor.renderPass->getRenderPassHandler(),
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE, // Optional
		//.basePipelineIndex = -1, // Optional
	};

	CRIMILD_VULKAN_CHECK(
		vkCreateGraphicsPipelines(
			m_renderDevice->getDeviceHandler(),
			VK_NULL_HANDLE,
			1,
			&createInfo,
			nullptr,
			&m_graphicsPipeline
		)
	);
	
	// Cleanup
	for ( const auto &module : shaderModules ) {
		vkDestroyShaderModule(
			renderDevice->getDeviceHandler(),
			module.handler,
			nullptr
		);
	};
}

Pipeline::~Pipeline( void )
{
	CRIMILD_LOG_TRACE( "Destroying pipeline" );
	
	if ( m_renderDevice != nullptr ) {
		if ( m_graphicsPipeline != VK_NULL_HANDLE ) {
			vkDestroyPipeline(
				m_renderDevice->getDeviceHandler(),
				m_graphicsPipeline,
				nullptr
			);
			m_graphicsPipeline = VK_NULL_HANDLE;
		}
		
		if ( m_pipelineLayout != VK_NULL_HANDLE ) {
			vkDestroyPipelineLayout(
				m_renderDevice->getDeviceHandler(),
				m_pipelineLayout,
				nullptr
			);
			m_pipelineLayout = VK_NULL_HANDLE;
		}
	}
}

Pipeline::ShaderModuleArray Pipeline::createShaderModules( ShaderProgram *program ) const
{
	CRIMILD_LOG_TRACE( "Creating shader modules" );
	
	ShaderModuleArray modules;
	program->getShaders().each( [ this, &modules ]( SharedPointer< Shader > &shader ) {
		modules.push_back( createShaderModule( crimild::get_ptr( shader ) ) );
	});

	return modules;
}

Pipeline::ShaderModule Pipeline::createShaderModule( crimild::Shader *shader ) const
{
	CRIMILD_LOG_TRACE( "Creating shader module for stage ", shader->getStageDescription() );
	
	const auto &code = shader->getData();

	auto createInfo = VkShaderModuleCreateInfo {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = code.size(),
		.pCode = reinterpret_cast< const uint32_t * >( code.data() ),
	};

	VkShaderModule shaderModuleHandler;
	if ( vkCreateShaderModule( m_renderDevice->getDeviceHandler(), &createInfo, nullptr, &shaderModuleHandler ) != VK_SUCCESS ) {
		throw VulkanException( "Failed to create shader module" );
	}

	return ShaderModule {
		.stage = utils::VULKAN_SHADER_STAGES[ static_cast< crimild::UInt32 >( shader->getStage() ) ],
		.handler = shaderModuleHandler,
		.entryPointName = shader->getEntryPointName(),
	};
}

Pipeline::ShaderStageArray Pipeline::createShaderStages( const ShaderModuleArray &modules ) const noexcept
{
	CRIMILD_LOG_TRACE( "Creating shader stages" );
	
	ShaderStageArray shaderStages;
	for ( const auto &module : modules ) {
		shaderStages.push_back( createShaderStage( module ) );
	}
	return shaderStages;
}

VkPipelineShaderStageCreateInfo Pipeline::createShaderStage( const ShaderModule &module ) const noexcept
{
	CRIMILD_LOG_TRACE( "Creating shader stage" );

	return VkPipelineShaderStageCreateInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = module.stage,
		.module = module.handler,
		.pName = module.entryPointName.c_str(),
	};
}

VkPipelineVertexInputStateCreateInfo Pipeline::createVertexInput( void ) const noexcept
{
	return VkPipelineVertexInputStateCreateInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 0,
		.pVertexBindingDescriptions = nullptr,
		.vertexAttributeDescriptionCount = 0,
		.pVertexAttributeDescriptions = nullptr,
	};
}

VkPipelineInputAssemblyStateCreateInfo Pipeline::createInputAssemby( void ) const noexcept
{
	return VkPipelineInputAssemblyStateCreateInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE,
	};
}

VkViewport Pipeline::createViewport( void ) const noexcept
{
	auto swapchain = m_renderDevice->getSwapchain();
	
	return VkViewport {
		.x = 0,
		.y = 0,
		.width = ( float ) swapchain->getExtent().width,
		.height = ( float ) swapchain->getExtent().height,
		.minDepth = 0,
		.maxDepth = 1,
	};
}

VkRect2D Pipeline::createScissor( void ) const noexcept
{
	auto swapchain = m_renderDevice->getSwapchain();
	
	return VkRect2D {
		.offset = { 0, 0 },
		.extent = swapchain->getExtent(),
	};
}

VkPipelineViewportStateCreateInfo Pipeline::createViewportState( const VkViewport &viewport, const VkRect2D &scissor ) const noexcept
{
	return VkPipelineViewportStateCreateInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports = &viewport,
		.scissorCount = 1,
		.pScissors = &scissor,
	};
}

VkPipelineRasterizationStateCreateInfo Pipeline::createRasterizer( void ) const noexcept
{
	return VkPipelineRasterizationStateCreateInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE, // VK_TRUE might be required for shadow maps
		.rasterizerDiscardEnable = VK_FALSE, // VK_TRUE disables output to the framebuffer
		.polygonMode = VK_POLYGON_MODE_FILL,
		.lineWidth = 1.0f,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
		.depthBiasEnable = VK_FALSE, // Might be needed for shadow mapping
		.depthBiasConstantFactor = 0,
		.depthBiasClamp = 0,
		.depthBiasSlopeFactor = 0,
	};
}

VkPipelineMultisampleStateCreateInfo Pipeline::createMultiplesampleState( void ) const noexcept
{
	return VkPipelineMultisampleStateCreateInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.sampleShadingEnable = VK_FALSE,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.minSampleShading = 1.0f,
		.pSampleMask = nullptr,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable = VK_FALSE,
	};
}

VkPipelineDepthStencilStateCreateInfo Pipeline::createDepthStencilState( void ) const noexcept
{
	return VkPipelineDepthStencilStateCreateInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.depthTestEnable = VK_TRUE,
		.depthWriteEnable = VK_TRUE,
		.depthCompareOp = VK_COMPARE_OP_LESS,
		.depthBoundsTestEnable = VK_FALSE,
		.minDepthBounds = 0.0f,
		.maxDepthBounds = 1.0f,
		.stencilTestEnable = VK_FALSE,
		.front = {},
		.back = {},
	};
}

VkPipelineColorBlendAttachmentState Pipeline::createColorBlendAttachment( void ) const noexcept
{
	return VkPipelineColorBlendAttachmentState {
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
		.blendEnable = VK_FALSE,
		.srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
		.colorBlendOp = VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp = VK_BLEND_OP_ADD,
	};
}

VkPipelineColorBlendStateCreateInfo Pipeline::createColorBlending( const VkPipelineColorBlendAttachmentState &colorBlendAttachment ) const noexcept
{
	return VkPipelineColorBlendStateCreateInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY,
		.attachmentCount = 1,
		.pAttachments = &colorBlendAttachment,
		.blendConstants[ 0 ] = 0.0f,
		.blendConstants[ 1 ] = 0.0f,
		.blendConstants[ 2 ] = 0.0f,
		.blendConstants[ 3 ] = 0.0f,
	};
}

void Pipeline::createPipelineLayout( void )
{
	auto pipelineLayoutInfo = VkPipelineLayoutCreateInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 0,
		.pSetLayouts = nullptr,
		.pushConstantRangeCount = 0,
		.pPushConstantRanges = nullptr,
	};

	if ( vkCreatePipelineLayout( m_renderDevice->getDeviceHandler(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout ) != VK_SUCCESS ) {
		throw VulkanException( "Failed to create pipeline layout" );
	}
}

