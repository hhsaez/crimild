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

#include "VulkanSemaphore.hpp"

#include "VulkanRenderDeviceOLD.hpp"

using namespace crimild;
using namespace crimild::vulkan;

Semaphore::~Semaphore( void )
{
    if ( manager != nullptr ) {
        manager->destroy( this );
    }
}

SharedPointer< Semaphore > SemaphoreManager::create( Semaphore::Descriptor const &descriptor ) noexcept
{
    CRIMILD_LOG_TRACE();

    auto renderDevice = m_renderDevice;
    if ( renderDevice == nullptr ) {
        renderDevice = descriptor.renderDevice;
    }

    auto createInfo = VkSemaphoreCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    VkSemaphore semaphoreHandler;
    CRIMILD_VULKAN_CHECK(
        vkCreateSemaphore(
            renderDevice->handler,
            &createInfo,
            nullptr,
            &semaphoreHandler ) );

    auto semaphore = crimild::alloc< Semaphore >();
    semaphore->renderDevice = renderDevice;
    semaphore->handler = semaphoreHandler;
    semaphore->manager = this;
    insert( crimild::get_ptr( semaphore ) );
    return semaphore;
}

void SemaphoreManager::destroy( Semaphore *semaphore ) noexcept
{
    CRIMILD_LOG_TRACE();

    if ( semaphore->renderDevice != nullptr && semaphore->handler != VK_NULL_HANDLE ) {
        vkDestroySemaphore(
            semaphore->renderDevice->handler,
            semaphore->handler,
            nullptr );
    }

    semaphore->handler = VK_NULL_HANDLE;
    semaphore->renderDevice = nullptr;
    semaphore->manager = nullptr;
    erase( semaphore );
}
