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

#include "VulkanDescriptorPool.hpp"

#include "Rendering/VulkanRenderDeviceOLD.hpp"

using namespace crimild;
using namespace crimild::vulkan;

crimild::Bool DescriptorPoolManager::bind( DescriptorPool *descriptorPool ) noexcept
{
    if ( validate( descriptorPool ) ) {
        return true;
    }

    CRIMILD_LOG_TRACE();

    auto renderDevice = getRenderDevice();
    if ( renderDevice == nullptr ) {
        return false;
    }

    auto swapchain = renderDevice->getSwapchain();

    std::vector< VkDescriptorPoolSize > poolSizes( descriptorPool->layout->bindings.size() );
    for ( auto i = 0l; i < poolSizes.size(); ++i ) {
        auto &binding = descriptorPool->layout->bindings[ i ];
        poolSizes[ i ] = {
            .type = utils::getVulkanDescriptorType( binding.descriptorType ),
            .descriptorCount = static_cast< crimild::UInt32 >( swapchain->getImages().size() ),
        };
    }

    auto createInfo = VkDescriptorPoolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = 0,
        .maxSets = static_cast< crimild::UInt32 >( swapchain->getImages().size() ),
        .poolSizeCount = static_cast< crimild::UInt32 >( poolSizes.size() ),
        .pPoolSizes = poolSizes.data(),
    };

    VkDescriptorPool handler;
    CRIMILD_VULKAN_CHECK(
        vkCreateDescriptorPool(
            renderDevice->handler,
            &createInfo,
            nullptr,
            &handler ) );

    setHandler( descriptorPool, handler );

    return ManagerImpl::bind( descriptorPool );
}

crimild::Bool DescriptorPoolManager::unbind( DescriptorPool *descriptorPool ) noexcept
{
    if ( !validate( descriptorPool ) ) {
        return false;
    }

    CRIMILD_LOG_TRACE();

    auto renderDevice = getRenderDevice();
    if ( renderDevice == nullptr ) {
        CRIMILD_LOG_ERROR( "No valid render device instance" );
        return false;
    }

    auto handler = getHandler( descriptorPool );
    if ( handler != VK_NULL_HANDLE ) {
        vkDestroyDescriptorPool(
            renderDevice->handler,
            handler,
            nullptr );
    }

    removeHandlers( descriptorPool );

    return ManagerImpl::unbind( descriptorPool );
}
