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
#include "Simulation/Simulation.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanSwapchain.hpp"
#include "Rendering/VulkanCommandPool.hpp"
#include "Rendering/VulkanCommandBuffer.hpp"
#include "Rendering/VulkanFence.hpp"
#include "Rendering/VulkanBuffer.hpp"
#include "SceneGraph/Camera.hpp"

#include "Foundation/Containers/Array.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Simulation/FileSystem.hpp"

using namespace crimild;
using namespace crimild::vulkan;

#define CRIMILD_VULKAN_MAX_FRAMES_IN_FLIGHT 2

crimild::Bool VulkanSystem::start( void )
{
    return System::start()
        && createInstance()
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
void VulkanSystem::update( void )
{
    auto renderDevice = crimild::get_ptr( m_renderDevice );
    if ( renderDevice == nullptr ) {
        CRIMILD_LOG_ERROR( "No valid render device instance" );
        return;
    }

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
        }
        else {
            CRIMILD_LOG_ERROR( "No image available" );
            exit( -1 );
        }
    }

    auto imageIndex = result.imageIndex;

    updateUniformBuffer( imageIndex );

    if ( m_commandBuffers.size() > 0 ) {
        auto commandBuffer = crimild::get_ptr( m_commandBuffers[ 0 ] );

        // Submit graphic commands to the render device, with the selected
        // image as attachment in the framebuffer
        renderDevice->submitGraphicsCommands(
            wait,
            commandBuffer,
            imageIndex,
            signal,
            fence
        );
    }
    else {
        CRIMILD_LOG_DEBUG( "No command buffers provided" );
    }

    // Return the image to the swapchain so it can be presented
    auto presentationResult = swapchain->presentImage( imageIndex, signal );
    if ( !presentationResult.success ) {
        if ( presentationResult.outOfDate ) {
            cleanSwapchain();
            recreateSwapchain();
            return;
        }
        else {
            CRIMILD_LOG_ERROR( "Failed to present image" );
            exit( -1 );
        }
    }

    m_currentFrame = ( m_currentFrame + 1 ) % CRIMILD_VULKAN_MAX_FRAMES_IN_FLIGHT;
}

void VulkanSystem::stop( void )
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
            .appVersionPatch = appVersionPatch
        }
    );

    return m_instance != nullptr;
}

crimild::Bool VulkanSystem::createDebugMessenger( void ) noexcept
{
    m_debugMessenger = create(
        VulkanDebugMessenger::Descriptor {
            .instance = crimild::get_ptr( m_instance )
        }
       );
    return m_debugMessenger != nullptr;
}

crimild::Bool VulkanSystem::createSurface( void ) noexcept
{
    m_surface = create(
		VulkanSurface::Descriptor {
    		.instance = crimild::get_ptr( m_instance )
    	}
	);
    if ( m_surface == nullptr ) {
        return false;
    }

    attach( crimild::get_ptr( m_surface ) );
    return true;
}

crimild::Bool VulkanSystem::createPhysicalDevice( void ) noexcept
{
    m_physicalDevice = create(
        PhysicalDevice::Descriptor {
            .instance = crimild::get_ptr( m_instance ),
            .surface = crimild::get_ptr( m_surface )
        }
    );
    return m_physicalDevice != nullptr;
}

crimild::Bool VulkanSystem::createRenderDevice( void ) noexcept
{
    m_renderDevice = m_physicalDevice->create( RenderDevice::Descriptor { } );
    return m_renderDevice != nullptr;
}

crimild::Bool VulkanSystem::createSwapchain( void ) noexcept
{
    auto settings = Simulation::getInstance()->getSettings();
    auto width = settings->get< crimild::Int32 >( "video.width", 0 );
    auto height = settings->get< crimild::Int32 >( "video.height", 0 );

    m_swapchain = m_renderDevice->create(
        Swapchain::Descriptor {
            .extent = Vector2i( width, height )
        }
    );

    if ( m_swapchain == nullptr ) {
        return false;
    }

    settings->set( "video.width", m_swapchain->extent.width );
    settings->set( "video.height", m_swapchain->extent.height );

    if ( auto mainCamera = Camera::getMainCamera() ) {
        mainCamera->setAspectRatio( ( crimild::Real32 ) m_swapchain->extent.width / ( crimild::Real32 ) m_swapchain->extent.height );
    }

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
        static_cast< BufferManager * >( renderDevice )->clear();
        static_cast< CommandBufferManager * >( renderDevice )->clear();
        static_cast< PipelineManager * >( renderDevice )->clear();

        renderDevice->reset( crimild::get_ptr( m_commandPool ) );
    }

    m_inFlightFences.clear();
    m_imageAvailableSemaphores.clear();
    m_renderFinishedSemaphores.clear();
    m_colorAttachment.imageView = nullptr;
    m_colorAttachment.image = nullptr;
    m_depthAttachment.imageView = nullptr;
    m_depthAttachment.image = nullptr;
    m_framebuffers.clear();
    m_renderPass = nullptr;
    m_swapchain = nullptr;
    m_currentFrame = 0;
}

crimild::Bool VulkanSystem::recreateSwapchain( void ) noexcept
{
    cleanSwapchain();

    return createSwapchain()
    	&& createRenderPass()
        && createColorResources()
    	&& createDepthResources()
        && createFramebuffers()
        && createSyncObjects();
}

crimild::Bool VulkanSystem::createRenderPass( void ) noexcept
{
    auto renderDevice = crimild::get_ptr( m_renderDevice );
    auto swapchain = crimild::get_ptr( m_swapchain );

    m_renderPass = renderDevice->create(
        RenderPass::Descriptor {
            .swapchain = swapchain,
        }
    );

    return m_renderPass != nullptr;
}

crimild::Bool VulkanSystem::createColorResources( void ) noexcept
{
    auto renderDevice = crimild::get_ptr( m_renderDevice );
    auto msaaSamples = renderDevice->physicalDevice->msaaSamples;
    auto swapchain = crimild::get_ptr( m_swapchain );

    auto colorFormat = swapchain->format;

    m_colorAttachment.image = renderDevice->create( Image::Descriptor {
		.width = swapchain->extent.width,
        .height = swapchain->extent.height,
        .mipLevels = 1,
        .numSamples = msaaSamples,
        .format = colorFormat,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    });

    m_colorAttachment.imageView = renderDevice->create( ImageView::Descriptor {
		.image = m_colorAttachment.image,
        .format = colorFormat,
        .aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT,
        .mipLevels = 1,
    });

    return true;
}

crimild::Bool VulkanSystem::createDepthResources( void ) noexcept
{
    auto renderDevice = crimild::get_ptr( m_renderDevice );
    auto msaaSamples = renderDevice->physicalDevice->msaaSamples;
    auto swapchain = crimild::get_ptr( m_swapchain );

    auto depthFormat = utils::findDepthFormat( renderDevice );

    m_depthAttachment.image = renderDevice->create( Image::Descriptor {
        .width = swapchain->extent.width,
        .height = swapchain->extent.height,
        .format = depthFormat,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        .properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        .mipLevels = 1,
        .numSamples = msaaSamples,
    });

    m_depthAttachment.imageView = renderDevice->create(
        ImageView::Descriptor {
            .image = m_depthAttachment.image,
            .format = depthFormat,
            .aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT,
            .mipLevels = 1,
    	}
    );

    // optional
    utils::transitionImageLayout(
        renderDevice,
        m_depthAttachment.image->handler,
        depthFormat,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        1,
        1
    );

    return true;
}

crimild::Bool VulkanSystem::createFramebuffers( void ) noexcept
{
    auto renderDevice = crimild::get_ptr( m_renderDevice );
    auto physicalDevice = renderDevice->physicalDevice;
    auto msaaSamples = physicalDevice->msaaSamples;
    auto swapchain = crimild::get_ptr( m_swapchain );
    auto imageCount = swapchain->imageViews.size();

    for ( auto i = 0l; i < imageCount; i++ ) {
        auto imageView = swapchain->imageViews[ i ];

        std::vector< ImageView * > attachments;
        if ( msaaSamples == VK_SAMPLE_COUNT_1_BIT ) {
            // Warning: Order must match the one defined by the render pass attachments
            attachments = {
                crimild::get_ptr( imageView ),
                crimild::get_ptr( m_depthAttachment.imageView ),
            };
        }
        else {
            // Warning: Order must match the one defined by the render pass attachments
            attachments = {
                crimild::get_ptr( m_colorAttachment.imageView ),
                crimild::get_ptr( m_depthAttachment.imageView ),
                crimild::get_ptr( imageView ),
            };
        }

        auto framebuffer = renderDevice->create(
            Framebuffer::Descriptor {
                .attachments = attachments,
                .renderPass = crimild::get_ptr( m_renderPass ),
                .extent = swapchain->extent,
            }
        );
        m_framebuffers.push_back( framebuffer );
    }

    return m_framebuffers.size() > 0;
}

crimild::Bool VulkanSystem::createSyncObjects( void ) noexcept
{
    auto renderDevice = crimild::get_ptr( m_renderDevice );

    for ( auto i = 0l; i < CRIMILD_VULKAN_MAX_FRAMES_IN_FLIGHT; i++ ) {
        m_imageAvailableSemaphores.push_back( renderDevice->create( Semaphore::Descriptor { } ) );
        m_renderFinishedSemaphores.push_back( renderDevice->create( Semaphore::Descriptor { } ) );
        m_inFlightFences.push_back( renderDevice->create( Fence::Descriptor { } ) );
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
        }
    );

    return m_commandPool != nullptr;
}

void VulkanSystem::updateUniformBuffer( crimild::UInt32 currentImage ) noexcept
{
    getRenderDevice()->updateUniformBuffers( currentImage );
}

