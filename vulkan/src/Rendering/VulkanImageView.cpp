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

#include "VulkanImageView.hpp"
#include "VulkanImage.hpp"
#include "VulkanRenderDevice.hpp"
#include "Exceptions/VulkanException.hpp"
#include "Foundation/Log.hpp"

using namespace crimild::vulkan;

ImageView::ImageView( VulkanRenderDevice *device, SharedPointer< Image > const &image, VkFormat format, VkImageAspectFlags aspectFlags, crimild::UInt32 mipLevels )
	: m_device( device ),
	  m_image( image )
{
	CRIMILD_LOG_TRACE( "Creating image view" );
	
	auto viewInfo = VkImageViewCreateInfo {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = m_image->getImageHandler(),

		// We're dealing with 2D images
		.viewType = VK_IMAGE_VIEW_TYPE_2D,

		// Match the specified format
		.format = format,

		// We don't need to swizzle (swap around) any of the color components
		.components.r = VK_COMPONENT_SWIZZLE_R,
		.components.g = VK_COMPONENT_SWIZZLE_G,
		.components.b = VK_COMPONENT_SWIZZLE_B,
		.components.a = VK_COMPONENT_SWIZZLE_A,

		// Determine what is affected by the image operations (color, depth, stencil, etc)
		.subresourceRange.aspectMask = aspectFlags,
		
		.subresourceRange.levelCount = mipLevels,
		.subresourceRange.baseArrayLayer = 0,
		.subresourceRange.layerCount = 1,

		// optional
		.flags = 0
	};

	if ( vkCreateImageView( device->getDeviceHandler(), &viewInfo, nullptr, &m_imageViewHandler ) != VK_SUCCESS ) {
		throw VulkanException( "Failed to create image view" );
	}
}

ImageView::~ImageView( void ) noexcept
{
	CRIMILD_LOG_TRACE( "Destroying image view" );
	
	if ( m_device != nullptr && m_imageViewHandler != VK_NULL_HANDLE ) {
		vkDestroyImageView(
			m_device->getDeviceHandler(),
			m_imageViewHandler,
			nullptr
		);
	}

	m_device = nullptr;
	m_image = nullptr;
	m_imageViewHandler = VK_NULL_HANDLE;
}

