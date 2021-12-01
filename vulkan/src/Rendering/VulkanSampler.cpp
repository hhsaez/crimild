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

#include "Rendering/VulkanSampler.hpp"
#include "Rendering/VulkanPhysicalDevice.hpp"
#include "Rendering/VulkanRenderDevice.hpp"

using namespace crimild::vulkan;

crimild::Bool SamplerManager::bind( Sampler *sampler ) noexcept
{
    if ( validate( sampler ) ) {
        return true;
    }

    CRIMILD_LOG_TRACE( "Binding Vulkan Sampler" );

    auto renderDevice = getRenderDevice();

    // Assume no layers excepts in the case where we're dealing with cubemaps
    auto addressMode = utils::getSamplerAddressMode( sampler->getWrapMode() );
    auto compareOp = utils::getCompareOp( sampler->getCompareOp() );
    auto borderColor = utils::getBorderColor( sampler->getBorderColor() );
	auto minLod = crimild::Real32( sampler->getMinLod() );
	auto maxLod = crimild::Real32( sampler->getMaxLod() );

    auto samplerInfo = VkSamplerCreateInfo {
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = utils::getSamplerFilter( sampler->getMagFilter() ),
        .minFilter = utils::getSamplerFilter( sampler->getMinFilter() ),
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU = addressMode,
        .addressModeV = addressMode,
        .addressModeW = addressMode,
        .mipLodBias = 0,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy = 16,
        .compareEnable = VK_FALSE,
        .compareOp = compareOp,
        .minLod = minLod,
        .maxLod = maxLod,
        .borderColor = borderColor,
        .unnormalizedCoordinates = VK_FALSE,
    };

	SamplerBindInfo bindInfo;
    CRIMILD_VULKAN_CHECK(
        vkCreateSampler(
            renderDevice->handler,
            &samplerInfo,
            nullptr,
            &bindInfo.sampler
        )
    );

    setBindInfo( sampler, bindInfo );

    return ManagerImpl::bind( sampler );
}

crimild::Bool SamplerManager::unbind( Sampler *sampler ) noexcept
{
    if ( !validate( sampler ) ) {
        return false;
    }

    CRIMILD_LOG_TRACE( "Unbind Vulkan Sampler" );

    auto renderDevice = getRenderDevice();
	auto bindInfo = getBindInfo( sampler );

	if ( renderDevice != nullptr && bindInfo.sampler != VK_NULL_HANDLE ) {
		vkDestroySampler(
			renderDevice->handler,
			bindInfo.sampler,
			nullptr
		);
	}

    return ManagerImpl::unbind( sampler );
}
