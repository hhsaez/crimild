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

#include "VulkanSystem.hpp"

#include "Foundation/Containers/Array.hpp"
#include "Rendering/Programs/SkyboxShaderProgram.hpp"
#include "Rendering/Programs/UnlitShaderProgram.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/VulkanCommandBuffer.hpp"
#include "Rendering/VulkanCommandPool.hpp"
#include "Rendering/VulkanFence.hpp"
#include "Rendering/VulkanPhysicalDevice.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanSwapchain.hpp"
#include "SceneGraph/Camera.hpp"
#include "Simulation/AssetManager.hpp"
#include "Simulation/FileSystem.hpp"
#include "Simulation/Simulation.hpp"
#include "Simulation/Systems/RenderSystem.hpp"

using namespace crimild;
using namespace crimild::vulkan;

void VulkanSystem::start( void ) noexcept
{
    initShaders();

    createInstance()
        && createDebugMessenger()
        && createSurface()
        && createPhysicalDevice()
        && createRenderDevice()
        && createCommandPool()
        && recreateSwapchain();
}

/*
 1. Acquire an image from the swapchain
 2. Execute command buffer with that image as attachment in the framebuffer
 3. Return the image to the swapchain for presentation
*/
void VulkanSystem::onRender( void ) noexcept
{
    auto renderDevice = crimild::get_ptr( m_renderDevice );
    if ( renderDevice == nullptr ) {
        CRIMILD_LOG_ERROR( "No valid render device instance" );
        return;
    }

    // auto graphicsCommandBuffers = m_frameGraph->recordGraphicsCommands( m_currentFrame, m_recordWithNonConditionalPasses );
    // auto computeCommandBuffers = m_frameGraph->recordComputeCommands( m_currentFrame, m_recordWithNonConditionalPasses );

    auto renderSystem = RenderSystem::getInstance();
    auto &graphicsCommandBuffers = renderSystem->getGraphicsCommands( m_currentFrame, m_recordWithNonConditionalPasses );
    auto &computeCommandBuffers = renderSystem->getComputeCommands( m_currentFrame, m_recordWithNonConditionalPasses );

    auto swapchain = crimild::get_ptr( m_swapchain );

    auto wait = crimild::get_ptr( m_imageAvailableSemaphores[ m_currentFrame ] );
    auto signal = crimild::get_ptr( m_renderFinishedSemaphores[ m_currentFrame ] );
    auto fence = crimild::get_ptr( m_inFlightFences[ m_currentFrame ] );

    // Wait for any pending operations to complete
    fence->wait();
    fence->reset();

    // Acquire the next image available image from the swapchain
    // Execution of command buffers is asynchrounous. It must be set up to
    // wait on image adquisition to finish
    auto result = swapchain->acquireNextImage( wait );
    if ( !result.success ) {
        if ( result.outOfDate ) {
            // No image is available since environemnt has changed
            // Maybe window was resized.
            // Recreate swapchain and related objects
            cleanSwapchain();
            recreateSwapchain();
            return;
        } else {
            CRIMILD_LOG_ERROR( "No image available" );
            exit( -1 );
        }
    }

    auto imageIndex = result.imageIndex;

    // TODO: This migth be a bit slow...
    getRenderDevice()->updateImages();
    updateVertexBuffers();
    updateIndexBuffers();
    updateUniformBuffers();

    // Submit graphic commands to the render device, with the selected
    // image as attachment in the framebuffer
    renderDevice->submitGraphicsCommands(
        wait,
        graphicsCommandBuffers,
        imageIndex,
        signal,
        fence );

    // Return the image to the swapchain so it can be presented
    auto presentationResult = swapchain->presentImage( imageIndex, signal );
    if ( !presentationResult.success ) {
        if ( presentationResult.outOfDate ) {
            cleanSwapchain();
            recreateSwapchain();
            return;
        } else {
            CRIMILD_LOG_ERROR( "Failed to present image" );
            exit( -1 );
        }
    }

    computeCommandBuffers.each(
        [ & ]( auto commandBuffer ) {
            renderDevice->submitComputeCommands( crimild::get_ptr( commandBuffer ) );
            renderDevice->waitIdle();
        } );

    m_currentFrame = ( m_currentFrame + 1 ) % swapchain->getImages().size();

    if ( m_recordWithNonConditionalPasses && m_currentFrame == 0 ) {
        // We have been rendering using command buffers that included conditional render passes
        // We need to record all commands again now, without the conditional passes.
        // If m_currentFrame == 0, that means we have rendered all in-flight frames already
        m_recordWithNonConditionalPasses = false;
    }

    if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_F8 ) ) {
        takeScreenshot( FileSystem::getInstance().pathForDocument( "screenshot.ppm" ) );
    }
}

void VulkanSystem::stop( void ) noexcept
{
    System::stop();

    cleanSwapchain();

    m_commandPool = nullptr;
    m_renderDevice = nullptr;
    m_physicalDevice = nullptr;
    m_debugMessenger = nullptr;
    m_surface = nullptr;
    m_instance = nullptr;

    RenderDeviceManager::cleanup();
    PhysicalDeviceManager::cleanup();
    VulkanDebugMessengerManager::cleanup();
    VulkanSurfaceManager::cleanup();
    VulkanInstanceManager::cleanup();
}

crimild::Bool VulkanSystem::createInstance( void ) noexcept
{
    auto settings = Simulation::getInstance()->getSettings();
    auto appName = settings->get< std::string >( Settings::SETTINGS_APP_NAME, "Crimild" );
    auto appVersionMajor = settings->get< crimild::UInt32 >( Settings::SETTINGS_APP_VERSION_MAJOR, 1 );
    auto appVersionMinor = settings->get< crimild::UInt32 >( Settings::SETTINGS_APP_VERSION_MINOR, 0 );
    auto appVersionPatch = settings->get< crimild::UInt32 >( Settings::SETTINGS_APP_VERSION_PATCH, 0 );

    m_instance = create(
        VulkanInstance::Descriptor {
            .appName = appName,
            .appVersionMajor = appVersionMajor,
            .appVersionMinor = appVersionMinor,
            .appVersionPatch = appVersionPatch } );

    return m_instance != nullptr;
}

crimild::Bool VulkanSystem::createDebugMessenger( void ) noexcept
{
    m_debugMessenger = create(
        VulkanDebugMessenger::Descriptor {
            .instance = crimild::get_ptr( m_instance ) } );

    // Never fails (Debug messenger is disabled for Release builds)
    return true;
}

crimild::Bool VulkanSystem::createSurface( void ) noexcept
{
    CRIMILD_LOG_TRACE( "Creating Vulkan surface" );
    m_surface = create(
        VulkanSurface::Descriptor {
            .instance = crimild::get_ptr( m_instance ) } );
    if ( m_surface == nullptr ) {
        CRIMILD_LOG_ERROR( "Failed to create Vulkan surface" );
        return false;
    }

    attach( crimild::get_ptr( m_surface ) );
    CRIMILD_LOG_ERROR( "Vulkan Surface created" );
    return true;
}

crimild::Bool VulkanSystem::createPhysicalDevice( void ) noexcept
{
    m_physicalDevice = create(
        PhysicalDevice::Descriptor {
            .instance = crimild::get_ptr( m_instance ),
            .surface = crimild::get_ptr( m_surface ) } );
    return m_physicalDevice != nullptr;
}

crimild::Bool VulkanSystem::createRenderDevice( void ) noexcept
{
    m_renderDevice = m_physicalDevice->create( RenderDevice::Descriptor {} );
    return m_renderDevice != nullptr;
}

crimild::Bool VulkanSystem::createSwapchain( void ) noexcept
{
    auto settings = Simulation::getInstance()->getSettings();
    auto width = settings->get< crimild::UInt >( "video.width", 0 );
    auto height = settings->get< crimild::UInt >( "video.height", 0 );

    m_swapchain = m_renderDevice->create(
        Swapchain::Descriptor {
            .extent = Vector2ui { width, height } } );

    if ( m_swapchain == nullptr ) {
        return false;
    }

    settings->set( "video.width", m_swapchain->extent.width );
    settings->set( "video.height", m_swapchain->extent.height );

    if ( auto mainCamera = Camera::getMainCamera() ) {
        mainCamera->setAspectRatio( ( crimild::Real32 ) m_swapchain->extent.width / ( crimild::Real32 ) m_swapchain->extent.height );
    }

    // Reset existing command buffers
    m_recordWithNonConditionalPasses = true;

    return true;
}

void VulkanSystem::cleanSwapchain( void ) noexcept
{
    if ( auto renderDevice = crimild::get_ptr( m_renderDevice ) ) {
        CRIMILD_LOG_TRACE( "Waiting for pending operations" );
        m_renderDevice->waitIdle();

        static_cast< DescriptorSetManager * >( renderDevice )->clear();
        static_cast< DescriptorSetLayoutManager * >( renderDevice )->clear();
        static_cast< DescriptorPoolManager * >( renderDevice )->clear();
        static_cast< UniformBufferManager * >( renderDevice )->clear();
        static_cast< CommandBufferManager * >( renderDevice )->clear();
        static_cast< GraphicsPipelineManager * >( renderDevice )->clear();
        static_cast< ComputePipelineManager * >( renderDevice )->clear();
        static_cast< RenderPassManager * >( renderDevice )->clear();
        static_cast< ImageViewManager * >( renderDevice )->clear();
        static_cast< ImageManager * >( renderDevice )->clear();

        renderDevice->reset( crimild::get_ptr( m_commandPool ) );
    }

    m_inFlightFences.clear();
    m_imageAvailableSemaphores.clear();
    m_renderFinishedSemaphores.clear();
    m_swapchain = nullptr;
    m_currentFrame = 0;
}

crimild::Bool VulkanSystem::recreateSwapchain( void ) noexcept
{
    cleanSwapchain();

    return createSwapchain()
           && createSyncObjects();
}

crimild::Bool VulkanSystem::createSyncObjects( void ) noexcept
{
    auto renderDevice = crimild::get_ptr( m_renderDevice );

    for ( auto i = 0l; i < m_swapchain->getImages().size(); i++ ) {
        m_imageAvailableSemaphores.push_back( renderDevice->create( Semaphore::Descriptor {} ) );
        m_renderFinishedSemaphores.push_back( renderDevice->create( Semaphore::Descriptor {} ) );
        m_inFlightFences.push_back( renderDevice->create( Fence::Descriptor {} ) );
    }

    return true;
}

crimild::Bool VulkanSystem::createCommandPool( void ) noexcept
{
    auto renderDevice = crimild::get_ptr( m_renderDevice );
    auto queueFamilyIndices = utils::findQueueFamilies( m_physicalDevice->handler, m_surface->handler );
    m_commandPool = renderDevice->create(
        CommandPool::Descriptor {
            .queueFamilyIndex = queueFamilyIndices.graphicsFamily[ 0 ],
        } );

    return m_commandPool != nullptr;
}

void VulkanSystem::updateVertexBuffers( void ) noexcept
{
    getRenderDevice()->updateVertexBuffers();
}

void VulkanSystem::updateIndexBuffers( void ) noexcept
{
    getRenderDevice()->updateIndexBuffers();
}

void VulkanSystem::updateUniformBuffers( void ) noexcept
{
    getRenderDevice()->updateUniformBuffers( 0 );
}

void VulkanSystem::initShaders( void ) noexcept
{
    auto createShader = []( Shader::Stage stage, const unsigned char *rawData, crimild::Size size ) {
        std::vector< char > data( size + ( size % 4 ) );
        memcpy( &data[ 0 ], rawData, size );
        return crimild::alloc< Shader >( stage, data );
    };

    auto assets = AssetManager::getInstance();

    assets->get< UnlitShaderProgram >()->setShaders(
        {
            [ & ] {
#include "Rendering/Shaders/unlit/unlit.vert.inl"
                return createShader( Shader::Stage::VERTEX, RESOURCE_BYTES, sizeof( RESOURCE_BYTES ) );
            }(),
            [ & ] {
#include "Rendering/Shaders/unlit/unlit.frag.inl"
                return createShader( Shader::Stage::FRAGMENT, RESOURCE_BYTES, sizeof( RESOURCE_BYTES ) );
            }(),
        } );

    assets->get< SkyboxShaderProgram >()->setShaders(
        {
            [ & ] {
#include "Rendering/Shaders/unlit/skybox.vert.inl"
                return createShader( Shader::Stage::VERTEX, RESOURCE_BYTES, sizeof( RESOURCE_BYTES ) );
            }(),
            [ & ] {
#include "Rendering/Shaders/unlit/skybox.frag.inl"
                return createShader( Shader::Stage::FRAGMENT, RESOURCE_BYTES, sizeof( RESOURCE_BYTES ) );
            }(),
        } );
}

// Take a screenshot from the current swapchain image
// This is done using a blit from the swapchain image to a linear image whose memory content is then saved as a ppm image
// Getting the image date directly from a swapchain image wouldn't work as they're usually stored in an implementation dependant optimal tiling format
// Note: This requires the swapchain images to be created with the VK_IMAGE_USAGE_TRANSFER_SRC_BIT flag (see VulkanSwapChain::create)
void VulkanSystem::takeScreenshot( std::string filename ) noexcept
{
    static auto capturing = false;

    if ( capturing ) {
        return;
    }

    auto physicalDevice = m_physicalDevice;
    auto swapchain = m_swapchain;
    auto renderDevice = getRenderDevice();

    CRIMILD_LOG_DEBUG( "Waiting for render device to finish" );
    renderDevice->waitIdle();

    CRIMILD_LOG_TRACE( "Capturing image..." );

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
    VkImage srcImage = renderDevice->getBindInfo( crimild::get_ptr( swapchain->getImages()[ m_currentFrame ] ) ).imageHandler;

    CRIMILD_LOG_DEBUG( "Creating destination image..." );

    // Create the linear tiled destination image to copy to and to read the memory from
    auto imageCreateCI = VkImageCreateInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        // Note that vkCmdBlitImage (if supported) will also do format conversions if the swapchain color format would differ
        // https://community.khronos.org/t/vkqueuesubmit-frozen-when-the-commad-buffer-contains-vkcmdcopyimage/105412/4
        .format = VK_FORMAT_B8G8R8A8_UNORM, //VK_FORMAT_R8G8B8A8_UNORM,
        .extent = VkExtent3D {
            .width = UInt32( width ),
            .height = UInt32( height ),
            .depth = 1,
        },
        .arrayLayers = 1,
        .mipLevels = 1,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_LINEAR,
        .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
    };

    // Create the image
    VkImage dstImage;
    CRIMILD_VULKAN_CHECK(
        vkCreateImage(
            renderDevice->handler,
            &imageCreateCI,
            nullptr,
            &dstImage ) );

    // Create memory to back up the image
    VkMemoryRequirements memRequirements;

    auto memAllocInfo = VkMemoryAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
    };

    VkDeviceMemory dstImageMemory;
    vkGetImageMemoryRequirements( renderDevice->handler, dstImage, &memRequirements );
    memAllocInfo.allocationSize = memRequirements.size;

    // Memory must be host visible to copy from
    memAllocInfo.memoryTypeIndex = utils::findMemoryType(
        physicalDevice->handler,
        memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );

    CRIMILD_VULKAN_CHECK( vkAllocateMemory( renderDevice->handler, &memAllocInfo, nullptr, &dstImageMemory ) );
    CRIMILD_VULKAN_CHECK( vkBindImageMemory( renderDevice->handler, dstImage, dstImageMemory, 0 ) );

    CRIMILD_LOG_DEBUG( "Creating command buffers..." );

    // Do the actual blit from the swapchain image to our host visible destination image

#if 0
    auto copyCmd = [ & ] {
        auto commandPool = renderDevice->getCommandPool();

        auto commandBufferAllocateInfo = VkCommandBufferAllocateInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = commandPool->handler,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };

        VkCommandBuffer cmdBuffer;
        CRIMILD_VULKAN_CHECK( vkAllocateCommandBuffers( renderDevice->handler, &commandBufferAllocateInfo, &cmdBuffer ) );

        // If requested, also start recording for the new command buffer
        VkCommandBufferBeginInfo cmdBufferBeginInfo {};
        cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        CRIMILD_VULKAN_CHECK( vkBeginCommandBuffer( cmdBuffer, &cmdBufferBeginInfo ) );

        return cmdBuffer;
    }();

#else
    auto copyCmd = utils::beginSingleTimeCommands( renderDevice );
#endif

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

    /**
		* Finish command buffer recording and submit it to a queue
		*
		* @param commandBuffer Command buffer to flush
		* @param queue Queue to submit the command buffer to 
		* @param free (Optional) Free the command buffer once it has been submitted (Defaults to true)
		*
		* @note The queue that the command buffer is submitted to must be from the same family index as the pool it was allocated from
		* @note Uses a fence to ensure command buffer has finished executing
		*/
    auto flushCommandBuffer = [ & ]( VkCommandBuffer commandBuffer, VkQueue queue, bool free = true ) {
        if ( commandBuffer == VK_NULL_HANDLE ) {
            return;
        }

        CRIMILD_VULKAN_CHECK( vkEndCommandBuffer( commandBuffer ) );

        VkSubmitInfo submitInfo {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        // Create fence to ensure that the command buffer has finished executing
        VkFenceCreateInfo fenceInfo {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = 0;
        VkFence fence;
        CRIMILD_VULKAN_CHECK( vkCreateFence( renderDevice->handler, &fenceInfo, nullptr, &fence ) );

        // Submit to the queue
        CRIMILD_VULKAN_CHECK( vkQueueSubmit( queue, 1, &submitInfo, fence ) );
        // Wait for the fence to signal that command buffer has finished executing

// Default fence timeout in nanoseconds
#define DEFAULT_FENCE_TIMEOUT 100000000000

        CRIMILD_VULKAN_CHECK( vkWaitForFences( renderDevice->handler, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT ) );

        vkDestroyFence( renderDevice->handler, fence, nullptr );

        if ( free ) {
            vkFreeCommandBuffers( renderDevice->handler, renderDevice->getCommandPool()->handler, 1, &commandBuffer );
        }
    };

    CRIMILD_LOG_DEBUG( "Executing commands..." );

    // flushCommandBuffer( copyCmd, renderDevice->graphicsQueue );

    // vulkanDevice->flushCommandBuffer( copyCmd, queue );

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

#if 1

    CRIMILD_LOG_DEBUG( "Saving image file..." );

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

#endif

    CRIMILD_LOG_DEBUG( "Screenshot saved..." );

    CRIMILD_LOG_DEBUG( "Cleaning up..." );

    // Clean up resources
    vkUnmapMemory( renderDevice->handler, dstImageMemory );
    vkFreeMemory( renderDevice->handler, dstImageMemory, nullptr );
    vkDestroyImage( renderDevice->handler, dstImage, nullptr );

    CRIMILD_LOG_DEBUG( "Done taking screenshot", filename );

    capturing = false;
}
