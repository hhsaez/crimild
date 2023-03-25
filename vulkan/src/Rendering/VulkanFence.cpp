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
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Rendering/VulkanFence.hpp"

#include "Foundation/VulkanUtils.hpp"
#include "Rendering/VulkanRenderDevice.hpp"

using namespace crimild::vulkan;

Fence::Fence( RenderDevice *device, std::string name, bool signaled ) noexcept
    : WithRenderDevice( device ),
      Named( name )
{
    const auto createInfo = VkFenceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : VkFenceCreateFlags {},
    };

    VkFence handle;
    CRIMILD_VULKAN_CHECK(
        vkCreateFence(
            getRenderDevice()->getHandle(),
            &createInfo,
            getRenderDevice()->getAllocator(),
            &handle
        )
    );
    setHandle( handle );

    getRenderDevice()->setObjectName(
        uint64_t( getHandle() ),
        VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT,
        name
    );
}

Fence::~Fence( void ) noexcept
{
    vkDestroyFence( getRenderDevice()->getHandle(), getHandle(), getRenderDevice()->getAllocator() );
    setHandle( VK_NULL_HANDLE );
}
