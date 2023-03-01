crimild_trace()

find_package( Vulkan REQUIRED )

add_library( crimild_vulkan OBJECT )

target_sources( 
    crimild_vulkan

    PUBLIC
    Crimild_Vulkan.hpp

    PRIVATE
    Foundation/VulkanInitializers.hpp
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
    Rendering/VulkanFramebuffer.hpp
    Rendering/VulkanFramebufferAttachment.hpp
    Rendering/VulkanGraphicsPipeline.hpp
    Rendering/VulkanImage.hpp
    Rendering/VulkanImageView.hpp
    Rendering/VulkanInstance.hpp
    Rendering/VulkanPhysicalDevice.hpp
    Rendering/VulkanRenderDevice.hpp
    Rendering/VulkanRenderPass.hpp
    Rendering/VulkanSceneRenderState.hpp
    Rendering/VulkanShaderCompiler.hpp
    Rendering/VulkanShadowMap.hpp
    Rendering/VulkanSurface.hpp
    Visitors/VulkanFetchSceneRenderState.hpp

    PRIVATE
    Crimild_Vulkan.cpp
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
    Rendering/VulkanFramebuffer.cpp
    Rendering/VulkanGraphicsPipeline.cpp
    Rendering/VulkanImage.cpp
    Rendering/VulkanImageView.cpp
    Rendering/VulkanInstance.cpp 
    Rendering/VulkanPhysicalDevice.cpp
    Rendering/VulkanRenderDevice.cpp
    Rendering/VulkanRenderPass.cpp
    Rendering/VulkanShaderCompiler.cpp
    Rendering/VulkanShadowMap.cpp
    Rendering/VulkanSurface.cpp
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
