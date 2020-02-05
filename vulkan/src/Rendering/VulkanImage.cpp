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

#include "VulkanImage.hpp"
#include "VulkanRenderDevice.hpp"
#include "Foundation/Log.hpp"

using namespace crimild;
using namespace crimild::vulkan;

crimild::vulkan::Image::~Image( void )
{
    if ( manager != nullptr ) {
        manager->destroy( this );
    }
}

SharedPointer< crimild::vulkan::Image > ImageManager::create( Image::Descriptor const &descriptor ) noexcept
{
    CRIMILD_LOG_TRACE( "Creating Vulkan image" );

    auto renderDevice = m_renderDevice;
    if ( renderDevice == nullptr ) {
        renderDevice = descriptor.renderDevice;
    }

    VkImage imageHandler;
    VkDeviceMemory imageMemoryHandler;

    utils::createImage(
        renderDevice,
        utils::ImageDescriptor {
            .width = descriptor.width,
            .height = descriptor.height,
            .format = descriptor.format,
            .tiling = descriptor.tiling,
            .usage = descriptor.usage,
            .properties = descriptor.properties,
        	.mipLevels = descriptor.mipLevels,
        	.numSamples = descriptor.numSamples,
    	},
        imageHandler,
        imageMemoryHandler
    );

    auto image = crimild::alloc< Image >();
    image->renderDevice = renderDevice;
    image->handler = imageHandler;
    image->memoryHandler = imageMemoryHandler;
    image->manager = this;
    insert( crimild::get_ptr( image ) );
    return image;
}

void ImageManager::attach( Image *image ) noexcept
{
    image->manager = this;
    insert( image );
}

void ImageManager::destroy( Image *image ) noexcept
{
    CRIMILD_LOG_DEBUG( "Destroying Vulkan image" );

    if ( image->renderDevice != nullptr ) {

    	if ( image->handler != VK_NULL_HANDLE ) {
        	vkDestroyImage( image->renderDevice->handler, image->handler, nullptr );
        }

        if ( image->memoryHandler != VK_NULL_HANDLE ) {
            vkFreeMemory( image->renderDevice->handler, image->memoryHandler, nullptr );
        }
    }

    image->handler = VK_NULL_HANDLE;
    image->memoryHandler = VK_NULL_HANDLE;
    image->manager = nullptr;
    image->renderDevice = nullptr;
    erase( image );
}
