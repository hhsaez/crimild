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

#include "Rendering/VulkanDescriptorSetLayout.hpp"

#include "Rendering/VulkanRenderDeviceOLD.hpp"

using namespace crimild;
using namespace crimild::vulkan;

crimild::Bool DescriptorSetLayoutManager::bind( DescriptorSetLayout *descriptorSetLayout ) noexcept
{
    if ( validate( descriptorSetLayout ) ) {
        return true;
    }

    CRIMILD_LOG_TRACE( "Binding Vulkan descriptor set layout" );

    auto renderDevice = getRenderDevice();
    if ( renderDevice == nullptr ) {
        return false;
    }

    std::vector< VkDescriptorSetLayoutBinding > bindings( descriptorSetLayout->bindings.size() );
    for ( auto i = 0l; i < bindings.size(); i++ ) {
        auto binding = descriptorSetLayout->bindings[ i ];
        bindings[ i ] = VkDescriptorSetLayoutBinding {
            .binding = static_cast< crimild::UInt32 >( i ),
            .descriptorType = utils::getVulkanDescriptorType( binding.descriptorType ),
            .descriptorCount = 1,
            .stageFlags = static_cast< VkShaderStageFlags >( utils::getVulkanShaderStageFlag( binding.stage ) ),
            .pImmutableSamplers = nullptr // optional
        };
    }

    auto createInfo = VkDescriptorSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = static_cast< crimild::UInt32 >( bindings.size() ),
        .pBindings = bindings.data(),
    };

    VkDescriptorSetLayout descriptorSetLayoutHandler;
    CRIMILD_VULKAN_CHECK(
        vkCreateDescriptorSetLayout(
            renderDevice->handler,
            &createInfo,
            nullptr,
            &descriptorSetLayoutHandler ) );

    setHandler( descriptorSetLayout, descriptorSetLayoutHandler );

    return ManagerImpl::bind( descriptorSetLayout );
}

crimild::Bool DescriptorSetLayoutManager::unbind( DescriptorSetLayout *descriptorSetLayout ) noexcept
{
    if ( !validate( descriptorSetLayout ) ) {
        return false;
    }

    CRIMILD_LOG_TRACE( "Unbinding Vulkan descriptor set layout" );

    auto handler = getHandler( descriptorSetLayout );
    if ( handler == VK_NULL_HANDLE ) {
        return false;
    }

    auto renderDevice = getRenderDevice();
    if ( renderDevice != nullptr ) {
        vkDestroyDescriptorSetLayout(
            renderDevice->handler,
            handler,
            nullptr );
    }

    removeHandlers( descriptorSetLayout );

    return ManagerImpl::unbind( descriptorSetLayout );
}
