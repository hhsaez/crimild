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
 *     * Neither the name of the copyright holder nor the
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

#ifndef CRIMILD_VULKAN_RENDERING_DESCRIPTOR_SET
#define CRIMILD_VULKAN_RENDERING_DESCRIPTOR_SET

#include "Foundation/VulkanUtils.hpp"
#include "Rendering/VulkanDescriptor.hpp"

namespace crimild::vulkan {

    class DescriptorSetLayout;
    class DescriptorPool;

    class DescriptorSet
        : public SharedObject,
          public Named,
          public WithRenderDevice,
          public WithHandle< VkDescriptorSet > {
    public:
        DescriptorSet(
            RenderDevice *device,
            std::string name,
            std::shared_ptr< DescriptorPool > pool,
            std::shared_ptr< DescriptorSetLayout > layout,
            const std::vector< Descriptor > &descriptors
        ) noexcept;

        DescriptorSet(
            RenderDevice *device,
            std::string name,
            const std::vector< Descriptor > &descriptors
        ) noexcept;

        virtual ~DescriptorSet( void ) noexcept;

        inline std::shared_ptr< DescriptorPool > &getDescirptorPool( void ) noexcept { return m_pool; }
        inline std::shared_ptr< DescriptorSetLayout > &getDescriptorSetLayout( void ) noexcept { return m_layout; }

        void updateDescriptors( void ) noexcept;

    private:
        std::shared_ptr< DescriptorPool > m_pool;
        std::shared_ptr< DescriptorSetLayout > m_layout;
        std::vector< Descriptor > m_descriptors;
    };

}

#endif
