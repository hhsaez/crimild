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

#ifndef CRIMILD_VULKAN_RENDERING_PIPELINE_
#define CRIMILD_VULKAN_RENDERING_PIPELINE_

#include "Foundation/Types.hpp"
#include "Foundation/SharedObject.hpp"
#include "Foundation/VulkanUtils.hpp"

namespace crimild {

	class Shader;
	class ShaderProgram;
	class Pipeline;

	/**
	   TODO:
	   - Primitive type
	   - VBO
	   - IBO
	   - Cull Mode
	   - Polygon Mode (fill, line, point)
	   - Line Width
	 */
	struct PipelineDescriptor {
		SharedPointer< ShaderProgram > program;
	};

	namespace vulkan {

		class VulkanRenderDevice;
		class Swapchain;

		/**
		 */
		class Pipeline : public SharedObject {
		public:
			struct ShaderModule {
				VkShaderStageFlagBits stage;
				VkShaderModule handler;
				std::string entryPointName;
			};

			using ShaderModuleArray = std::vector< ShaderModule >;
			using ShaderStageArray = std::vector< VkPipelineShaderStageCreateInfo >;
			
		public:
			Pipeline( VulkanRenderDevice *device, const PipelineDescriptor *descriptor );
			~Pipeline( void );

			const VkPipelineLayout &getPipelineLayout( void ) const noexcept { return m_pipelineLayout; }
			const VkPipeline &getGraphicsPipelineHandler( void ) const noexcept { return m_graphicsPipeline; }

		private:
			ShaderModuleArray createShaderModules( ShaderProgram *program ) const;
			ShaderModule createShaderModule( Shader *shader ) const;
			ShaderStageArray createShaderStages( const ShaderModuleArray &modules ) const noexcept;
			VkPipelineShaderStageCreateInfo createShaderStage( const ShaderModule &module ) const noexcept;
			VkPipelineVertexInputStateCreateInfo createVertexInput( void ) const noexcept;
			VkPipelineInputAssemblyStateCreateInfo createInputAssemby( void ) const noexcept;
			VkViewport createViewport( void ) const noexcept;
			VkRect2D createScissor( void ) const noexcept;
			VkPipelineViewportStateCreateInfo createViewportState( const VkViewport &viewport, const VkRect2D &scissor ) const noexcept;
			VkPipelineRasterizationStateCreateInfo createRasterizer( void ) const noexcept;
			VkPipelineMultisampleStateCreateInfo createMultiplesampleState( void ) const noexcept;
			VkPipelineDepthStencilStateCreateInfo createDepthStencilState( void ) const noexcept;
			VkPipelineColorBlendAttachmentState createColorBlendAttachment( void ) const noexcept;
			VkPipelineColorBlendStateCreateInfo createColorBlending( const VkPipelineColorBlendAttachmentState &colorBlendAttachment ) const noexcept;

			void createPipelineLayout( void );
			void createPipeline( void );

		private:
			VulkanRenderDevice *m_renderDevice = nullptr;
			VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
			VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;			
		};

	}

}
	
#endif
	
