crimild_trace()

find_package( Vulkan REQUIRED )

add_library( crimild_vulkan OBJECT )

target_sources( 
    crimild_vulkan

    PUBLIC
    Crimild_Vulkan.hpp

    PRIVATE
    Debug/VulkanDebugMessenger.hpp
    Exceptions/VulkanException.hpp
    Foundation/VulkanInitializers.hpp
    Foundation/VulkanObject.hpp
    Foundation/VulkanUtils.hpp
    Rendering/RenderPasses/VulkanBlitPass.hpp
    Rendering/RenderPasses/VulkanClearPass.hpp
    Rendering/RenderPasses/VulkanDebugAttachmentPass.hpp
    Rendering/RenderPasses/VulkanGBufferPass.hpp
    Rendering/RenderPasses/VulkanGenerateDirectionalLightsShadowMapsPass.hpp
    Rendering/RenderPasses/VulkanGeneratePointLightsShadowMapsPass.hpp
    Rendering/RenderPasses/VulkanGenerateSpotLightsShadowMapsPass.hpp
    Rendering/RenderPasses/VulkanLocalLightingPass.hpp
    Rendering/RenderPasses/VulkanOverlayPass.hpp
    Rendering/RenderPasses/VulkanPresentPass.hpp
    Rendering/RenderPasses/VulkanRenderPassBase.hpp
    Rendering/RenderPasses/VulkanSceneDebugPass.hpp
    Rendering/RenderPasses/VulkanScenePass.hpp
    Rendering/RenderPasses/VulkanSelectionOutlinePass.hpp
    Rendering/RenderPasses/VulkanShaderPass.hpp
    Rendering/RenderPasses/VulkanShadowPass.hpp
    Rendering/RenderPasses/VulkanSoftRTPass.hpp
    Rendering/RenderPasses/VulkanUnlitPass.hpp
    Rendering/VulkanCommandBuffer.hpp
    Rendering/VulkanCommandPool.hpp
    Rendering/VulkanComputePipeline.hpp
    Rendering/VulkanDescriptorPool.hpp
    Rendering/VulkanDescriptorSet.hpp
    Rendering/VulkanDescriptorSetLayout.hpp
    Rendering/VulkanFence.hpp
    Rendering/VulkanFramebuffer.hpp
    Rendering/VulkanFramebufferAttachment.hpp
    Rendering/VulkanGraphicsPipeline.hpp
    Rendering/VulkanGraphicsPipelineOLD.hpp
    Rendering/VulkanImage.hpp
    Rendering/VulkanImageOLD.hpp
    Rendering/VulkanImageView.hpp
    Rendering/VulkanImageViewOLD.hpp
    Rendering/VulkanIndexBuffer.hpp
    Rendering/VulkanInstance.hpp
    Rendering/VulkanPhysicalDevice.hpp
    Rendering/VulkanPhysicalDeviceOLD.hpp
    Rendering/VulkanPipelineLayout.hpp
    Rendering/VulkanRenderDevice.hpp
    Rendering/VulkanRenderDeviceOLD.hpp
    Rendering/VulkanRenderPass.hpp
    Rendering/VulkanRenderPassOLD.hpp
    Rendering/VulkanRenderResource.hpp
    Rendering/VulkanSampler.hpp
    Rendering/VulkanSceneRenderState.hpp
    Rendering/VulkanSemaphore.hpp
    Rendering/VulkanShaderCompiler.hpp
    Rendering/VulkanShaderModule.hpp
    Rendering/VulkanShadowMap.hpp
    Rendering/VulkanStorageBuffer.hpp
    Rendering/VulkanSurface.hpp
    Rendering/VulkanSwapchainOLD.hpp
    Rendering/VulkanTexture.hpp
    Rendering/VulkanUniformBuffer.hpp
    Rendering/VulkanVertexBuffer.hpp
    Simulation/Systems/VulkanCaptureSystem.hpp
    Simulation/Systems/VulkanSystem.hpp
    Visitors/VulkanFetchSceneRenderState.hpp

    PRIVATE
    Crimild_Vulkan.cpp
    Debug/VulkanDebugMessenger.cpp
    Foundation/VulkanUtils.cpp
    Rendering/RenderPasses/VulkanBlitPass.cpp
    Rendering/RenderPasses/VulkanClearPass.cpp
    Rendering/RenderPasses/VulkanDebugAttachmentPass.cpp
    Rendering/RenderPasses/VulkanGBufferPass.cpp
    Rendering/RenderPasses/VulkanGenerateDirectionalLightsShadowMapsPass.cpp
    Rendering/RenderPasses/VulkanGeneratePointLightsShadowMapsPass.cpp
    Rendering/RenderPasses/VulkanGenerateSpotLightsShadowMapsPass.cpp 
    Rendering/RenderPasses/VulkanLocalLightingPass.cpp
    Rendering/RenderPasses/VulkanOverlayPass.cpp
    Rendering/RenderPasses/VulkanPresentPass.cpp
    Rendering/RenderPasses/VulkanRenderPassBase.cpp
    Rendering/RenderPasses/VulkanSceneDebugPass.cpp
    Rendering/RenderPasses/VulkanScenePass.cpp
    Rendering/RenderPasses/VulkanSelectionOutlinePass.cpp
    Rendering/RenderPasses/VulkanShaderPass.cpp
    Rendering/RenderPasses/VulkanShadowPass.cpp
    Rendering/RenderPasses/VulkanSoftRTPass.cpp
    Rendering/RenderPasses/VulkanUnlitPass.cpp
    Rendering/VulkanCommandBuffer.cpp
    Rendering/VulkanCommandPool.cpp
    Rendering/VulkanComputePipeline.cpp
    Rendering/VulkanDescriptorPool.cpp
    Rendering/VulkanDescriptorSet.cpp
    Rendering/VulkanDescriptorSetLayout.cpp
    Rendering/VulkanFence.cpp
    Rendering/VulkanFramebuffer.cpp
    Rendering/VulkanGraphicsPipeline.cpp
    Rendering/VulkanGraphicsPipelineOLD.cpp
    Rendering/VulkanImage.cpp
    Rendering/VulkanImageOLD.cpp
    Rendering/VulkanImageView.cpp
    Rendering/VulkanImageViewOLD.cpp
    Rendering/VulkanIndexBuffer.cpp
    Rendering/VulkanInstance.cpp 
    Rendering/VulkanPhysicalDevice.cpp
    Rendering/VulkanPhysicalDeviceOLD.cpp
    Rendering/VulkanPipelineLayout.cpp
    Rendering/VulkanRenderDevice.cpp
    Rendering/VulkanRenderDeviceOLD.cpp
    Rendering/VulkanRenderPass.cpp
    Rendering/VulkanRenderPassOLD.cpp
    Rendering/VulkanSampler.cpp
    Rendering/VulkanSemaphore.cpp
    Rendering/VulkanShaderCompiler.cpp
    Rendering/VulkanShaderModule.cpp
    Rendering/VulkanShadowMap.cpp
    Rendering/VulkanStorageBuffer.cpp
    Rendering/VulkanSurface.cpp
    Rendering/VulkanSwapchainOLD.cpp
    Rendering/VulkanTexture.cpp
    Rendering/VulkanUniformBuffer.cpp
    Rendering/VulkanVertexBuffer.cpp
    Simulation/Systems/VulkanCaptureSystem.cpp
    Simulation/Systems/VulkanSystem.cpp
    Visitors/VulkanFetchSceneRenderState.cpp
)

target_include_directories( 
	crimild_vulkan
	PUBLIC . 
    # Make sure to include GLSLang headers before Vulkan's so there is no conflict and
    # the app will run correctly. Otherwise, you may seem errors when compiling shaders
    # due to invalid resource settings (like when using gl_VertexIdentifier in arrays)
    PRIVATE ${PROJECT_SOURCE_DIR}/third-party/glslang
	PRIVATE ${Vulkan_INCLUDE_DIRS} 
)

target_link_libraries( 
	crimild_vulkan
	PRIVATE Crimild::Core
	PRIVATE ${Vulkan_LIBRARIES}
    PRIVATE glslang
    PRIVATE SPIRV 
)

add_library( Crimild::Vulkan ALIAS crimild_vulkan )

# Keep IDEs folders clean
set_target_properties( glslang PROPERTIES FOLDER extern )
set_target_properties( SPIRV PROPERTIES FOLDER extern )
