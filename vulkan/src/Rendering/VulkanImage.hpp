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

        struct ImageBindInfo {
            VkImage imageHandler = VK_NULL_HANDLE;
            VkDeviceMemory imageMemoryHandler = VK_NULL_HANDLE;

            /**
             * \brief Staging buffer for dynamic images
             * 
             * Dynamic images are updated using a staging buffer, which is created
             * during image binding. Static images also use a staging buffer but they
             * release it as soon as possible.
             */
            VkBuffer stagingBuffer = VK_NULL_HANDLE;
            VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
        };

        class ImageManager : public BasicRenderResourceManagerImpl< Image, ImageBindInfo > {
            using ManagerImpl = BasicRenderResourceManagerImpl< Image, ImageBindInfo >;

        public:
            virtual ~ImageManager( void ) = default;

            crimild::Bool bind( Image *image ) noexcept override;
            crimild::Bool unbind( Image *image ) noexcept override;

            void updateImages( void ) noexcept;
        };

    }

}

#endif
