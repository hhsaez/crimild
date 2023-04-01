crimild_trace()

find_package( Vulkan REQUIRED )

add_library( crimild_vulkan OBJECT )

target_sources( 
    crimild_vulkan

    PUBLIC
    Crimild_Vulkan.hpp

    PRIVATE
    Crimild_Vulkan.cpp
    
    Foundation/VulkanInitializers.hpp
    Foundation/VulkanUtils.cpp
    Foundation/VulkanUtils.hpp

    Rendering/FrameGraph/VulkanComputeBase.hpp
    Rendering/FrameGraph/VulkanComputeImageMix.cpp
    Rendering/FrameGraph/VulkanComputeImageMix.hpp
    Rendering/FrameGraph/VulkanComputeImageSwizzle.cpp
    Rendering/FrameGraph/VulkanComputeImageSwizzle.hpp
    Rendering/FrameGraph/VulkanFramegraphNode.hpp
    Rendering/FrameGraph/VulkanRenderBase.cpp
    Rendering/FrameGraph/VulkanRenderBase.hpp
    Rendering/FrameGraph/VulkanRenderScene.cpp
    Rendering/FrameGraph/VulkanRenderScene.hpp
    Rendering/FrameGraph/VulkanRenderSceneGBuffer.cpp
    Rendering/FrameGraph/VulkanRenderSceneGBuffer.hpp
    Rendering/FrameGraph/VulkanRenderSceneLighting.cpp
    Rendering/FrameGraph/VulkanRenderSceneLighting.hpp
    Rendering/FrameGraph/VulkanRenderSceneRT.cpp
    Rendering/FrameGraph/VulkanRenderSceneRT.hpp
    Rendering/FrameGraph/VulkanRenderSceneUnlit.cpp
    Rendering/FrameGraph/VulkanRenderSceneUnlit.hpp
    Rendering/FrameGraph/VulkanRenderShadowMaps.cpp
    Rendering/FrameGraph/VulkanRenderShadowMaps.hpp

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

    Rendering/VulkanBuffer.cpp
    Rendering/VulkanBuffer.hpp
    Rendering/VulkanCommandBuffer.cpp
    Rendering/VulkanCommandBuffer.hpp
    Rendering/VulkanComputePipeline.cpp
    Rendering/VulkanComputePipeline.hpp
    Rendering/VulkanDescriptor.hpp
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
    Rendering/VulkanImage.cpp
    Rendering/VulkanImage.hpp
    Rendering/VulkanImageView.cpp
    Rendering/VulkanImageView.hpp
    Rendering/VulkanInstance.cpp 
    Rendering/VulkanInstance.hpp
    Rendering/VulkanPhysicalDevice.cpp
    Rendering/VulkanPhysicalDevice.hpp
    Rendering/VulkanRenderDevice.cpp
    Rendering/VulkanRenderDevice.hpp
    Rendering/VulkanRenderDeviceCache.cpp
    Rendering/VulkanRenderDeviceCache.hpp
    Rendering/VulkanRenderPass.cpp
    Rendering/VulkanRenderPass.hpp
    Rendering/VulkanRenderTarget.cpp
    Rendering/VulkanRenderTarget.hpp
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
    Rendering/VulkanSurface.cpp
    Rendering/VulkanSurface.hpp
    Rendering/VulkanSynchronization.hpp
    Visitors/VulkanFetchSceneRenderState.cpp
    Visitors/VulkanFetchSceneRenderState.hpp
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
