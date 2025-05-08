crimild_trace()

find_package(Vulkan REQUIRED COMPONENTS glslang SPIRV-Tools)

if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Could not find Vulkan libraries")
else()
    message(STATUS ${Vulkan_LIBRARY})
endif()

add_library(crimild_vulkan OBJECT)

target_sources(
    crimild_vulkan

    PUBLIC Crimild_Vulkan.hpp

    PRIVATE Crimild_Vulkan.cpp

    PRIVATE Foundation/VulkanInitializers.hpp
    PRIVATE Foundation/VulkanUtils.cpp
    PRIVATE Foundation/VulkanUtils.hpp

    PRIVATE Rendering/FrameGraph/VulkanComputeBase.hpp
    PRIVATE Rendering/FrameGraph/VulkanComputeImageMix.cpp
    PRIVATE Rendering/FrameGraph/VulkanComputeImageMix.hpp
    PRIVATE Rendering/FrameGraph/VulkanComputeImageSwizzle.cpp
    PRIVATE Rendering/FrameGraph/VulkanComputeImageSwizzle.hpp
    PRIVATE Rendering/FrameGraph/VulkanFrameGraphNode.hpp
    PRIVATE Rendering/FrameGraph/VulkanRenderBase.cpp
    PRIVATE Rendering/FrameGraph/VulkanRenderBase.hpp
    PRIVATE Rendering/FrameGraph/VulkanRenderScene.cpp
    PRIVATE Rendering/FrameGraph/VulkanRenderScene.hpp
    PRIVATE Rendering/FrameGraph/VulkanRenderSceneGBuffer.cpp
    PRIVATE Rendering/FrameGraph/VulkanRenderSceneGBuffer.hpp
    PRIVATE Rendering/FrameGraph/VulkanRenderSceneLighting.cpp
    PRIVATE Rendering/FrameGraph/VulkanRenderSceneLighting.hpp
    PRIVATE Rendering/FrameGraph/VulkanRenderSceneRT.cpp
    PRIVATE Rendering/FrameGraph/VulkanRenderSceneRT.hpp
    PRIVATE Rendering/FrameGraph/VulkanRenderSceneUnlit.cpp
    PRIVATE Rendering/FrameGraph/VulkanRenderSceneUnlit.hpp
    PRIVATE Rendering/FrameGraph/VulkanRenderShadowMaps.cpp
    PRIVATE Rendering/FrameGraph/VulkanRenderShadowMaps.hpp

    PRIVATE Rendering/VulkanBuffer.cpp
    PRIVATE Rendering/VulkanBuffer.hpp
    PRIVATE Rendering/VulkanCommandBuffer.cpp
    PRIVATE Rendering/VulkanCommandBuffer.hpp
    PRIVATE Rendering/VulkanComputePipeline.cpp
    PRIVATE Rendering/VulkanComputePipeline.hpp
    PRIVATE Rendering/VulkanDescriptor.hpp
    PRIVATE Rendering/VulkanDescriptorPool.cpp
    PRIVATE Rendering/VulkanDescriptorPool.hpp
    PRIVATE Rendering/VulkanDescriptorSet.cpp
    PRIVATE Rendering/VulkanDescriptorSet.hpp
    PRIVATE Rendering/VulkanDescriptorSetLayout.cpp
    PRIVATE Rendering/VulkanDescriptorSetLayout.hpp
    PRIVATE Rendering/VulkanFence.cpp
    PRIVATE Rendering/VulkanFence.hpp
    PRIVATE Rendering/VulkanFramebuffer.cpp
    PRIVATE Rendering/VulkanFramebuffer.hpp
    PRIVATE Rendering/VulkanFramebufferAttachment.hpp
    PRIVATE Rendering/VulkanGraphicsPipeline.cpp
    PRIVATE Rendering/VulkanGraphicsPipeline.hpp
    PRIVATE Rendering/VulkanImage.cpp
    PRIVATE Rendering/VulkanImage.hpp
    PRIVATE Rendering/VulkanImageView.cpp
    PRIVATE Rendering/VulkanImageView.hpp
    PRIVATE Rendering/VulkanInstance.cpp
    PRIVATE Rendering/VulkanInstance.hpp
    PRIVATE Rendering/VulkanPhysicalDevice.cpp
    PRIVATE Rendering/VulkanPhysicalDevice.hpp
    PRIVATE Rendering/VulkanRenderDevice.cpp
    PRIVATE Rendering/VulkanRenderDevice.hpp
    PRIVATE Rendering/VulkanRenderDeviceCache.cpp
    PRIVATE Rendering/VulkanRenderDeviceCache.hpp
    PRIVATE Rendering/VulkanRenderPass.cpp
    PRIVATE Rendering/VulkanRenderPass.hpp
    PRIVATE Rendering/VulkanRenderTarget.cpp
    PRIVATE Rendering/VulkanRenderTarget.hpp
    PRIVATE Rendering/VulkanSampler.cpp
    PRIVATE Rendering/VulkanSampler.hpp
    PRIVATE Rendering/VulkanSceneRenderState.hpp
    PRIVATE Rendering/VulkanSemaphore.cpp
    PRIVATE Rendering/VulkanSemaphore.hpp
    PRIVATE Rendering/VulkanShaderCompiler.cpp
    PRIVATE Rendering/VulkanShaderCompiler.hpp
    PRIVATE Rendering/VulkanShaderModule.cpp
    PRIVATE Rendering/VulkanShaderModule.hpp
    PRIVATE Rendering/VulkanShadowMap.cpp
    PRIVATE Rendering/VulkanShadowMap.hpp
    PRIVATE Rendering/VulkanSurface.cpp
    PRIVATE Rendering/VulkanSurface.hpp
    PRIVATE Rendering/VulkanSwapchain.cpp
    PRIVATE Rendering/VulkanSwapchain.hpp
    PRIVATE Rendering/VulkanSynchronization.hpp
    PRIVATE Visitors/VulkanFetchSceneRenderState.cpp
    PRIVATE Visitors/VulkanFetchSceneRenderState.hpp
)

target_include_directories(
    crimild_vulkan
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    crimild_vulkan
    PRIVATE Crimild::Foundation
    PRIVATE Crimild::Mathematics
    PRIVATE Crimild::Coding
    PRIVATE Crimild::Core

    # Vulkan libraries are made public so they can be used in other projects, like
    # the editor or players
    PUBLIC Vulkan::Vulkan
    PUBLIC Vulkan::glslang
    PUBLIC Vulkan::SPIRV-Tools
)

add_library(Crimild::Vulkan ALIAS crimild_vulkan)
