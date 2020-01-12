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

#include "Foundation/Containers/Array.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Simulation/FileSystem.hpp"

using namespace crimild;
using namespace crimild::vulkan;

#define CRIMILD_VULKAN_MAX_FRAMES_IN_FLIGHT 2

struct Vertex {
    Vector2f pos;
    RGBColorf color;

    static VkVertexInputBindingDescription getBindingDescription( void )
    {
        auto bindingDescription = VkVertexInputBindingDescription {
            .binding = 0,
            .stride = sizeof( Vertex ),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        };
        return bindingDescription;
    }

    static std::vector< VkVertexInputAttributeDescription > getAttributeDescriptions( void )
    {
        return {
            VkVertexInputAttributeDescription {
                .binding = 0,
                .location = 0,
                .format = VK_FORMAT_R32G32_SFLOAT,
                .offset = offsetof( Vertex, pos ),
            },
            VkVertexInputAttributeDescription {
                .binding = 0,
                .location = 1,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = offsetof( Vertex, color )
            }
        };
    }
};

struct UniformBufferObject {
    Matrix4f model;
    Matrix4f view;
    Matrix4f proj;
};

namespace crimild {

    namespace vulkan {

        template< typename VertexType >
        class VertexBuffer : public VulkanObject {
        public:
            static VkVertexInputBindingDescription getBindingDescription( void )
            {
                return VkVertexInputBindingDescription {
                    .binding = 0,
                    .stride = sizeof( VertexType ),
                    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                };
            }
        };

    }

}

crimild::Bool VulkanSystem::start( void )
{
    return System::start()
        && createInstance()
        && createDebugMessenger()
        && createSurface()
        && createPhysicalDevice()
        && createRenderDevice()
        && createCommandPool()
//        && createVertexBuffer()
//        && createIndexBuffer()
//        && createDescriptorSetLayout()
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

//    updateUniformBuffer( imageIndex );

    auto commandBuffer = crimild::get_ptr( m_commandBuffers[ 0 ] );//[ imageIndex ];

    // Submit graphic commands to the render device, with the selected
    // image as attachment in the framebuffer
    renderDevice->submitGraphicsCommands(
        wait,
        commandBuffer,
        imageIndex,
        signal,
        fence
    );

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

//    m_vertexBuffer = nullptr;
//    m_indexBuffer = nullptr;
//    m_descriptorSetLayout = nullptr;

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

    return m_swapchain != nullptr;
}

void VulkanSystem::cleanSwapchain( void ) noexcept
{
    if ( m_renderDevice != nullptr ) {
        CRIMILD_LOG_TRACE( "Waiting for pending operations" );
        m_renderDevice->waitIdle();
    }

//    m_descriptorSets.clear();
//    m_descriptorPool = nullptr;
//    m_uniformBuffers.clear();
    m_inFlightFences.clear();
    m_imageAvailableSemaphores.clear();
    m_renderFinishedSemaphores.clear();
    m_commandBuffers.clear();
    m_framebuffers.clear();
//    m_pipeline = nullptr;
    m_renderPass = nullptr;
    m_swapchain = nullptr;
}

crimild::Bool VulkanSystem::recreateSwapchain( void ) noexcept
{
    return createSwapchain()
    	&& createRenderPass()
//    	&& createPipeline()
//    	&& createUniformBuffers()
//    	&& createDescriptorPool()
//    	&& createDescriptorSets()
        && createFramebuffers()
//        && createCommandBuffers()
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

/*
crimild::Bool VulkanSystem::createPipeline( void ) noexcept
{
    auto renderDevice = crimild::get_ptr( m_renderDevice );
    auto swapchain = crimild::get_ptr( m_swapchain );

    m_pipeline = renderDevice->create(
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
            .primitiveType = Primitive::Type::TRIANGLES,
            .viewport = Rectf( 0, 0, swapchain->extent.width, swapchain->extent.height ),
            .scissor = Rectf( 0, 0, swapchain->extent.width, swapchain->extent.height ),
            .bindingDescription = { Vertex::getBindingDescription() },
            .attributeDescriptions = Vertex::getAttributeDescriptions(),
            .setLayouts = {
                crimild::get_ptr( m_descriptorSetLayout ),
            },
        }
    );

    return m_pipeline != nullptr;
}
     */

crimild::Bool VulkanSystem::createFramebuffers( void ) noexcept
{
    auto renderDevice = crimild::get_ptr( m_renderDevice );
    auto swapchain = crimild::get_ptr( m_swapchain );
    auto imageCount = swapchain->imageViews.size();

    for ( auto i = 0l; i < imageCount; i++ ) {
        auto imageView = swapchain->imageViews[ i ];

        auto framebuffer = renderDevice->create(
            Framebuffer::Descriptor {
                .attachments = {
                    crimild::get_ptr( imageView )
                },
                .renderPass = crimild::get_ptr( m_renderPass ),
                .extent = swapchain->extent,
            }
        );
        m_framebuffers.push_back( framebuffer );
    }

    return m_framebuffers.size() > 0;
}

crimild::Bool VulkanSystem::createCommandBuffers( void ) noexcept
{
    /*
    auto renderDevice = crimild::get_ptr( m_renderDevice );
    auto swapchain = crimild::get_ptr( m_swapchain );
    auto imageCount = swapchain->imageViews.size();

    for ( auto i = 0l; i < imageCount; i++ ) {
        auto framebuffer = m_framebuffers[ i ];

        m_commandBuffers[ i ].push_back(
            [ this, framebuffer, renderDevice, i ]() {
                auto commandBuffer = renderDevice->create(
                    CommandBuffer::Descriptor {
                        .commandPool = crimild::get_ptr( m_commandPool ),
                    }
                );
                commandBuffer->begin( CommandBuffer::Usage::SIMULTANEOUS_USE );
                commandBuffer->beginRenderPass(
                    crimild::get_ptr( m_renderPass ),
                    crimild::get_ptr( framebuffer ),
                    RGBAColorf( 0.0f, 0.0f, 0.0f, 1.0f )
                );
                commandBuffer->bindGraphicsPipeline(
                    crimild::get_ptr( m_pipeline )
                );
                commandBuffer->bindVertexBuffer(
                    crimild::get_ptr( m_vertexBuffer )
                );
                commandBuffer->bindIndexBuffer(
                       crimild::get_ptr( m_indexBuffer )
                   );
                commandBuffer->bindDescriptorSets(
                    crimild::get_ptr( m_descriptorSets[ i ] ),
                    crimild::get_ptr( m_pipeline->layout )
                  );
                commandBuffer->drawIndexed( static_cast< crimild::UInt32 >( m_indexBuffer->size / sizeof( crimild::UInt32 ) ) );
                commandBuffer->endRenderPass();
                commandBuffer->end();
                return commandBuffer;
            }()
        );
    }

     */
    return m_commandBuffers.size() > 0;
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

/*
crimild::Bool VulkanSystem::createVertexBuffer( void ) noexcept
{
    const auto vertices = std::vector< Vertex > {
        { Vector2f( -0.5f, 0.5f ), RGBColorf( 1.0f, 0.0f, 0.0f ) },
        { Vector2f( -0.5f, -0.5f ), RGBColorf( 1.0f, 1.0f, 1.0f ) },
        { Vector2f( 0.5f, -0.5f ), RGBColorf( 0.0f, 0.0f, 1.0f ) },
        { Vector2f( 0.5f, 0.5f ), RGBColorf( 0.0f, 1.0f, 0.0f ) },
    };

    auto renderDevice = crimild::get_ptr( m_renderDevice );
    auto commandPool = crimild::get_ptr( m_commandPool );

    m_vertexBuffer = renderDevice->create(
		Buffer::Descriptor {
			.size = sizeof( vertices[ 0 ] ) * vertices.size(),
        	.data = vertices.data(),
        	.usage = Buffer::Usage::VERTEX_BUFFER,
        	.commandPool = commandPool,
    	}
    );
    return m_vertexBuffer != nullptr;
}
 */

/*
crimild::Bool VulkanSystem::createIndexBuffer( void ) noexcept
{
    const auto indices = std::vector< crimild::UInt32 > {
		0, 1, 2,
        0, 2, 3,
    };

    auto renderDevice = crimild::get_ptr( m_renderDevice );
    auto commandPool = crimild::get_ptr( m_commandPool );

    m_indexBuffer = renderDevice->create(
        Buffer::Descriptor {
        	.size = sizeof( indices[ 0 ] ) * indices.size(),
        	.data = indices.data(),
        	.usage = Buffer::Usage::INDEX_BUFFER,
        	.commandPool = commandPool,
        }
    );
    return m_indexBuffer != nullptr;
}
 */

/*
crimild::Bool VulkanSystem::createDescriptorSetLayout( void ) noexcept
{
    m_descriptorSetLayout = m_renderDevice->create(
        DescriptorSetLayout::Descriptor {

        }
   	);
    return m_descriptorSetLayout != nullptr;
}
 */

/*
crimild::Bool VulkanSystem::createUniformBuffers( void ) noexcept
{
    auto renderDevice = crimild::get_ptr( m_renderDevice );
    auto commandPool = crimild::get_ptr( m_commandPool );
    auto swapchain = crimild::get_ptr( m_swapchain );

    auto bufferSize = sizeof( UniformBufferObject );
    m_uniformBuffers.resize( swapchain->images.size() );

    for ( crimild::Size i = 0; i < swapchain->images.size(); ++i ) {
		auto ubo = renderDevice->create(
            Buffer::Descriptor {
				.size = bufferSize,
            	.usage = Buffer::Usage::UNIFORM_BUFFER,
            	.commandPool = commandPool,
            	.data = nullptr,
            }
        );
        m_uniformBuffers[ i ] = ubo;
    }
    return m_uniformBuffers.size() > 0;
}
 */

void VulkanSystem::updateUniformBuffer( crimild::UInt32 currentImage ) noexcept
{
    static const bool ENABLE_ROTATION = true;
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    auto time = ENABLE_ROTATION * std::chrono::duration< float, std::chrono::seconds::period >( currentTime - startTime ).count();

    auto ubo = UniformBufferObject {
        .model = []( crimild::Real32 time ) {
            Transformation t;
            t.rotate().fromAxisAngle( Vector3f::UNIT_Z, time * -90.0f * Numericf::DEG_TO_RAD );
            return t.computeModelMatrix();
        }( time ),
        .view = [] {
            Transformation t;
            t.setTranslate( 4.0f, 4.0f, 4.0f );
            t.lookAt( Vector3f::ZERO, Vector3f::UNIT_Y );
            return t.computeModelMatrix().getInverse();
        }(),
        .proj = []( float width, float height ) {
            auto frustum = Frustumf( 45.0f, width / height, 0.1f, 100.0f );
            auto proj = frustum.computeProjectionMatrix();

            // Invert Y-axis
            // This also needs to set front face as counter-clockwise for culling
            // when creating pipeline. In addition, we need to use a depth range
            // of [0, 1] for Vulkan
            // Check: https://matthewwellings.com/blog/the-new-vulkan-coordinate-system/
            static const auto CLIP_CORRECTION = Matrix4f(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, -1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.5f, 0.5f,
                0.0f, 0.0f, 0.0f, 1.0f
            ).getTranspose(); // TODO: why do I need to transpose this matrix?

            return CLIP_CORRECTION * proj;
        }( m_swapchain->extent.width, m_swapchain->extent.height ),
    };

//    m_uniformBuffers[ currentImage ]->update( &ubo );
}

/*
crimild::Bool VulkanSystem::createDescriptorPool( void ) noexcept
{
    auto renderDevice = crimild::get_ptr( m_renderDevice );
    auto swapchain = crimild::get_ptr( m_swapchain );

    m_descriptorPool = renderDevice->create(
        DescriptorPool::Descriptor {
            .swapchain = swapchain
        }
    );
    return m_descriptorPool != nullptr;
}
 */

crimild::Bool VulkanSystem::createDescriptorSets( void ) noexcept
{
    /*
    auto renderDevice = crimild::get_ptr( m_renderDevice );
    auto swapchain = crimild::get_ptr( m_swapchain );
    auto descriptorPool = crimild::get_ptr( m_descriptorPool );
    auto layout = crimild::get_ptr( m_descriptorSetLayout );

    m_descriptorSets.resize( swapchain->images.size() );
    for ( crimild::Size i = 0; i < m_descriptorSets.size(); ++i ) {
        auto descriptorSet = renderDevice->create(
            DescriptorSet::Descriptor {
            	.descriptorPool = descriptorPool,
            	.layout = layout
        	}
      	);
        descriptorSet->write(
			crimild::get_ptr( m_uniformBuffers[ i ] ),
         	0,
         	sizeof( UniformBufferObject )
     	);
        m_descriptorSets[ i ] = descriptorSet;
    }

    return m_descriptorSets.size() > 0;
     */
}

