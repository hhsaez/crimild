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

#include "VulkanFramebuffer.hpp"
#include "VulkanRenderDevice.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanImageView.hpp"

using namespace crimild::vulkan;

Framebuffer::Framebuffer( const VulkanRenderDevice *device, const Descriptor &descriptor )
	: m_device( device ),
	  m_extent( descriptor.extent )
{
	CRIMILD_LOG_TRACE( "Creating framebuffer" );

	std::vector< VkImageView > attachments;
	for ( const auto &att : descriptor.attachments ) {
		attachments.push_back( att->getImageViewHandler() );
	}
	
	auto createInfo = VkFramebufferCreateInfo {
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.renderPass = descriptor.renderPass->getRenderPassHandler(),
		.attachmentCount = static_cast< uint32_t >( attachments.size() ),
		.pAttachments = attachments.data(),
		.width = m_extent.width,
		.height = m_extent.height,
		.layers = 1,
	};

	CRIMILD_VULKAN_CHECK(
		vkCreateFramebuffer(
			m_device->getDeviceHandler(),
			&createInfo,
			nullptr,
			&m_framebufferHandler
		)
	);
}

Framebuffer::~Framebuffer( void ) noexcept
{
	CRIMILD_LOG_TRACE( "Destroying framebuffer" );
	
	if ( m_device != nullptr && m_framebufferHandler != VK_NULL_HANDLE ) {
		vkDestroyFramebuffer(
			m_device->getDeviceHandler(),
			m_framebufferHandler,
			nullptr
		);
		m_framebufferHandler = VK_NULL_HANDLE;
	}
}

