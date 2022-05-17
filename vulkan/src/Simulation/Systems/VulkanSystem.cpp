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
#include "Rendering/VulkanSwapchainOLD.hpp"
#include "SceneGraph/Camera.hpp"
#include "Simulation/AssetManager.hpp"
#include "Simulation/FileSystem.hpp"
#include "Simulation/Simulation.hpp"
#include "Simulation/Systems/RenderSystem.hpp"

using namespace crimild;
using namespace crimild::vulkan;

void VulkanSystem::start( void ) noexcept
{
    // registerMessageHandler< messaging::FrameGraphDidChange >( [ & ]( auto & ) {
    //     m_recordWithNonConditionalPasses = true;
    // } );

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
        VulkanInstanceOLD::Descriptor {
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
    CRIMILD_LOG_TRACE();
    m_surface = create(
        VulkanSurfaceOLD::Descriptor {
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
        PhysicalDeviceOLD::Descriptor {
            .instance = crimild::get_ptr( m_instance ),
            .surface = crimild::get_ptr( m_surface ) } );
    return m_physicalDevice != nullptr;
}

crimild::Bool VulkanSystem::createRenderDevice( void ) noexcept
{
    m_renderDevice = m_physicalDevice->create( RenderDeviceOLD::Descriptor {} );
    return m_renderDevice != nullptr;
}

crimild::Bool VulkanSystem::createSwapchain( void ) noexcept
{
    auto settings = Simulation::getInstance()->getSettings();
    auto width = settings->get< crimild::UInt >( "video.width", 0 );
    auto height = settings->get< crimild::UInt >( "video.height", 0 );

    m_swapchain = m_renderDevice->create(
        SwapchainOLD::Descriptor {
            .extent = Vector2ui { width, height } } );

    if ( m_swapchain == nullptr ) {
        return false;
    }

    settings->set( "video.width", m_swapchain->extent.width );
    settings->set( "video.height", m_swapchain->extent.height );

    if ( auto mainCamera = Camera::getMainCamera() ) {
        // mainCamera->setAspectRatio( ( crimild::Real32 ) m_swapchain->extent.width / ( crimild::Real32 ) m_swapchain->extent.height );
    }

    // Reset existing command buffers
    m_recordWithNonConditionalPasses = true;

    return true;
}

void VulkanSystem::cleanSwapchain( void ) noexcept
{
    if ( auto renderDevice = crimild::get_ptr( m_renderDevice ) ) {
        CRIMILD_LOG_TRACE();
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
