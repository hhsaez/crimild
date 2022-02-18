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

#include "VulkanFence.hpp"

#include "VulkanRenderDeviceOLD.hpp"

using namespace crimild;
using namespace crimild::vulkan;

Fence::~Fence( void ) noexcept
{
    if ( manager != nullptr ) {
        manager->destroy( this );
    }
}

void Fence::wait( crimild::UInt64 timeout ) const noexcept
{
    vkWaitForFences(
        renderDevice->handler,
        1,
        &handler,
        VK_TRUE,
        timeout );
}

void Fence::reset( void ) const noexcept
{
    vkResetFences(
        renderDevice->handler,
        1,
        &handler );
}

SharedPointer< Fence > FenceManager::create( Fence::Descriptor const &descriptor ) noexcept
{
    CRIMILD_LOG_TRACE();

    auto renderDevice = m_renderDevice;
    if ( renderDevice == nullptr ) {
        renderDevice = descriptor.renderDevice;
    }

    auto fenceInfo = VkFenceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,

        // Initialize fence in signal state so we can wait for it from the beginning
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    VkFence fenceHandler;
    CRIMILD_VULKAN_CHECK(
        vkCreateFence(
            renderDevice->handler,
            &fenceInfo,
            nullptr,
            &fenceHandler ) );

    auto fence = crimild::alloc< Fence >();
    fence->manager = this;
    fence->renderDevice = renderDevice;
    fence->handler = fenceHandler;
    insert( crimild::get_ptr( fence ) );
    return fence;
}

void FenceManager::destroy( Fence *fence ) noexcept
{
    CRIMILD_LOG_TRACE();

    if ( fence->renderDevice != nullptr && fence->handler != VK_NULL_HANDLE ) {
        vkDestroyFence(
            fence->renderDevice->handler,
            fence->handler,
            nullptr );
    }

    fence->handler = VK_NULL_HANDLE;
    fence->renderDevice = nullptr;
    fence->manager = nullptr;
    erase( fence );
}
