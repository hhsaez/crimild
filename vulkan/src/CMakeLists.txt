crimild_trace()

find_package( Vulkan )

if ( NOT Vulkan_FOUND )
    if ( WIN32 )
        find_library( Vulkan_LIBRARY NAMES vulkan-1 vulkan PATHS ${PROJECT_SOURCE_DIR}/third-party/vulkan/lib )
        if ( Vulkan_LIBRARY )
            message( "Using bundled Vulkan libraries" )
            set( Vulkan_FOUND ON )
            set( Vulkan_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/third-party/vulkan/include )
        endif ()
    elseif ( LINUX )
        find_library( Vulkan_LIBRARY NAMES vulkan HINTS "$ENV{VULKAN_SDK}/lib" "${PROJECT_SOURCE_DIR}/third-party/vulkan/lib" )
        if ( Vulkan_LIBRARY )
            message( "Using bundled Vulkan libraries" )
            set( Vulkan_FOUND ON )
            set( Vulkan_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/third-party/vulkan/include )
        endif ()
    elseif ( APPLE )
        message( "Using bundled Vulkan libraries" )
        set( Vulkan_FOUND ON )
        set( Vulkan_LIBRARY ${PROJECT_SOURCE_DIR}/third-party/vulkan/lib/libvulkan.dylib )
        set( Vulkan_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/third-party/vulkan/include )
    endif ()
endif ()

if ( NOT Vulkan_FOUND )
    message( FATAL_ERROR "Could not find Vulkan libraries" )
else ()
    message( STATUS ${Vulkan_LIBRARY} )
endif ()

add_library( crimild_vulkan OBJECT )

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

    PRIVATE Rendering/RenderPasses/VulkanBlitPass.cpp
    PRIVATE Rendering/RenderPasses/VulkanBlitPass.hpp
    PRIVATE Rendering/RenderPasses/VulkanClearPass.cpp
    PRIVATE Rendering/RenderPasses/VulkanClearPass.hpp
    PRIVATE Rendering/RenderPasses/VulkanDebugAttachmentPass.cpp
    PRIVATE Rendering/RenderPasses/VulkanDebugAttachmentPass.hpp
    PRIVATE Rendering/RenderPasses/VulkanGBufferPass.cpp
    PRIVATE Rendering/RenderPasses/VulkanGBufferPass.hpp
    PRIVATE Rendering/RenderPasses/VulkanGenerateDirectionalLightsShadowMapsPass.cpp
    PRIVATE Rendering/RenderPasses/VulkanGenerateDirectionalLightsShadowMapsPass.hpp
    PRIVATE Rendering/RenderPasses/VulkanGeneratePointLightsShadowMapsPass.cpp
    PRIVATE Rendering/RenderPasses/VulkanGeneratePointLightsShadowMapsPass.hpp
    PRIVATE Rendering/RenderPasses/VulkanGenerateSpotLightsShadowMapsPass.cpp 
    PRIVATE Rendering/RenderPasses/VulkanGenerateSpotLightsShadowMapsPass.hpp
    PRIVATE Rendering/RenderPasses/VulkanLocalLightingPass.cpp
    PRIVATE Rendering/RenderPasses/VulkanLocalLightingPass.hpp
    PRIVATE Rendering/RenderPasses/VulkanOverlayPass.cpp
    PRIVATE Rendering/RenderPasses/VulkanOverlayPass.hpp
    PRIVATE Rendering/RenderPasses/VulkanPresentPass.cpp
    PRIVATE Rendering/RenderPasses/VulkanPresentPass.hpp
    PRIVATE Rendering/RenderPasses/VulkanRenderPassBase.cpp
    PRIVATE Rendering/RenderPasses/VulkanRenderPassBase.hpp
    PRIVATE Rendering/RenderPasses/VulkanSceneDebugPass.cpp
    PRIVATE Rendering/RenderPasses/VulkanSceneDebugPass.hpp
    PRIVATE Rendering/RenderPasses/VulkanScenePass.cpp
    PRIVATE Rendering/RenderPasses/VulkanScenePass.hpp
    PRIVATE Rendering/RenderPasses/VulkanSelectionOutlinePass.cpp
    PRIVATE Rendering/RenderPasses/VulkanSelectionOutlinePass.hpp
    PRIVATE Rendering/RenderPasses/VulkanShaderPass.cpp
    PRIVATE Rendering/RenderPasses/VulkanShaderPass.hpp
    PRIVATE Rendering/RenderPasses/VulkanShadowPass.cpp
    PRIVATE Rendering/RenderPasses/VulkanShadowPass.hpp
    PRIVATE Rendering/RenderPasses/VulkanSoftRTPass.cpp
    PRIVATE Rendering/RenderPasses/VulkanSoftRTPass.hpp
    PRIVATE Rendering/RenderPasses/VulkanUnlitPass.cpp
    PRIVATE Rendering/RenderPasses/VulkanUnlitPass.hpp

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
	PUBLIC . 
    # Make sure to include GLSLang headers before Vulkan's so there is no conflict and
    # the app will run correctly. Otherwise, you may seem errors when compiling shaders
    # due to invalid resource settings (like when using gl_VertexIdentifier in arrays)
    PUBLIC ${PROJECT_SOURCE_DIR}/third-party/glslang
	PUBLIC ${Vulkan_INCLUDE_DIRS} 
)

target_link_libraries( 
	crimild_vulkan
	PRIVATE Crimild::Core

    # Vulkan libraries are made public so they can be used in other projects, like 
    # the editor or players
	PUBLIC ${Vulkan_LIBRARY}
    PUBLIC glslang
    PUBLIC SPIRV 
)

add_library( Crimild::Vulkan ALIAS crimild_vulkan )

# Keep IDEs folders clean
set_target_properties( glslang PROPERTIES FOLDER extern )
set_target_properties( SPIRV PROPERTIES FOLDER extern )
