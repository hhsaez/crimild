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

#include "Foundation/Containers/Array.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Simulation/FileSystem.hpp"

using namespace crimild;
using namespace crimild::vulkan;

#define CRIMILD_VULKAN_MAX_FRAMES_IN_FLIGHT 2

crimild::Bool VulkanSystem::start( void )
{
    if ( !System::start()
        || !createInstance()
        || !createDebugMessenger()
        || !createSurface()
        || !createPhysicalDevice()
        || !createRenderDevice()
        || !createSwapchain() ) {
        return false;
    }

    /*
    auto renderDevice = crimild::get_ptr( m_renderDevice );
    auto swapchain = renderDevice->getSwapchain();

    m_renderPass = renderDevice->createRenderPass();

    m_pipeline = renderDevice->createPipeline(
        Pipeline::Descriptor {
            .program = crimild::alloc< ShaderProgram >(
                containers::Array< SharedPointer< Shader >> {
                    crimild::alloc< Shader >(
                        Shader::Stage::VERTEX,
                        FileSystem::getInstance().readResourceFile( "assets/shaders/triangle.vert.spv" )
                    ),
                    crimild::alloc< Shader >(
                        Shader::Stage::FRAGMENT,
                        FileSystem::getInstance().readResourceFile( "assets/shaders/triangle.frag.spv" )
                    ),
                }
            ),
            .renderPass = crimild::get_ptr( m_renderPass ),
        }
    );

    m_commandPool = renderDevice->createGraphicsCommandPool();

    auto imageCount = swapchain->getImageViews().size();

    for ( auto i = 0l; i < imageCount; i++ ) {
        auto imageView = swapchain->getImageViews()[ i ];

        auto framebuffer = renderDevice->createFramebuffer(
            Framebuffer::Descriptor {
                .attachments = {
                    crimild::get_ptr( imageView )
                },
                .renderPass = crimild::get_ptr( m_renderPass ),
                .extent = swapchain->getExtent(),
            }
        );
        m_framebuffers.push_back( framebuffer );

        m_commandBuffers.push_back(
            [ this, framebuffer ]() {
                auto commandBuffer = m_commandPool->createCommandBuffer();
                commandBuffer->begin( CommandBuffer::Usage::SIMULTANEOUS_USE );
                commandBuffer->beginRenderPass(
                    crimild::get_ptr( m_renderPass ),
                    crimild::get_ptr( framebuffer ),
                    RGBAColorf( 0.0f, 0.0f, 0.0f, 1.0f )
                );
                commandBuffer->bindGraphicsPipeline(
                    crimild::get_ptr( m_pipeline )
                );
                commandBuffer->draw();
                commandBuffer->endRenderPass();
                commandBuffer->end();
                return commandBuffer;
            }()
        );
    }

    for ( auto i = 0l; i < CRIMILD_VULKAN_MAX_FRAMES_IN_FLIGHT; i++ ) {
        m_imageAvailableSemaphores.push_back( renderDevice->createSemaphore() );
        m_renderFinishedSemaphores.push_back( renderDevice->createSemaphore() );
        m_inFlightFences.push_back( renderDevice->createFence() );
    }
     */

    return true;
}

/*
 1. Acquire an image from the swapchain
 2. Execute command buffer with that image as attachment in the framebuffer
 3. Return the image to the swapchain for presentation
*/
void VulkanSystem::update( void )
{
    /*
    auto renderDevice = crimild::get_ptr( m_renderDevice );
    auto swapchain = renderDevice->getSwapchain();

    auto wait = crimild::get_ptr( m_imageAvailableSemaphores[ m_currentFrame ] );
    auto signal = crimild::get_ptr( m_renderFinishedSemaphores[ m_currentFrame ] );
    auto fence = crimild::get_ptr( m_inFlightFences[ m_currentFrame ] );

    // Wait for any pending operations to complete
    fence->wait();
    fence->reset();

    // Acquire the next image available image from the swapchain
    // Execution of command buffers is asynchrounous. It must be set up to
    // wait on image adquisition to finish
    auto imageIndex = swapchain->acquireNextImage( wait );
    if ( imageIndex == std::numeric_limits< crimild::UInt32 >::max() ) {
        CRIMILD_LOG_WARNING( "No image available" );
        return;
    }

    // Submit graphic commands to the render device, with the selected
    // image as attachment in the framebuffer
    renderDevice->submitGraphicsCommands(
        wait,
        crimild::get_ptr( m_commandBuffers[ imageIndex ] ),
        signal,
        fence
    );

    // Return the image to the swapchain so it can be presented
    swapchain->presentImage( imageIndex, signal );

    m_currentFrame = ( m_currentFrame + 1 ) % CRIMILD_VULKAN_MAX_FRAMES_IN_FLIGHT;
     */
}

void VulkanSystem::stop( void )
{
    System::stop();

    if ( m_renderDevice != nullptr ) {
        CRIMILD_LOG_TRACE( "Waiting for pending operations" );
//        m_renderDevice->waitIdle();
    }

    m_inFlightFences.clear();
    m_imageAvailableSemaphores.clear();
    m_renderFinishedSemaphores.clear();
    m_commandBuffers.clear();
    m_commandPool = nullptr;
    m_framebuffers.clear();
    m_pipeline = nullptr;
    m_renderPass = nullptr;

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
    m_renderDevice = create(
        RenderDevice::Descriptor {
        	.physicalDevice = crimild::get_ptr( m_physicalDevice ),
    	}
    );
    return m_renderDevice != nullptr;
}

crimild::Bool VulkanSystem::createSwapchain( void ) noexcept
{
    /*
    auto swapchain = Swapchain::create( crimild::get_ptr( m_renderDevice ), crimild::get_ptr( m_surface ) );
    if ( swapchain == nullptr ) {
        return false;
    }

    m_renderDevice->setSwapchain( swapchain );
    return true;
     */
    return false;
}
