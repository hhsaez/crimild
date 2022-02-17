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

#include "Rendering/VulkanSwapchainOLD.hpp"

#include "Rendering/VulkanPhysicalDeviceOLD.hpp"
#include "Rendering/VulkanRenderDeviceOLD.hpp"
#include "Rendering/VulkanSurface.hpp"
#include "Simulation/Simulation.hpp"
#include "VulkanImage.hpp"
#include "VulkanSemaphore.hpp"

using namespace crimild;
using namespace crimild::vulkan;

//////////////////////
// DELETE FROM HERE //
//////////////////////

vulkan::SwapchainOLD::~SwapchainOLD( void ) noexcept
{
    if ( manager != nullptr ) {
        manager->destroy( this );
    }
}

vulkan::SwapchainOLD::AcquireImageResult vulkan::SwapchainOLD::acquireNextImage( const Semaphore *imageAvailableSemaphore ) const noexcept
{
    crimild::UInt32 imageIndex;
    auto ret = vkAcquireNextImageKHR(
        renderDevice->handler,
        handler,
        std::numeric_limits< uint64_t >::max(), // disable timeout
        imageAvailableSemaphore->handler,
        VK_NULL_HANDLE,
        &imageIndex );

    return AcquireImageResult {
        .imageIndex = imageIndex,
        .success = ( ret == VK_SUCCESS ),
        .outOfDate = ( ret == VK_ERROR_OUT_OF_DATE_KHR ),
    };
}

vulkan::SwapchainOLD::PresentImageResult vulkan::SwapchainOLD::presentImage( crimild::UInt32 imageIndex, const Semaphore *signal ) const noexcept
{
    VkSemaphore signalSemaphores[] = {
        signal->handler,
    };

    VkSwapchainKHR swapchains[] = {
        handler,
    };

    auto presentInfo = VkPresentInfoKHR {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signalSemaphores,
        .swapchainCount = 1,
        .pSwapchains = swapchains,
        .pImageIndices = &imageIndex,
        .pResults = nullptr,
    };

    auto ret = vkQueuePresentKHR(
        renderDevice->presentQueue,
        &presentInfo );

    return PresentImageResult {
        .success = ( ret == VK_SUCCESS ),
        .outOfDate = ( ret == VK_ERROR_OUT_OF_DATE_KHR || VK_SUBOPTIMAL_KHR ),
    };
}

SharedPointer< vulkan::SwapchainOLD > SwapchainManager::create( SwapchainOLD::Descriptor const &descriptor ) noexcept
{
    return nullptr;
}

void SwapchainManager::destroy( SwapchainOLD *swapchain ) noexcept
{
    CRIMILD_LOG_TRACE( "Destroying swapchain" );

    swapchain->getImageViews().clear();
    swapchain->getImages().clear();

    if ( swapchain->handler != VK_NULL_HANDLE ) {
        vkDestroySwapchainKHR( swapchain->renderDevice->handler, swapchain->handler, nullptr );
    }

    swapchain->handler = VK_NULL_HANDLE;
    swapchain->renderDevice = nullptr;
    swapchain->manager = nullptr;
    erase( swapchain );
}
