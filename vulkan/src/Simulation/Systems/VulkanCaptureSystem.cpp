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

#include "Simulation/Systems/VulkanCaptureSystem.hpp"

#include "Simulation/FileSystem.hpp"
#include "Simulation/Systems/VulkanSystem.hpp"

using namespace crimild;

// Take a screenshot from the current swapchain image
// This is done using a blit from the swapchain image to a linear image whose memory content is then saved as a ppm image
// Getting the image date directly from a swapchain image wouldn't work as they're usually stored in an implementation dependant optimal tiling format
// Note: This requires the swapchain images to be created with the VK_IMAGE_USAGE_TRANSFER_SRC_BIT flag (see VulkanSwapChain::create)
Bool vulkan::CaptureSystem::takeScreenshot( void ) noexcept
{
    static auto capturing = false;

    if ( capturing ) {
        return false;
    }

    auto physicalDevice = VulkanSystem::getInstance()->getPhysicalDevice();
    auto swapchain = VulkanSystem::getInstance()->getSwapchain();
    auto renderDevice = VulkanSystem::getInstance()->getRenderDevice();

    CRIMILD_LOG_DEBUG( "Waiting for render device to finish" );
    renderDevice->waitIdle();

    CRIMILD_LOG_TRACE();

    capturing = true;

    bool supportsBlit = true;

    auto width = swapchain->extent.width;
    auto height = swapchain->extent.height;

    // Check blit support for source and destination
    VkFormatProperties formatProps;

    CRIMILD_LOG_DEBUG( "Checking device capabilities..." );

    // Check if the device supports blitting from optimal images (the swapchain images are in optimal format)
    vkGetPhysicalDeviceFormatProperties( physicalDevice->handler, swapchain->format, &formatProps );
    if ( !( formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT ) ) {
        CRIMILD_LOG_WARNING( "Device does not support blitting from optimal tiled images, using copy instead of blit!" );
        supportsBlit = false;
    }

    // Check if the device supports blitting to linear images
    vkGetPhysicalDeviceFormatProperties( physicalDevice->handler, VK_FORMAT_R8G8B8A8_UNORM, &formatProps );
    if ( !( formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT ) ) {
        std::cerr << "Device does not support blitting to linear tiled images, using copy instead of blit!" << std::endl;
        supportsBlit = false;
    }

    // Source for the copy is the last rendered swapchain image
    VkImage srcImage = renderDevice->getBindInfo( crimild::get_ptr( swapchain->getImages()[ VulkanSystem::getInstance()->getCurrentFrameId() ] ) ).imageHandler;

    CRIMILD_LOG_DEBUG( "Creating destination image..." );

    // Create the linear tiled destination image to copy to and to read the memory from
    VkImage dstImage;
    VkDeviceMemory dstImageMemory;
    utils::createImage(
        renderDevice,
        {
            .width = width,
            .height = height,
            // Note that vkCmdBlitImage (if supported) will also do format conversions if the swapchain color format would differ
            // https://community.khronos.org/t/vkqueuesubmit-frozen-when-the-commad-buffer-contains-vkcmdcopyimage/105412/4
            .format = VK_FORMAT_B8G8R8A8_UNORM, //VK_FORMAT_R8G8B8A8_UNORM,
            .tiling = VK_IMAGE_TILING_LINEAR,
            .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            .memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            .mipLevels = 1,
            .numSamples = VK_SAMPLE_COUNT_1_BIT,
            .arrayLayers = 1,
        },
        dstImage,
        dstImageMemory );

    CRIMILD_LOG_DEBUG( "Creating command buffers..." );

    // Do the actual blit from the swapchain image to our host visible destination image
    auto copyCmd = utils::beginSingleTimeCommands( renderDevice );

    auto insertImageMemoryBarrier = [](
                                        VkCommandBuffer cmdbuffer,
                                        VkImage image,
                                        VkAccessFlags srcAccessMask,
                                        VkAccessFlags dstAccessMask,
                                        VkImageLayout oldImageLayout,
                                        VkImageLayout newImageLayout,
                                        VkPipelineStageFlags srcStageMask,
                                        VkPipelineStageFlags dstStageMask,
                                        VkImageSubresourceRange subresourceRange ) {
        VkImageMemoryBarrier imageMemoryBarrier {};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.srcAccessMask = srcAccessMask;
        imageMemoryBarrier.dstAccessMask = dstAccessMask;
        imageMemoryBarrier.oldLayout = oldImageLayout;
        imageMemoryBarrier.newLayout = newImageLayout;
        imageMemoryBarrier.image = image;
        imageMemoryBarrier.subresourceRange = subresourceRange;

        vkCmdPipelineBarrier(
            cmdbuffer,
            srcStageMask,
            dstStageMask,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &imageMemoryBarrier );
    };

    CRIMILD_LOG_DEBUG( "Transitioning images to destination layout..." );

    // Transition destination image to transfer destination layout
    insertImageMemoryBarrier(
        copyCmd,
        dstImage,
        0,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VkImageSubresourceRange { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 } );

    CRIMILD_LOG_DEBUG( "Transitioning swapchian to transfer layout..." );

    // Transition swapchain image from present to transfer source layout
    insertImageMemoryBarrier(
        copyCmd,
        srcImage,
        VK_ACCESS_MEMORY_READ_BIT,
        VK_ACCESS_TRANSFER_READ_BIT,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VkImageSubresourceRange { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 } );

    // If source and destination support blit we'll blit as this also does automatic format conversion (e.g. from BGR to RGB)
    if ( supportsBlit ) {
        CRIMILD_LOG_DEBUG( "Blitting image..." );

        // Define the region to blit (we will blit the whole swapchain image)
        VkOffset3D blitSize;
        blitSize.x = width;
        blitSize.y = height;
        blitSize.z = 1;
        VkImageBlit imageBlitRegion {};
        imageBlitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBlitRegion.srcSubresource.layerCount = 1;
        imageBlitRegion.srcOffsets[ 1 ] = blitSize;
        imageBlitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBlitRegion.dstSubresource.layerCount = 1;
        imageBlitRegion.dstOffsets[ 1 ] = blitSize;

        // Issue the blit command
        vkCmdBlitImage(
            copyCmd,
            srcImage,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            dstImage,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &imageBlitRegion,
            VK_FILTER_NEAREST );
    } else {
        CRIMILD_LOG_DEBUG( "Copying image..." );

        // Otherwise use image copy (requires us to manually flip components)
        VkImageCopy imageCopyRegion {};
        imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopyRegion.srcSubresource.layerCount = 1;
        imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopyRegion.dstSubresource.layerCount = 1;
        imageCopyRegion.extent.width = width;
        imageCopyRegion.extent.height = height;
        imageCopyRegion.extent.depth = 1;

        // Issue the copy command
        vkCmdCopyImage(
            copyCmd,
            srcImage,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            dstImage,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &imageCopyRegion );
    }

    CRIMILD_LOG_DEBUG( "Transitioning destination images to general layout..." );

    // Transition destination image to general layout, which is the required layout for mapping the image memory later on
    insertImageMemoryBarrier(
        copyCmd,
        dstImage,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_ACCESS_MEMORY_READ_BIT,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_GENERAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VkImageSubresourceRange { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 } );

    CRIMILD_LOG_DEBUG( "Transitioning swapchain back to present layout..." );

    // Transition back the swap chain image after the blit is done
    insertImageMemoryBarrier(
        copyCmd,
        srcImage,
        VK_ACCESS_TRANSFER_READ_BIT,
        VK_ACCESS_MEMORY_READ_BIT,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VkImageSubresourceRange { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 } );

    CRIMILD_LOG_DEBUG( "Executing commands..." );
    utils::endSingleTimeCommands( renderDevice, copyCmd );

    renderDevice->waitIdle();

    CRIMILD_LOG_DEBUG( "Getting image layout..." );

    // Get layout of the image (including row pitch)
    VkImageSubresource subResource { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0 };
    VkSubresourceLayout subResourceLayout;
    vkGetImageSubresourceLayout( renderDevice->handler, dstImage, &subResource, &subResourceLayout );

    CRIMILD_LOG_DEBUG( "Mapping memory..." );

    // Map image memory so we can start copying from it
    const char *data;
    vkMapMemory( renderDevice->handler, dstImageMemory, 0, VK_WHOLE_SIZE, 0, ( void ** ) &data );
    data += subResourceLayout.offset;

    CRIMILD_LOG_DEBUG( "Saving image file..." );

    std::string filename = FileSystem::getInstance().pathForDocument( "screenshot.ppm" );
    std::ofstream file( filename, std::ios::out | std::ios::binary );

    // ppm header
    file << "P6\n"
         << width << "\n"
         << height << "\n"
         << 255 << "\n";

    // If source is BGR (destination is always RGB) and we can't use blit (which does automatic conversion), we'll have to manually swizzle color components
    bool colorSwizzle = false;
    // Check if source is BGR
    // Note: Not complete, only contains most common and basic BGR surface formats for demonstation purposes
    if ( !supportsBlit ) {
        std::vector< VkFormat > formatsBGR = { VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_SNORM };
        colorSwizzle = ( std::find( formatsBGR.begin(), formatsBGR.end(), swapchain->format ) != formatsBGR.end() );
    }

    // ppm binary pixel data
    for ( uint32_t y = 0; y < height; y++ ) {
        unsigned int *row = ( unsigned int * ) data;
        for ( uint32_t x = 0; x < width; x++ ) {
            if ( colorSwizzle ) {
                file.write( ( char * ) row + 2, 1 );
                file.write( ( char * ) row + 1, 1 );
                file.write( ( char * ) row, 1 );
            } else {
                file.write( ( char * ) row, 3 );
            }
            row++;
        }
        data += subResourceLayout.rowPitch;
    }
    file.close();

    CRIMILD_LOG_DEBUG( "Screenshot saved..." );

    CRIMILD_LOG_DEBUG( "Cleaning up..." );

    // Clean up resources
    vkUnmapMemory( renderDevice->handler, dstImageMemory );
    vkFreeMemory( renderDevice->handler, dstImageMemory, nullptr );
    vkDestroyImage( renderDevice->handler, dstImage, nullptr );

    CRIMILD_LOG_DEBUG( "Done taking screenshot", filename );

    capturing = false;

    return true;
}
