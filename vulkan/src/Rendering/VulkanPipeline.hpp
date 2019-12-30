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

#include "Foundation/VulkanObject.hpp"
#include "Primitives/Primitive.hpp"
#include "Mathematics/Rect.hpp"

namespace crimild {

	class Shader;
	class ShaderProgram;

	namespace vulkan {

        class PipelineLayout;
        class PipelineManager;
		class RenderDevice;
        class RenderPass;
        class ShaderModule;
        class Swapchain;

		/**
		 */
		class Pipeline : public VulkanObject {
            CRIMILD_IMPLEMENT_RTTI( crimild::vulkan::Pipeline )

		public:
			/**
			   TODO:
			   - Primitive type
			   - VBO
			   - IBO
			   - Cull Mode
			   - Polygon Mode (fill, line, point)
			   - Line Width
			*/
			struct Descriptor {
                RenderDevice *renderDevice;
				SharedPointer< ShaderProgram > program;
				const RenderPass *renderPass;
                Primitive::Type primitiveType;
                Rectf viewport;
                Rectf scissor;
			};
			
//			struct ShaderModule {
//				VkShaderStageFlagBits stage;
//				VkShaderModule handler;
//				std::string entryPointName;
//			};
//
//			using ShaderModuleArray = std::vector< ShaderModule >;
//			using ShaderStageArray = std::vector< VkPipelineShaderStageCreateInfo >;
//
		public:
//			Pipeline( const VulkanRenderDevice *device, const Descriptor &descriptor );
			~Pipeline( void );

            RenderDevice *renderDevice = nullptr;
            VkPipeline handler = VK_NULL_HANDLE;
            PipelineManager *manager = nullptr;
            SharedPointer< PipelineLayout > layout;

//			const VkPipelineLayout &getPipelineLayout( void ) const noexcept { return m_pipelineLayout; }
//			const VkPipeline &getGraphicsPipelineHandler( void ) const noexcept { return m_graphicsPipeline; }

		private:
//			ShaderModuleArray createShaderModules( ShaderProgram *program ) const;
//			ShaderModule createShaderModule( Shader *shader ) const;
//			ShaderStageArray createShaderStages( const ShaderModuleArray &modules ) const noexcept;
//			VkPipelineShaderStageCreateInfo createShaderStage( const ShaderModule &module ) const noexcept;
//			VkPipelineVertexInputStateCreateInfo createVertexInput( void ) const noexcept;
//			VkPipelineInputAssemblyStateCreateInfo createInputAssemby( void ) const noexcept;
//			VkViewport createViewport( void ) const noexcept;
//			VkRect2D createScissor( void ) const noexcept;
//			VkPipelineViewportStateCreateInfo createViewportState( const VkViewport &viewport, const VkRect2D &scissor ) const noexcept;
//			VkPipelineRasterizationStateCreateInfo createRasterizer( void ) const noexcept;
//			VkPipelineMultisampleStateCreateInfo createMultiplesampleState( void ) const noexcept;
//			VkPipelineDepthStencilStateCreateInfo createDepthStencilState( void ) const noexcept;
//			VkPipelineColorBlendAttachmentState createColorBlendAttachment( void ) const noexcept;
//			VkPipelineColorBlendStateCreateInfo createColorBlending( const VkPipelineColorBlendAttachmentState &colorBlendAttachment ) const noexcept;

			void createPipelineLayout( void );

		private:
//			const VulkanRenderDevice *m_renderDevice = nullptr;
//			VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
//			VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
		};

        class PipelineManager : public VulkanObjectManager< Pipeline > {
        public:
            explicit PipelineManager( RenderDevice *renderDevice ) noexcept : m_renderDevice( renderDevice ) { }
            virtual ~PipelineManager( void ) noexcept = default;

            SharedPointer< Pipeline > create( Pipeline::Descriptor const &descriptor ) noexcept;
            void destroy( Pipeline *pipeline ) noexcept;

        private:
            using ShaderModuleArray = std::vector< SharedPointer< ShaderModule >>;
            using ShaderStageArray = std::vector< VkPipelineShaderStageCreateInfo >;

            ShaderModuleArray createShaderModules( RenderDevice *renderDevice, ShaderProgram *program ) const noexcept;
            ShaderStageArray createShaderStages( const ShaderModuleArray &modules ) const noexcept;
            VkPipelineShaderStageCreateInfo createShaderStage( const ShaderModule &module ) const noexcept;
            VkPipelineVertexInputStateCreateInfo createVertexInput( void ) const noexcept;
            VkPipelineInputAssemblyStateCreateInfo createInputAssemby( Primitive::Type primitiveType ) const noexcept;
            VkViewport createViewport( const Rectf &viewport ) const noexcept;
            VkRect2D createScissor( const Rectf &scissor ) const noexcept;
            VkPipelineViewportStateCreateInfo createViewportState( const VkViewport &viewport, const VkRect2D &scissor ) const noexcept;
            VkPipelineRasterizationStateCreateInfo createRasterizer( void ) const noexcept;
            VkPipelineMultisampleStateCreateInfo createMultiplesampleState( void ) const noexcept;
            VkPipelineDepthStencilStateCreateInfo createDepthStencilState( void ) const noexcept;
            VkPipelineColorBlendAttachmentState createColorBlendAttachment( void ) const noexcept;
            VkPipelineColorBlendStateCreateInfo createColorBlending( const VkPipelineColorBlendAttachmentState &colorBlendAttachment ) const noexcept;
            void createPipelineLayout( void );

        private:
            RenderDevice *m_renderDevice = nullptr;
        };

	}

}
	
#endif
	
