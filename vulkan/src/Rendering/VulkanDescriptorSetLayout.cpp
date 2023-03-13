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

#include "Rendering/VulkanDescriptorSetLayout.hpp"

#include "Rendering/VulkanDescriptor.hpp"
#include "Rendering/VulkanRenderDevice.hpp"

using namespace crimild::vulkan;

DescriptorSetLayout::DescriptorSetLayout(
    RenderDevice *device,
    std::string name,
    const VkDescriptorSetLayoutCreateInfo &info
) noexcept
    : Named( name ),
      WithRenderDevice( device )
{
    VkDescriptorSetLayout handle;
    CRIMILD_VULKAN_CHECK(
        vkCreateDescriptorSetLayout(
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

DescriptorSetLayout::DescriptorSetLayout(
    RenderDevice *device,
    std::string name,
    const std::vector< VkDescriptorSetLayoutBinding > &bindings
) noexcept
    : DescriptorSetLayout(
        device,
        name,
        VkDescriptorSetLayoutCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = uint32_t( bindings.size() ),
            .pBindings = bindings.data(),
        }
    )
{
    // no-op
}

DescriptorSetLayout::DescriptorSetLayout(
    RenderDevice *device,
    std::string name,
    const std::vector< Descriptor > &descriptors
) noexcept
    : DescriptorSetLayout(
        device,
        name,
        [ & ] {
            std::vector< VkDescriptorSetLayoutBinding > bindings;
            std::transform(
                descriptors.begin(),
                descriptors.end(),
                std::back_inserter( bindings ),
                [ binding = uint32_t( 0 ) ]( auto &descriptor ) mutable {
                    return VkDescriptorSetLayoutBinding {
                        .binding = binding++,
                        .descriptorType = descriptor.type,
                        .descriptorCount = 1,
                        .stageFlags = descriptor.stage,
                        .pImmutableSamplers = nullptr,
                    };
                }
            );
            return bindings;
        }()
    )
{
    // no-op
}

DescriptorSetLayout::~DescriptorSetLayout( void ) noexcept
{
    vkDestroyDescriptorSetLayout(
        getRenderDevice()->getHandle(),
        getHandle(),
        getRenderDevice()->getAllocator()
    );
    setHandle( VK_NULL_HANDLE );
}
