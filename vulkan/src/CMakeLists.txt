crimild_trace()

set(
    CRIMILD_LIBRARY_NAME
    crimild_vulkan
)

set( 
    CRIMILD_LIBRARY_SOURCE_FILES 
    Crimild_Vulkan.cpp
    Crimild_Vulkan.hpp
    Debug/VulkanDebugMessenger.cpp
    Debug/VulkanDebugMessenger.hpp
    Exceptions/VulkanException.hpp
    Foundation/VulkanInitializers.hpp
    Foundation/VulkanObject.hpp
    Foundation/VulkanUtils.cpp
    Foundation/VulkanUtils.hpp
    Rendering/RenderPasses/VulkanBlitPass.cpp
    Rendering/RenderPasses/VulkanBlitPass.hpp
    Rendering/RenderPasses/VulkanClearPass.cpp
    Rendering/RenderPasses/VulkanClearPass.hpp
    Rendering/RenderPasses/VulkanDebugAttachmentPass.cpp
    Rendering/RenderPasses/VulkanDebugAttachmentPass.hpp
    Rendering/RenderPasses/VulkanGBufferPass.cpp
    Rendering/RenderPasses/VulkanGBufferPass.hpp
    Rendering/RenderPasses/VulkanGenerateDirectionalLightsShadowMapsPass.cpp
    Rendering/RenderPasses/VulkanGenerateDirectionalLightsShadowMapsPass.hpp
    Rendering/RenderPasses/VulkanGeneratePointLightsShadowMapsPass.cpp
    Rendering/RenderPasses/VulkanGeneratePointLightsShadowMapsPass.hpp
    Rendering/RenderPasses/VulkanGenerateSpotLightsShadowMapsPass.cpp 
    Rendering/RenderPasses/VulkanGenerateSpotLightsShadowMapsPass.hpp
    Rendering/RenderPasses/VulkanLocalLightingPass.cpp
    Rendering/RenderPasses/VulkanLocalLightingPass.hpp
    Rendering/RenderPasses/VulkanOverlayPass.cpp
    Rendering/RenderPasses/VulkanOverlayPass.hpp
    Rendering/RenderPasses/VulkanPresentPass.cpp
    Rendering/RenderPasses/VulkanPresentPass.hpp
    Rendering/RenderPasses/VulkanRenderPassBase.cpp
    Rendering/RenderPasses/VulkanRenderPassBase.hpp
    Rendering/RenderPasses/VulkanSceneDebugPass.cpp
    Rendering/RenderPasses/VulkanSceneDebugPass.hpp
    Rendering/RenderPasses/VulkanScenePass.cpp
    Rendering/RenderPasses/VulkanScenePass.hpp
    Rendering/RenderPasses/VulkanSelectionOutlinePass.cpp
    Rendering/RenderPasses/VulkanSelectionOutlinePass.hpp
    Rendering/RenderPasses/VulkanShaderPass.cpp
    Rendering/RenderPasses/VulkanShaderPass.hpp
    Rendering/RenderPasses/VulkanShadowPass.cpp
    Rendering/RenderPasses/VulkanShadowPass.hpp
    Rendering/RenderPasses/VulkanSoftRTPass.cpp
    Rendering/RenderPasses/VulkanSoftRTPass.hpp
    Rendering/RenderPasses/VulkanUnlitPass.cpp
    Rendering/RenderPasses/VulkanUnlitPass.hpp
    Rendering/VulkanCommandBuffer.cpp
    Rendering/VulkanCommandBuffer.hpp
    Rendering/VulkanCommandPool.cpp
    Rendering/VulkanCommandPool.hpp
    Rendering/VulkanComputePipeline.cpp
    Rendering/VulkanComputePipeline.hpp
    Rendering/VulkanDescriptorPool.cpp
    Rendering/VulkanDescriptorPool.hpp
    Rendering/VulkanDescriptorSet.cpp
    Rendering/VulkanDescriptorSet.hpp
    Rendering/VulkanDescriptorSetLayout.cpp
    Rendering/VulkanDescriptorSetLayout.hpp
    Rendering/VulkanFence.cpp
    Rendering/VulkanFence.hpp
    Rendering/VulkanFramebuffer.cpp
    Rendering/VulkanFramebuffer.hpp
    Rendering/VulkanFramebufferAttachment.hpp
    Rendering/VulkanGraphicsPipeline.cpp
    Rendering/VulkanGraphicsPipeline.hpp
    Rendering/VulkanGraphicsPipelineOLD.cpp
    Rendering/VulkanGraphicsPipelineOLD.hpp
    Rendering/VulkanImage.cpp
    Rendering/VulkanImage.hpp
    Rendering/VulkanImageOLD.cpp
    Rendering/VulkanImageOLD.hpp
    Rendering/VulkanImageView.cpp
    Rendering/VulkanImageView.hpp
    Rendering/VulkanImageViewOLD.cpp
    Rendering/VulkanImageViewOLD.hpp
    Rendering/VulkanIndexBuffer.cpp
    Rendering/VulkanIndexBuffer.hpp
    Rendering/VulkanInstance.cpp 
    Rendering/VulkanInstance.hpp
    Rendering/VulkanPhysicalDevice.cpp
    Rendering/VulkanPhysicalDevice.hpp
    Rendering/VulkanPhysicalDeviceOLD.cpp
    Rendering/VulkanPhysicalDeviceOLD.hpp
    Rendering/VulkanPipelineLayout.cpp
    Rendering/VulkanPipelineLayout.hpp
    Rendering/VulkanRenderDevice.cpp
    Rendering/VulkanRenderDevice.hpp
    Rendering/VulkanRenderDeviceOLD.cpp
    Rendering/VulkanRenderDeviceOLD.hpp
    Rendering/VulkanRenderPass.cpp
    Rendering/VulkanRenderPass.hpp
    Rendering/VulkanRenderResource.hpp
    Rendering/VulkanSampler.cpp
    Rendering/VulkanSampler.hpp
    Rendering/VulkanSceneRenderState.hpp
    Rendering/VulkanSemaphore.cpp
    Rendering/VulkanSemaphore.hpp
    Rendering/VulkanShaderCompiler.cpp
    Rendering/VulkanShaderCompiler.hpp
    Rendering/VulkanShaderModule.cpp
    Rendering/VulkanShaderModule.hpp
    Rendering/VulkanShadowMap.cpp
    Rendering/VulkanShadowMap.hpp
    Rendering/VulkanStorageBuffer.cpp
    Rendering/VulkanStorageBuffer.hpp
    Rendering/VulkanSurface.cpp
    Rendering/VulkanSurface.hpp
    Rendering/VulkanSwapchainOLD.cpp
    Rendering/VulkanSwapchainOLD.hpp
    Rendering/VulkanTexture.cpp
    Rendering/VulkanTexture.hpp
    Rendering/VulkanUniformBuffer.cpp
    Rendering/VulkanUniformBuffer.hpp
    Rendering/VulkanVertexBuffer.cpp
    Rendering/VulkanVertexBuffer.hpp
    Simulation/Systems/VulkanCaptureSystem.cpp
    Simulation/Systems/VulkanCaptureSystem.hpp
    Simulation/Systems/VulkanSystem.cpp
    Simulation/Systems/VulkanSystem.hpp
    Visitors/VulkanFetchSceneRenderState.cpp
    Visitors/VulkanFetchSceneRenderState.hpp
)

find_package( Vulkan REQUIRED )

add_library(
    ${CRIMILD_LIBRARY_NAME}
    ${CRIMILD_LIBRARY_SOURCE_FILES}
)

target_include_directories( 
	${CRIMILD_LIBRARY_NAME} 
	PUBLIC . 
     # Make sure to include GLSLang headers before Vulkan's so there is no conflict and
     # the app will run correctly. Otherwise, you may seem errors when compiling shaders
     # due to invalid resource settings (like when using gl_VertexIdentifier in arrays)
     PRIVATE ${Crimild_SOURCE_DIR}/third-party/glslang
	PUBLIC ${Vulkan_INCLUDE_DIRS} 
)

target_link_libraries( 
	${CRIMILD_LIBRARY_NAME} 
	PRIVATE crimild_core 
	PRIVATE ${Vulkan_LIBRARIES}
     PRIVATE glslang
     PRIVATE SPIRV 
)

target_compile_features( ${CRIMILD_LIBRARY_NAME} PUBLIC cxx_std_20 )
