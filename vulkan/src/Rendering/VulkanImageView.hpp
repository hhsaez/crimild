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

#ifndef CRIMILD_VULKAN_RENDERING_IMAGE_VIEW_
#define CRIMILD_VULKAN_RENDERING_IMAGE_VIEW_

#include "Rendering/ImageView.hpp"
#include "Rendering/VulkanRenderResource.hpp"

namespace crimild {

	namespace vulkan {

#if 0

		class RenderDevice;
		class Image;
        class ImageViewManager;

		/**
		 */
		class ImageView : public VulkanObject {
            CRIMILD_IMPLEMENT_RTTI( crimild::vulkan::ImageView )

        public:
            struct Descriptor {
                RenderDevice *renderDevice;
                SharedPointer< Image > image;
                VkImageViewType imageType = VK_IMAGE_VIEW_TYPE_2D;
                VkFormat format;
                VkImageAspectFlags aspectFlags;
                crimild::UInt32 mipLevels = 1;
                crimild::UInt32 layerCount = 1;
            };

		public:
            ~ImageView( void ) noexcept;

            RenderDevice *renderDevice = nullptr;
            SharedPointer< Image > image;
            VkImageView handler = VK_NULL_HANDLE;
            ImageViewManager *manager = nullptr;
		};

        class ImageViewManager : public VulkanObjectManager< ImageView > {
        public:
            explicit ImageViewManager( RenderDevice *renderDevice = nullptr ) noexcept : m_renderDevice( renderDevice ) { }
            virtual ~ImageViewManager( void ) noexcept = default;

            SharedPointer< ImageView > create( ImageView::Descriptor const &descriptor ) noexcept;
            void destroy( ImageView *imageView ) noexcept override;

        private:
            RenderDevice *m_renderDevice = nullptr;
        };

#endif

        class ImageViewManager : public BasicRenderResourceManagerImpl< ImageView, VkImageView > {
            using ManagerImpl = BasicRenderResourceManagerImpl< ImageView, VkImageView >;

        public:
            virtual ~ImageViewManager( void ) = default;

            crimild::Bool bind( ImageView *imageView ) noexcept override;
            crimild::Bool unbind( ImageView *imageView ) noexcept override;
        };

	}

}
	
#endif
	
