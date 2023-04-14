// Dear ImGui: standalone example application for Glfw + Vulkan
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// Important note to the reader who wish to integrate imgui_impl_vulkan.cpp/.h in their own engine/app.
// - Common ImGui_ImplVulkan_XXX functions and structures are used to interface with imgui_impl_vulkan.cpp/.h.
//   You will use those if you want to use this rendering backend in your engine/app.
// - Helper ImGui_ImplVulkanH_XXX functions and structures are only used by this example (main.cpp) and by
//   the backend itself (imgui_impl_vulkan.cpp), but should PROBABLY NOT be used by your own engine/app code.
// Read comments in imgui_impl_vulkan.h.

#include "Foundation/GLFWUtils.hpp"
#include "Foundation/ImGuiUtils.hpp"
#include "Panels/BehaviorsPanel.hpp"
#include "Panels/ConsolePanel.hpp"
#include "Panels/InspectorPanel.hpp"
#include "Panels/MainMenuBar.hpp"
#include "Panels/PlaybackControlsPanel.hpp"
#include "Panels/ProjectPanel.hpp"
#include "Panels/SceneHierarchyPanel.hpp"
#include "Panels/ScenePanel.hpp"
#include "Panels/SceneRTPanel.hpp"
#include "Panels/SimulationPanel.hpp"
#include "Panels/TimelinePanel.hpp"
#include "Rendering/STBImageManager.hpp"
#include "SceneGraph/PrefabNode.hpp"
#include "Simulation/Editor.hpp"
#include "Simulation/Project.hpp"

#include <Crimild.hpp>
#include <Crimild_Vulkan.hpp>

namespace crimild::vulkan {

    class GLFWVulkanSurface : public vulkan::VulkanSurface {
    public:
        GLFWVulkanSurface( VulkanInstance *instance, GLFWwindow *window ) noexcept
            : vulkan::VulkanSurface(
                instance,
                [ & ] {
                    CRIMILD_LOG_DEBUG( "Creating GLFW Vulkan Surface" );
                    VkSurfaceKHR handle;
                    auto result = glfwCreateWindowSurface(
                        instance->getHandle(),
                        window,
                        nullptr,
                        &handle
                    );
                    if ( result != VK_SUCCESS ) {
                        CRIMILD_LOG_FATAL( "Failed to create window surface for Vulkan. Error: ", result );
                        exit( -1 );
                    }
                    return handle;
                }()
            )
        {
            // no-op
        }

        virtual ~GLFWVulkanSurface( void ) noexcept = default;
    };

}

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
// #if defined( _MSC_VER ) && ( _MSC_VER >= 1900 ) && !defined( IMGUI_DISABLE_WIN32_FUNCTIONS )
//     #pragma comment( lib, "legacy_stdio_definitions" )
// #endif

// #define IMGUI_UNLIMITED_FRAME_RATE
// #ifdef _DEBUG
#define IMGUI_VULKAN_DEBUG_REPORT
// #endif

// Data
static VkAllocationCallbacks *g_Allocator = NULL;
static VkInstance g_Instance = VK_NULL_HANDLE;
static VkPhysicalDevice g_PhysicalDevice = VK_NULL_HANDLE;
static VkDevice g_Device = VK_NULL_HANDLE;
static uint32_t g_QueueFamily = ( uint32_t ) -1;
static VkQueue g_Queue = VK_NULL_HANDLE;
static VkDebugReportCallbackEXT g_DebugReport = VK_NULL_HANDLE;
static VkPipelineCache g_PipelineCache = VK_NULL_HANDLE;
static VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;

static ImGui_ImplVulkanH_Window g_MainWindowData;
static int g_MinImageCount = 2;
static bool g_SwapChainRebuild = false;

static void glfw_error_callback( int error, const char *description )
{
    fprintf( stderr, "GLFW Error %d: %s\n", error, description );
}
static void check_vk_result( VkResult err )
{
    if ( err == 0 )
        return;
    fprintf( stderr, "[vulkan] Error: VkResult = %d\n", err );
    if ( err < 0 )
        abort();
}

#ifdef IMGUI_VULKAN_DEBUG_REPORT
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report( VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char *pLayerPrefix, const char *pMessage, void *pUserData )
{
    ( void ) flags;
    ( void ) object;
    ( void ) location;
    ( void ) messageCode;
    ( void ) pUserData;
    ( void ) pLayerPrefix; // Unused arguments
    fprintf( stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", objectType, pMessage );
    return VK_FALSE;
}
#endif // IMGUI_VULKAN_DEBUG_REPORT

static void SetupVulkan( void ) noexcept ///*const char **extensions, uint32_t extensions_count*/ )
{
    VkResult err;

    //     // Create Vulkan Instance
    //     {
    //         VkInstanceCreateInfo create_info = {};
    //         create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    //         create_info.enabledExtensionCount = extensions_count;
    //         create_info.ppEnabledExtensionNames = extensions;
    // #ifdef IMGUI_VULKAN_DEBUG_REPORT
    //         // Enabling validation layers
    //         const char *layers[] = { "VK_LAYER_KHRONOS_validation" };
    //         create_info.enabledLayerCount = 1;
    //         create_info.ppEnabledLayerNames = layers;

    //         // Enable debug report extension (we need additional storage, so we duplicate the user array to add our new extension to it)
    //         const char **extensions_ext = ( const char ** ) malloc( sizeof( const char * ) * ( extensions_count + 2 ) );
    //         memcpy( extensions_ext, extensions, extensions_count * sizeof( const char * ) );
    //         extensions_ext[ extensions_count + 0 ] = "VK_EXT_debug_report";
    //         extensions_ext[ extensions_count + 1 ] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
    //         create_info.enabledExtensionCount = extensions_count + 2;
    //         create_info.ppEnabledExtensionNames = extensions_ext;
    //         create_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    //         // Create Vulkan Instance
    //         err = vkCreateInstance( &create_info, g_Allocator, &g_Instance );
    //         check_vk_result( err );
    //         free( extensions_ext );

    //         // Get the function pointer (required for any extensions)
    //         auto vkCreateDebugReportCallbackEXT = ( PFN_vkCreateDebugReportCallbackEXT ) vkGetInstanceProcAddr( g_Instance, "vkCreateDebugReportCallbackEXT" );
    //         IM_ASSERT( vkCreateDebugReportCallbackEXT != NULL );

    //         // Setup the debug report callback
    //         VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
    //         debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    //         debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    //         debug_report_ci.pfnCallback = debug_report;
    //         debug_report_ci.pUserData = NULL;
    //         err = vkCreateDebugReportCallbackEXT( g_Instance, &debug_report_ci, g_Allocator, &g_DebugReport );
    //         check_vk_result( err );
    // #else
    //         // Create Vulkan Instance without any debug feature
    //         err = vkCreateInstance( &create_info, g_Allocator, &g_Instance );
    //         check_vk_result( err );
    //         IM_UNUSED( g_DebugReport );
    // #endif
    //     }

    // // Select GPU
    // {
    //     uint32_t gpu_count;
    //     err = vkEnumeratePhysicalDevices( g_Instance, &gpu_count, NULL );
    //     check_vk_result( err );
    //     IM_ASSERT( gpu_count > 0 );

    //     VkPhysicalDevice *gpus = ( VkPhysicalDevice * ) malloc( sizeof( VkPhysicalDevice ) * gpu_count );
    //     err = vkEnumeratePhysicalDevices( g_Instance, &gpu_count, gpus );
    //     check_vk_result( err );

    //     // If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available. This covers
    //     // most common cases (multi-gpu/integrated+dedicated graphics). Handling more complicated setups (multiple
    //     // dedicated GPUs) is out of scope of this sample.
    //     int use_gpu = 0;
    //     for ( int i = 0; i < ( int ) gpu_count; i++ ) {
    //         VkPhysicalDeviceProperties properties;
    //         vkGetPhysicalDeviceProperties( gpus[ i ], &properties );
    //         if ( properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ) {
    //             use_gpu = i;
    //             break;
    //         }
    //     }

    //     g_PhysicalDevice = gpus[ use_gpu ];
    //     free( gpus );
    // }

    // // Select graphics queue family
    // {
    //     uint32_t count;
    //     vkGetPhysicalDeviceQueueFamilyProperties( g_PhysicalDevice, &count, NULL );
    //     VkQueueFamilyProperties *queues = ( VkQueueFamilyProperties * ) malloc( sizeof( VkQueueFamilyProperties ) * count );
    //     vkGetPhysicalDeviceQueueFamilyProperties( g_PhysicalDevice, &count, queues );
    //     for ( uint32_t i = 0; i < count; i++ )
    //         if ( queues[ i ].queueFlags & VK_QUEUE_GRAPHICS_BIT ) {
    //             g_QueueFamily = i;
    //             break;
    //         }
    //     free( queues );
    //     IM_ASSERT( g_QueueFamily != ( uint32_t ) -1 );
    // }

    // // Create Logical Device (with 1 queue)
    // {
    //     int device_extension_count = 1;
    //     const char *device_extensions[] = { "VK_KHR_swapchain", "VK_KHR_portability_subset" };
    //     const float queue_priority[] = { 1.0f };
    //     VkDeviceQueueCreateInfo queue_info[ 1 ] = {};
    //     queue_info[ 0 ].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    //     queue_info[ 0 ].queueFamilyIndex = g_QueueFamily;
    //     queue_info[ 0 ].queueCount = 1;
    //     queue_info[ 0 ].pQueuePriorities = queue_priority;
    //     VkDeviceCreateInfo create_info = {};
    //     create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    //     create_info.queueCreateInfoCount = sizeof( queue_info ) / sizeof( queue_info[ 0 ] );
    //     create_info.pQueueCreateInfos = queue_info;
    //     create_info.enabledExtensionCount = device_extension_count;
    //     create_info.ppEnabledExtensionNames = device_extensions;
    //     err = vkCreateDevice( g_PhysicalDevice, &create_info, g_Allocator, &g_Device );
    //     check_vk_result( err );
    //     vkGetDeviceQueue( g_Device, g_QueueFamily, 0, &g_Queue );
    // }

    // Create Descriptor Pool
    {
        VkDescriptorPoolSize pool_sizes[] = {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000 * IM_ARRAYSIZE( pool_sizes );
        pool_info.poolSizeCount = ( uint32_t ) IM_ARRAYSIZE( pool_sizes );
        pool_info.pPoolSizes = pool_sizes;
        err = vkCreateDescriptorPool( g_Device, &pool_info, g_Allocator, &g_DescriptorPool );
        check_vk_result( err );
    }
}

// All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
// Your real engine/app may not use them.
static void SetupVulkanWindow( ImGui_ImplVulkanH_Window *wd, VkSurfaceKHR surface, int width, int height )
{
    wd->Surface = surface;

    // Check for WSI support
    VkBool32 res;
    vkGetPhysicalDeviceSurfaceSupportKHR( g_PhysicalDevice, g_QueueFamily, wd->Surface, &res );
    if ( res != VK_TRUE ) {
        fprintf( stderr, "Error no WSI support on physical device 0\n" );
        exit( -1 );
    }

    // Select Surface Format
    const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
    const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat( g_PhysicalDevice, wd->Surface, requestSurfaceImageFormat, ( size_t ) IM_ARRAYSIZE( requestSurfaceImageFormat ), requestSurfaceColorSpace );

    // Select Present Mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
    wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode( g_PhysicalDevice, wd->Surface, &present_modes[ 0 ], IM_ARRAYSIZE( present_modes ) );
    // printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

    // Create SwapChain, RenderPass, Framebuffer, etc.
    IM_ASSERT( g_MinImageCount >= 2 );
    ImGui_ImplVulkanH_CreateOrResizeWindow( g_Instance, g_PhysicalDevice, g_Device, wd, g_QueueFamily, g_Allocator, width, height, g_MinImageCount );
}

static void CleanupVulkan()
{
    vkDestroyDescriptorPool( g_Device, g_DescriptorPool, g_Allocator );

    // #ifdef IMGUI_VULKAN_DEBUG_REPORT
    //     // Remove the debug report callback
    //     auto vkDestroyDebugReportCallbackEXT = ( PFN_vkDestroyDebugReportCallbackEXT ) vkGetInstanceProcAddr( g_Instance, "vkDestroyDebugReportCallbackEXT" );
    //     vkDestroyDebugReportCallbackEXT( g_Instance, g_DebugReport, g_Allocator );
    // #endif // IMGUI_VULKAN_DEBUG_REPORT

    //     vkDestroyDevice( g_Device, g_Allocator );
    //     vkDestroyInstance( g_Instance, g_Allocator );
}

static void CleanupVulkanWindow()
{
    ImGui_ImplVulkanH_DestroyWindow( g_Instance, g_Device, &g_MainWindowData, g_Allocator );
}

static void FrameRender( ImGui_ImplVulkanH_Window *wd, ImDrawData *draw_data )
{
    VkResult err;

    VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[ wd->SemaphoreIndex ].ImageAcquiredSemaphore;
    VkSemaphore render_complete_semaphore = wd->FrameSemaphores[ wd->SemaphoreIndex ].RenderCompleteSemaphore;
    err = vkAcquireNextImageKHR( g_Device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex );
    if ( err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR ) {
        g_SwapChainRebuild = true;
        return;
    }
    check_vk_result( err );

    ImGui_ImplVulkanH_Frame *fd = &wd->Frames[ wd->FrameIndex ];
    {
        err = vkWaitForFences( g_Device, 1, &fd->Fence, VK_TRUE, UINT64_MAX ); // wait indefinitely instead of periodically checking
        check_vk_result( err );

        err = vkResetFences( g_Device, 1, &fd->Fence );
        check_vk_result( err );
    }
    {
        err = vkResetCommandPool( g_Device, fd->CommandPool, 0 );
        check_vk_result( err );
        VkCommandBufferBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        err = vkBeginCommandBuffer( fd->CommandBuffer, &info );
        check_vk_result( err );
    }
    {
        VkRenderPassBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.renderPass = wd->RenderPass;
        info.framebuffer = fd->Framebuffer;
        info.renderArea.extent.width = wd->Width;
        info.renderArea.extent.height = wd->Height;
        info.clearValueCount = 1;
        info.pClearValues = &wd->ClearValue;
        vkCmdBeginRenderPass( fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE );
    }

    // Record dear imgui primitives into command buffer
    ImGui_ImplVulkan_RenderDrawData( draw_data, fd->CommandBuffer );

    // Submit command buffer
    vkCmdEndRenderPass( fd->CommandBuffer );
    {
        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &image_acquired_semaphore;
        info.pWaitDstStageMask = &wait_stage;
        info.commandBufferCount = 1;
        info.pCommandBuffers = &fd->CommandBuffer;
        info.signalSemaphoreCount = 1;
        info.pSignalSemaphores = &render_complete_semaphore;

        err = vkEndCommandBuffer( fd->CommandBuffer );
        check_vk_result( err );
        err = vkQueueSubmit( g_Queue, 1, &info, fd->Fence );
        check_vk_result( err );
    }
}

static void FramePresent( ImGui_ImplVulkanH_Window *wd )
{
    if ( g_SwapChainRebuild )
        return;
    VkSemaphore render_complete_semaphore = wd->FrameSemaphores[ wd->SemaphoreIndex ].RenderCompleteSemaphore;
    VkPresentInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &render_complete_semaphore;
    info.swapchainCount = 1;
    info.pSwapchains = &wd->Swapchain;
    info.pImageIndices = &wd->FrameIndex;
    VkResult err = vkQueuePresentKHR( g_Queue, &info );
    if ( err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR ) {
        g_SwapChainRebuild = true;
        return;
    }
    check_vk_result( err );
    wd->SemaphoreIndex = ( wd->SemaphoreIndex + 1 ) % wd->ImageCount; // Now we can use the next set of semaphores
}

struct VulkanObjects {

    std::unique_ptr< crimild::vulkan::VulkanInstance > instance;
    std::unique_ptr< crimild::vulkan::GLFWVulkanSurface > surface;
    std::unique_ptr< crimild::vulkan::PhysicalDevice > physicalDevice;
    std::unique_ptr< crimild::vulkan::RenderDevice > renderDevice;

    void init( GLFWwindow *window ) noexcept
    {
        instance = std::make_unique< crimild::vulkan::VulkanInstance >();
        surface = std::make_unique< crimild::vulkan::GLFWVulkanSurface >( instance.get(), window );
        physicalDevice = instance->createPhysicalDevice( surface.get() );
        renderDevice = physicalDevice->createRenderDevice();
    }

    void cleanup( void ) noexcept
    {
        if ( renderDevice != nullptr ) {
            renderDevice->flush();
        }

        renderDevice = nullptr;
        physicalDevice = nullptr;
        surface = nullptr;
        instance = nullptr;
    }
};

inline void setupImGuiStyles( bool dark, float alpha )
{
    ImGuiStyle &style = ImGui::GetStyle();

    style.WindowRounding = 5.3f;
    style.FrameRounding = 2.3f;
    style.ScrollbarRounding = 0;

    style.Colors[ ImGuiCol_Text ] = ImVec4( 0.90f, 0.90f, 0.90f, 0.90f );
    style.Colors[ ImGuiCol_TextDisabled ] = ImVec4( 0.60f, 0.60f, 0.60f, 1.00f );
    style.Colors[ ImGuiCol_WindowBg ] = ImVec4( 0.09f, 0.09f, 0.15f, 1.00f );
    style.Colors[ ImGuiCol_ChildBg ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
    style.Colors[ ImGuiCol_PopupBg ] = ImVec4( 0.05f, 0.05f, 0.10f, 0.85f );
    style.Colors[ ImGuiCol_Border ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
    style.Colors[ ImGuiCol_BorderShadow ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
    style.Colors[ ImGuiCol_FrameBg ] = ImVec4( 0.00f, 0.00f, 0.01f, 1.00f );
    style.Colors[ ImGuiCol_FrameBgHovered ] = ImVec4( 0.90f, 0.80f, 0.80f, 0.40f );
    style.Colors[ ImGuiCol_FrameBgActive ] = ImVec4( 0.90f, 0.65f, 0.65f, 0.45f );
    style.Colors[ ImGuiCol_TitleBg ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.83f );
    style.Colors[ ImGuiCol_TitleBgCollapsed ] = ImVec4( 0.40f, 0.40f, 0.80f, 0.20f );
    style.Colors[ ImGuiCol_TitleBgActive ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.87f );
    style.Colors[ ImGuiCol_MenuBarBg ] = ImVec4( 0.01f, 0.01f, 0.02f, 0.80f );
    style.Colors[ ImGuiCol_ScrollbarBg ] = ImVec4( 0.20f, 0.25f, 0.30f, 0.60f );
    style.Colors[ ImGuiCol_ScrollbarGrab ] = ImVec4( 0.55f, 0.53f, 0.55f, 0.51f );
    style.Colors[ ImGuiCol_ScrollbarGrabHovered ] = ImVec4( 0.56f, 0.56f, 0.56f, 1.00f );
    style.Colors[ ImGuiCol_ScrollbarGrabActive ] = ImVec4( 0.56f, 0.56f, 0.56f, 0.91f );
    // style.Colors[ ImGuiCol_ComboBg ] = ImVec4( 0.1f, 0.1f, 0.1f, 0.99f );
    style.Colors[ ImGuiCol_CheckMark ] = ImVec4( 0.90f, 0.90f, 0.90f, 0.83f );
    style.Colors[ ImGuiCol_SliderGrab ] = ImVec4( 0.70f, 0.70f, 0.70f, 0.62f );
    style.Colors[ ImGuiCol_SliderGrabActive ] = ImVec4( 0.30f, 0.30f, 0.30f, 0.84f );
    style.Colors[ ImGuiCol_Button ] = ImVec4( 0.48f, 0.72f, 0.89f, 0.49f );
    style.Colors[ ImGuiCol_ButtonHovered ] = ImVec4( 0.50f, 0.69f, 0.99f, 0.68f );
    style.Colors[ ImGuiCol_ButtonActive ] = ImVec4( 0.80f, 0.50f, 0.50f, 1.00f );
    style.Colors[ ImGuiCol_Header ] = ImVec4( 0.30f, 0.69f, 1.00f, 0.53f );
    style.Colors[ ImGuiCol_HeaderHovered ] = ImVec4( 0.44f, 0.61f, 0.86f, 1.00f );
    style.Colors[ ImGuiCol_HeaderActive ] = ImVec4( 0.38f, 0.62f, 0.83f, 1.00f );
    style.Colors[ ImGuiCol_Separator ] = ImVec4( 0.50f, 0.50f, 0.50f, 1.00f );
    style.Colors[ ImGuiCol_SeparatorHovered ] = ImVec4( 0.70f, 0.60f, 0.60f, 1.00f );
    style.Colors[ ImGuiCol_SeparatorActive ] = ImVec4( 0.90f, 0.70f, 0.70f, 1.00f );
    style.Colors[ ImGuiCol_ResizeGrip ] = ImVec4( 1.00f, 1.00f, 1.00f, 0.85f );
    style.Colors[ ImGuiCol_ResizeGripHovered ] = ImVec4( 1.00f, 1.00f, 1.00f, 0.60f );
    style.Colors[ ImGuiCol_ResizeGripActive ] = ImVec4( 1.00f, 1.00f, 1.00f, 0.90f );
    // style.Colors[ ImGuiCol_Tab ] = ImVec4( 0.48f, 0.72f, 0.89f, 0.49f );
    // style.Colors[ ImGuiCol_TabHovered ] = ImVec4( 0.50f, 0.69f, 0.99f, 0.68f );
    // style.Colors[ ImGuiCol_TabActive ] = ImVec4( 0.80f, 0.50f, 0.50f, 1.00f );
    // style.Colors[ ImGuiCol_TabUnfocused ] = ImVec4( 0.48f, 0.72f, 0.89f, 0.29f );
    // style.Colors[ ImGuiCol_TabUnfocusedActive ] = ImVec4( 0.80f, 0.50f, 0.50f, 0.68f );
    // style.Colors[ ImGuiCol_DockingPreview ] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
    // style.Colors[ ImGuiCol_DockingEmptyBg ] = ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
    style.Colors[ ImGuiCol_PlotLines ] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
    style.Colors[ ImGuiCol_PlotLinesHovered ] = ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
    style.Colors[ ImGuiCol_PlotHistogram ] = ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
    style.Colors[ ImGuiCol_PlotHistogramHovered ] = ImVec4( 1.00f, 0.60f, 0.00f, 1.00f );
    // style.Colors[ ImGuiCol_TableHeaderBg ] = ImVec4( 0.00f, 0.00f, 1.00f, 0.35f );
    // style.Colors[ ImGuiCol_TableBorderStrong ] = ImVec4( 0.00f, 0.00f, 1.00f, 0.35f );
    // style.Colors[ ImGuiCol_TableBorderLight ] = ImVec4( 0.00f, 0.00f, 1.00f, 0.35f );
    // style.Colors[ ImGuiCol_TableRowBg ] = ImVec4( 0.00f, 0.00f, 1.00f, 0.35f );
    // style.Colors[ ImGuiCol_TableRowBgAlt ] = ImVec4( 0.00f, 0.00f, 1.00f, 0.35f );
    style.Colors[ ImGuiCol_TextSelectedBg ] = ImVec4( 0.00f, 0.00f, 1.00f, 0.35f );
    // style.Colors[ ImGuiCol_DragDropTarget ] = ImVec4( 0.00f, 0.00f, 1.00f, 0.35f );
    // style.Colors[ ImGuiCol_NavHighlight ] = ImVec4( 0.20f, 0.20f, 0.20f, 0.35f );
    // style.Colors[ ImGuiCol_NavWindowingHighlight ] = ImVec4( 0.20f, 0.20f, 0.20f, 0.35f );
    // style.Colors[ ImGuiCol_NavWindowingDimBg ] = ImVec4( 0.20f, 0.20f, 0.20f, 0.35f );
    style.Colors[ ImGuiCol_ModalWindowDimBg ] = ImVec4( 0.20f, 0.20f, 0.20f, 0.35f );
}

// Main code
int main( int argc, char **argv )
{
    crimild::init();
    crimild::vulkan::init();

    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::editor::Editor::State );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::editor::Project );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::PrefabNode );

    crimild::Log::setOutputHandlers(
        {
            std::make_shared< crimild::ConsoleOutputHandler >( crimild::Log::LOG_LEVEL_DEBUG ),
            std::make_shared< crimild::editor::panels::Console::OutputHandler >( crimild::Log::LOG_LEVEL_DEBUG ),
        }
    );

    crimild::Settings settings;
    settings.parseCommandLine( argc, argv );

    // This should not be needed anymore.
    // TODO: Deprecate JobScheduler?
    crimild::concurrency::JobScheduler jobScheduler;
    jobScheduler.configure( 0 );
    jobScheduler.start();

    // TODO: Move AssetManager here
    // TODO: Move AudioManager here
    crimild::editor::ImageManager imageManager;

    // Create simulation before creating the window, but
    // delay its actual start until the window has been
    // created
    auto simulation = crimild::Simulation::create();

    glfwSetErrorCallback( glfw_error_callback );
    if ( !glfwInit() )
        return 1;

    // Create window with Vulkan context
    glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
    GLFWwindow *window = glfwCreateWindow( 2560, 1440, "Crimild Editor", NULL, NULL );
    if ( !glfwVulkanSupported() ) {
        printf( "GLFW: Vulkan Not Supported\n" );
        return 1;
    }

    VulkanObjects vulkanObjects;
    vulkanObjects.init( window );

    // uint32_t extensions_count = 0;
    // const char **extensions = glfwGetRequiredInstanceExtensions( &extensions_count );
    // SetupVulkan( extensions, extensions_count );

    g_Instance = vulkanObjects.instance->getHandle();
    g_Allocator = vulkanObjects.instance->getAllocator();
    g_PhysicalDevice = vulkanObjects.physicalDevice->getHandle();
    g_Device = vulkanObjects.renderDevice->getHandle();
    g_QueueFamily = vulkanObjects.renderDevice->getGraphicsQueueFamily();
    g_Queue = vulkanObjects.renderDevice->getGraphicsQueue();
    SetupVulkan();

    // Create Window Surface
    VkSurfaceKHR surface;
    VkResult err = glfwCreateWindowSurface( g_Instance, window, g_Allocator, &surface );
    check_vk_result( err );

    // Create Framebuffers
    int w, h;
    glfwGetFramebufferSize( window, &w, &h );
    ImGui_ImplVulkanH_Window *wd = &g_MainWindowData;
    SetupVulkanWindow( wd, surface, w, h );

    // Set number of in-flight frames. This will result in one framegraph per frame
    vulkanObjects.renderDevice->configure( wd->ImageCount );

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ( void ) io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan( window, true );
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = g_Instance;
    init_info.PhysicalDevice = g_PhysicalDevice;
    init_info.Device = g_Device;
    init_info.QueueFamily = g_QueueFamily;
    init_info.Queue = g_Queue;
    init_info.PipelineCache = g_PipelineCache;
    init_info.DescriptorPool = g_DescriptorPool;
    init_info.Subpass = 0;
    init_info.MinImageCount = g_MinImageCount;
    init_info.ImageCount = wd->ImageCount;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = g_Allocator;
    init_info.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init( &init_info, wd->RenderPass );

    setupImGuiStyles( true, 1.0f );

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // io.Fonts->AddFontDefault();
    // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    // ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    // IM_ASSERT(font != NULL);

    // Upload Fonts
    {
        // Use any command queue
        VkCommandPool command_pool = wd->Frames[ wd->FrameIndex ].CommandPool;
        VkCommandBuffer command_buffer = wd->Frames[ wd->FrameIndex ].CommandBuffer;

        err = vkResetCommandPool( g_Device, command_pool, 0 );
        check_vk_result( err );
        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        err = vkBeginCommandBuffer( command_buffer, &begin_info );
        check_vk_result( err );

        ImGui_ImplVulkan_CreateFontsTexture( command_buffer );

        VkSubmitInfo end_info = {};
        end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        end_info.commandBufferCount = 1;
        end_info.pCommandBuffers = &command_buffer;
        err = vkEndCommandBuffer( command_buffer );
        check_vk_result( err );
        err = vkQueueSubmit( g_Queue, 1, &end_info, VK_NULL_HANDLE );
        check_vk_result( err );

        err = vkDeviceWaitIdle( g_Device );
        check_vk_result( err );
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    // Our state
    ImVec4 clear_color = ImVec4( 0.45f, 0.55f, 0.60f, 1.00f );

    struct Panels {
        Panels( crimild::vulkan::RenderDevice *renderDevice ) noexcept
            : scene( renderDevice ),
              simulation( renderDevice ),
              inspector( renderDevice ),
              sceneRT( renderDevice )
        {
            // no-op
        }

        crimild::editor::panels::MainMenuBar mainMenu;
        crimild::editor::panels::Timeline timeline;
        crimild::editor::panels::Behaviors behaviors;
        crimild::editor::panels::Scene scene;
        crimild::editor::panels::SceneHierarchy sceneHierarchy;
        crimild::editor::panels::Simulation simulation;
        crimild::editor::panels::PlaybackControls playbackControls;
        crimild::editor::panels::Inspector inspector;
        crimild::editor::panels::Project project;
        crimild::editor::panels::SceneRT sceneRT;
        crimild::editor::panels::Console console;

        void render( void ) noexcept
        {
            mainMenu.render();

            ImGui::DockSpaceOverViewport( ImGui::GetMainViewport() );

            timeline.render();
            behaviors.render();
            scene.render();
            sceneHierarchy.render();
            simulation.render();
            playbackControls.render();
            inspector.render();
            project.render();
            sceneRT.render();
            console.render();
        }
    };
    auto panels = std::make_unique< Panels >( vulkanObjects.renderDevice.get() );

    // Start simulation to init all systems, but then pause it.
    simulation->start();
    simulation->pause();

    // Main loop
    while ( !glfwWindowShouldClose( window ) ) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // This also dispatch any sync_frame calls
        jobScheduler.executeDelayedJobs();

        // Dispatch deferred messages
        crimild::MessageQueue::getInstance()->dispatchDeferredMessages();

        // Resize swap chain?
        if ( g_SwapChainRebuild ) {
            int width, height;
            glfwGetFramebufferSize( window, &width, &height );
            if ( width > 0 && height > 0 ) {
                ImGui_ImplVulkan_SetMinImageCount( g_MinImageCount );
                ImGui_ImplVulkanH_CreateOrResizeWindow( g_Instance, g_PhysicalDevice, g_Device, &g_MainWindowData, g_QueueFamily, g_Allocator, width, height, g_MinImageCount );
                g_MainWindowData.FrameIndex = 0;
                g_SwapChainRebuild = false;
            }
        }

        const auto tick = crimild::Event {
            .type = crimild::Event::Type::TICK,
        };
        if ( simulation->handle( tick ).type == crimild::Event::Type::TERMINATE ) {
            break;
        }

        // Start the Dear ImGui frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        vulkanObjects.renderDevice->setCurrentFrameIndex( wd->FrameIndex );
        vulkanObjects.renderDevice->getCache()->onBeforeFrame();
        panels->render();
        vulkanObjects.renderDevice->getCache()->onAfterFrame();

        // Rendering
        ImGui::Render();
        ImDrawData *draw_data = ImGui::GetDrawData();
        const bool is_minimized = ( draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f );
        if ( !is_minimized ) {
            wd->ClearValue.color.float32[ 0 ] = clear_color.x * clear_color.w;
            wd->ClearValue.color.float32[ 1 ] = clear_color.y * clear_color.w;
            wd->ClearValue.color.float32[ 2 ] = clear_color.z * clear_color.w;
            wd->ClearValue.color.float32[ 3 ] = clear_color.w;
            FrameRender( wd, draw_data );
            FramePresent( wd );
        }

        if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable ) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    crimild::PrefabNode::cleanup();

    jobScheduler.executeDelayedJobs();
    crimild::MessageQueue::getInstance()->dispatchDeferredMessages();

    jobScheduler.stop();
    crimild::MessageQueue::getInstance()->clear();

    panels = nullptr;

    // Cleanup
    err = vkDeviceWaitIdle( g_Device );
    check_vk_result( err );
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    CleanupVulkanWindow();
    CleanupVulkan();

    vulkanObjects.cleanup();

    glfwDestroyWindow( window );
    glfwTerminate();

    return 0;
}
