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
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "GLFWVulkanSystem.hpp"
#include "WindowSystem.hpp"
#include "Simulation/Simulation.hpp"
#include "Rendering/VulkanInstance.hpp"
#include "Rendering/VulkanSurface.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanSwapchain.hpp"

#include "Rendering/VulkanRenderPass.hpp"
#include "Rendering/VulkanPipeline.hpp"
#include "Rendering/VulkanFramebuffer.hpp"
#include "Rendering/VulkanCommandPool.hpp"
#include "Foundation/Containers/Array.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Simulation/FileSystem.hpp"

using namespace crimild;
using namespace crimild::glfw;
using namespace crimild::vulkan;

crimild::Bool GLFWVulkanSystem::start( void )
{
	auto ret = System::start()
		&& createInstance()
		&& createSurface()
		&& createRenderDevice()
		&& createSwapchain();

	auto renderDevice = m_instance->getRenderDevice();
	auto swapchain = renderDevice->getSwapchain();

	auto renderPass = crimild::alloc< RenderPass >( m_instance->getRenderDevice(), m_instance->getRenderDevice()->getSwapchain() );

	auto pipeline = crimild::alloc< vulkan::Pipeline >(
		m_instance->getRenderDevice(),
		crimild::get_ptr( renderPass ),
		PipelineDescriptor {
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
		}
	);

	std::vector< SharedPointer< vulkan::Framebuffer >> framebuffers;

	swapchain->getImageViews().each(
		[ &framebuffers, swapchain, renderDevice, renderPass ]( SharedPointer< ImageView > const &iv ) {
			
			auto framebuffer = crimild::alloc< vulkan::Framebuffer >(
				renderDevice,
				vulkan::FramebufferDescriptor {
					.attachments = {
						crimild::get_ptr( iv )
					},
					.renderPass = crimild::get_ptr( renderPass ),
				  	.extent = swapchain->getExtent(),
		   		}
			);
			framebuffers.push_back( framebuffer );
		}
	);

	auto commandPool = renderDevice->createGraphicsCommandPool();
	auto commandBuffer = commandPool->createCommandBuffer();

	commandBuffer = nullptr;
	commandPool = nullptr;

	framebuffers.clear();

	pipeline = nullptr;
	renderPass = nullptr;

	return ret;
}

void GLFWVulkanSystem::update( void )
{
	/*
	auto swapchain = m_instance->getSwapchain();
	if ( swapchain == nullptr ) {
		return;
	}

	// Acquire the next image available image from the swapchain
	auto imageIndex = swapchain->acquireNextImage();
	if ( imageIndex == std::numeric_limits< crimild::UInt32 >::max() ) {
		// No image available
		return;
	}

	swapchain->presentImage( imageIndex );
	*/
}

void GLFWVulkanSystem::stop( void )
{
	System::stop();

	m_instance = nullptr;
}

crimild::Bool GLFWVulkanSystem::createInstance( void ) noexcept
{
	CRIMILD_LOG_TRACE( "Creating Vulkan instance" );

	m_instance = VulkanInstance::create();
	return m_instance != nullptr;
}

crimild::Bool GLFWVulkanSystem::createSurface( void ) noexcept
{
	CRIMILD_LOG_TRACE( "Creating Vulkan surface" );

	auto sim = Simulation::getInstance();
	auto windowSystem = sim->getSystem< WindowSystem >();
	auto window = windowSystem->getWindowHandler();
	
	VkSurfaceKHR surfaceHandler;
	auto result = glfwCreateWindowSurface(
		m_instance->getInstanceHandler(),
		window,
		nullptr,
		&surfaceHandler
	);
	if ( result != VK_SUCCESS ) {
		CRIMILD_LOG_FATAL( "Failed to create window surface for Vulkan. Error: ", result );
		return false;
	}

	auto surface = crimild::alloc< VulkanSurface >( crimild::get_ptr( m_instance ), surfaceHandler );
	m_instance->setSurface( surface );

	return true;
}

crimild::Bool GLFWVulkanSystem::createRenderDevice( void ) noexcept
{
	CRIMILD_LOG_TRACE( "Creating Vulkan render device" );
	
	auto renderDevice = VulkanRenderDevice::create(
		getInstance(),
		getInstance()->getSurface()
	);

	if ( renderDevice == nullptr ) {
		return false;
	}

	getInstance()->setRenderDevice( renderDevice );
	return true;
}

crimild::Bool GLFWVulkanSystem::createSwapchain( void ) noexcept
{
	auto swapchain = Swapchain::create( getInstance()->getRenderDevice(), getInstance()->getSurface() );
	if ( swapchain == nullptr ) {
		return false;
	}

	getInstance()->getRenderDevice()->setSwapchain( swapchain );
	return true;
}
