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

#include "Rendering/VulkanDescriptorPool.hpp"

#include "Rendering/VulkanRenderDevice.hpp"

using namespace crimild::vulkan;

DescriptorPool::DescriptorPool(
    RenderDevice *device,
    std::string name,
    const VkDescriptorPoolCreateInfo &info
) noexcept
    : Named( name ),
      WithRenderDevice( device )
{
    VkDescriptorPool handle;
    CRIMILD_VULKAN_CHECK(
        vkCreateDescriptorPool(
            getRenderDevice()->getHandle(),
            &info,
            getRenderDevice()->getAllocator(),
            &handle
        )
    );
    setHandle( handle );

    if ( !name.empty() ) {
        device->setObjectName( getHandle(), name );
    }
}

DescriptorPool::DescriptorPool(
    RenderDevice *device,
    std::string name,
    const std::vector< VkDescriptorPoolSize > &sizes
) noexcept
    : DescriptorPool(
        device,
        name,
        VkDescriptorPoolCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = 1,
            .poolSizeCount = uint32_t( sizes.size() ),
            .pPoolSizes = sizes.data(),
        }
    )
{
    // no-op
}

DescriptorPool::DescriptorPool(
    RenderDevice *device,
    std::string name,
    const std::vector< Descriptor > &descriptors
) noexcept
    : DescriptorPool(
        device,
        name,
        [ & ] {
            std::vector< VkDescriptorPoolSize > poolSizes;
            std::transform(
                descriptors.begin(),
                descriptors.end(),
                std::back_inserter( poolSizes ),
                []( auto &descriptor ) {
                    return VkDescriptorPoolSize {
                        .type = descriptor.type,
                        .descriptorCount = 1,
                    };
                }
            );
            return poolSizes;
        }()
    )
{
    // no-op
}

DescriptorPool::~DescriptorPool( void ) noexcept
{
    if ( getHandle() != VK_NULL_HANDLE ) {
        vkDestroyDescriptorPool(
            getRenderDevice()->getHandle(),
            getHandle(),
            getRenderDevice()->getAllocator()
        );
        setHandle( VK_NULL_HANDLE );
    }
}
