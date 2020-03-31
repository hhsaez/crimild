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

#ifndef CRIMILD_VULKAN_RENDERING_IMAGE_
#define CRIMILD_VULKAN_RENDERING_IMAGE_

#include "Rendering/Image.hpp"
#include "Rendering/VulkanRenderResource.hpp"

namespace crimild {

	namespace vulkan {

#if 0

		class RenderDevice;
        class ImageManager;

		class Image : public VulkanObject {
            CRIMILD_IMPLEMENT_RTTI( crimild::vulkan::Image )

        public:
            struct Descriptor {
                RenderDevice *renderDevice;
                crimild::UInt32 width;
                crimild::UInt32 height;
                VkFormat format;
                VkImageTiling tiling;
                VkImageUsageFlags usage;
                VkMemoryPropertyFlags properties;
                crimild::UInt32 mipLevels = 1;
                VkSampleCountFlagBits numSamples = VK_SAMPLE_COUNT_1_BIT;
            };

		public:
			~Image( void );

            VkImage handler = VK_NULL_HANDLE;
            VkDeviceMemory memoryHandler = VK_NULL_HANDLE;
            RenderDevice *renderDevice = nullptr;
            ImageManager *manager = nullptr;
		};

        /**
           It migth be possible to create an image with a null device, in which
           case the image handler won't be destroyed since the image is managed
           elsewhere (\see Swapchain)
         */
        class ImageManager : public VulkanObjectManager< Image > {
        public:
            explicit ImageManager( RenderDevice *renderDevice = nullptr ) noexcept : m_renderDevice( renderDevice ) { }
            virtual ~ImageManager( void ) noexcept = default;

            SharedPointer< Image > create( Image::Descriptor const &descriptor ) noexcept;
            void attach( Image *image ) noexcept;
            void destroy( Image *image ) noexcept override;

        private:
            RenderDevice *m_renderDevice = nullptr;
        };

#endif
        struct ImageBindInfo {
            VkImage imageHandler = VK_NULL_HANDLE;
            VkDeviceMemory imageMemoryHandler = VK_NULL_HANDLE;
        };

        class ImageManager : public BasicRenderResourceManagerImpl< Image, ImageBindInfo > {
            using ManagerImpl = BasicRenderResourceManagerImpl< Image, ImageBindInfo >;

        public:
            virtual ~ImageManager( void ) = default;

            crimild::Bool bind( Image *image ) noexcept override;
            crimild::Bool unbind( Image *image ) noexcept override;
        };

	}

}
	
#endif
	
