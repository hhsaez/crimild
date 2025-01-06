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
 *     * Neither the name of the copyright holder nor the
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

#include "Foundation/GLFWUtils.hpp"
#include "Foundation/ImGuiUtils.hpp"
#include "Layout/Layout.hpp"
#include "Layout/LayoutManager.hpp"
#include "Rendering/GLFWVulkanSurface.hpp"
#include "Rendering/STBImageManager.hpp"
#include "SceneGraph/PrefabNode.hpp"
#include "Simulation/Editor.hpp"
#include "Simulation/Project.hpp"
#include "Views/Menus/MainMenu/MainMenu.hpp"
#include "Views/Windows/BehaviorsWindow.hpp"
#include "Views/Windows/FileSystemWindow.hpp"
#include "Views/Windows/GraphEditor/GraphEditorWindow.hpp"
#include "Views/Windows/InspectorWindow.hpp"
#include "Views/Windows/LogWindow.hpp"
#include "Views/Windows/PlaybackControlsWindow.hpp"
#include "Views/Windows/Scene3DWindow.hpp"
#include "Views/Windows/SceneWindow.hpp"
#include "Views/Windows/SimulationWindow.hpp"
#include "Views/Windows/TimelineWindow.hpp"

#include <Crimild.hpp>
#include <Crimild_Vulkan.hpp>

#define IMGUI_VULKAN_DEBUG_REPORT

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

static void SetupVulkan( void ) noexcept
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
    VkResult err = vkCreateDescriptorPool( g_Device, &pool_info, g_Allocator, &g_DescriptorPool );
    check_vk_result( err );
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
    style.ScrollbarRounding = 10;

    style.Colors[ ImGuiCol_Text ] = ImVec4( 0.778f, 0.771f, 0.778f, 1.00f );
    style.Colors[ ImGuiCol_TextDisabled ] = ImVec4( 0.50f, 0.50f, 0.50f, 1.00f );
    style.Colors[ ImGuiCol_WindowBg ] = ImVec4( 0.163f, 0.153f, 0.163f, 1.00f );
    style.Colors[ ImGuiCol_ChildBg ] = ImVec4( 0.13f, 0.14f, 0.15f, 1.00f );
    style.Colors[ ImGuiCol_PopupBg ] = ImVec4( 0.13f, 0.14f, 0.15f, 1.00f );
    style.Colors[ ImGuiCol_Border ] = ImVec4( 0.087f, 0.076f, 0.0087f, 0.50f );
    style.Colors[ ImGuiCol_BorderShadow ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
    style.Colors[ ImGuiCol_FrameBg ] = ImVec4( 0.25f, 0.25f, 0.25f, 1.00f );
    style.Colors[ ImGuiCol_FrameBgHovered ] = ImVec4( 0.38f, 0.38f, 0.38f, 1.00f );
    style.Colors[ ImGuiCol_FrameBgActive ] = ImVec4( 0.67f, 0.67f, 0.67f, 0.39f );
    style.Colors[ ImGuiCol_TitleBg ] = ImVec4( 0.08f, 0.08f, 0.09f, 1.00f );
    style.Colors[ ImGuiCol_TitleBgActive ] = ImVec4( 0.08f, 0.08f, 0.09f, 1.00f );
    style.Colors[ ImGuiCol_TitleBgCollapsed ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.51f );
    style.Colors[ ImGuiCol_MenuBarBg ] = ImVec4( 0.14f, 0.14f, 0.14f, 1.00f );
    style.Colors[ ImGuiCol_ScrollbarBg ] = ImVec4( 0.02f, 0.02f, 0.02f, 0.53f );
    style.Colors[ ImGuiCol_ScrollbarGrab ] = ImVec4( 0.31f, 0.31f, 0.31f, 1.00f );
    style.Colors[ ImGuiCol_ScrollbarGrabHovered ] = ImVec4( 0.41f, 0.41f, 0.41f, 1.00f );
    style.Colors[ ImGuiCol_ScrollbarGrabActive ] = ImVec4( 0.51f, 0.51f, 0.51f, 1.00f );
    style.Colors[ ImGuiCol_CheckMark ] = ImVec4( 0.11f, 0.64f, 0.92f, 1.00f );
    style.Colors[ ImGuiCol_SliderGrab ] = ImVec4( 0.11f, 0.64f, 0.92f, 1.00f );
    style.Colors[ ImGuiCol_SliderGrabActive ] = ImVec4( 0.08f, 0.50f, 0.72f, 1.00f );
    style.Colors[ ImGuiCol_Button ] = ImVec4( 0.25f, 0.25f, 0.25f, 1.00f );
    style.Colors[ ImGuiCol_ButtonHovered ] = ImVec4( 0.38f, 0.38f, 0.38f, 1.00f );
    style.Colors[ ImGuiCol_ButtonActive ] = ImVec4( 0.67f, 0.67f, 0.67f, 0.39f );
    style.Colors[ ImGuiCol_Header ] = ImVec4( 0.22f, 0.22f, 0.22f, 1.00f );
    style.Colors[ ImGuiCol_HeaderHovered ] = ImVec4( 0.25f, 0.25f, 0.25f, 1.00f );
    style.Colors[ ImGuiCol_HeaderActive ] = ImVec4( 0.67f, 0.67f, 0.67f, 0.39f );
    style.Colors[ ImGuiCol_Separator ] = style.Colors[ ImGuiCol_Border ];
    style.Colors[ ImGuiCol_SeparatorHovered ] = ImVec4( 0.41f, 0.42f, 0.44f, 1.00f );
    style.Colors[ ImGuiCol_SeparatorActive ] = ImVec4( 0.26f, 0.59f, 0.98f, 0.95f );
    style.Colors[ ImGuiCol_ResizeGrip ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
    style.Colors[ ImGuiCol_ResizeGripHovered ] = ImVec4( 0.29f, 0.30f, 0.31f, 0.67f );
    style.Colors[ ImGuiCol_ResizeGripActive ] = ImVec4( 0.26f, 0.59f, 0.98f, 0.95f );
    style.Colors[ ImGuiCol_Tab ] = ImVec4( 0.163f, 0.153f, 0.163f, 1.0f );
    style.Colors[ ImGuiCol_TabHovered ] = ImVec4( 0.33f, 0.34f, 0.36f, 0.83f );
    style.Colors[ ImGuiCol_TabActive ] = ImVec4( 0.23f, 0.23f, 0.24f, 1.00f );
    style.Colors[ ImGuiCol_TabUnfocused ] = ImVec4( 0.08f, 0.08f, 0.09f, 1.00f );
    style.Colors[ ImGuiCol_TabUnfocusedActive ] = ImVec4( 0.13f, 0.14f, 0.15f, 1.00f );
    style.Colors[ ImGuiCol_DockingPreview ] = ImVec4( 0.26f, 0.59f, 0.98f, 0.70f );
    style.Colors[ ImGuiCol_DockingEmptyBg ] = ImVec4( 0.20f, 0.20f, 0.20f, 1.00f );
    style.Colors[ ImGuiCol_PlotLines ] = ImVec4( 0.61f, 0.61f, 0.61f, 1.00f );
    style.Colors[ ImGuiCol_PlotLinesHovered ] = ImVec4( 1.00f, 0.43f, 0.35f, 1.00f );
    style.Colors[ ImGuiCol_PlotHistogram ] = ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
    style.Colors[ ImGuiCol_PlotHistogramHovered ] = ImVec4( 1.00f, 0.60f, 0.00f, 1.00f );
    style.Colors[ ImGuiCol_TextSelectedBg ] = ImVec4( 0.26f, 0.59f, 0.98f, 0.35f );
    style.Colors[ ImGuiCol_DragDropTarget ] = ImVec4( 0.11f, 0.64f, 0.92f, 1.00f );
    style.Colors[ ImGuiCol_NavHighlight ] = ImVec4( 0.26f, 0.59f, 0.98f, 1.00f );
    style.Colors[ ImGuiCol_NavWindowingHighlight ] = ImVec4( 1.00f, 1.00f, 1.00f, 0.70f );
    style.Colors[ ImGuiCol_NavWindowingDimBg ] = ImVec4( 0.80f, 0.80f, 0.80f, 0.20f );
    style.Colors[ ImGuiCol_ModalWindowDimBg ] = ImVec4( 0.80f, 0.80f, 0.80f, 0.35f );
    style.GrabRounding = style.FrameRounding = 2.3f;
}

bool beginFrame(
    GLFWwindow *window,
    crimild::concurrency::JobScheduler &jobScheduler,
    std::unique_ptr< crimild::Simulation > &simulation
) noexcept
{
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
        return false;
    }

    // Start the Dear ImGui frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    return true;
}

void endFrame( ImGuiIO &io, ImGui_ImplVulkanH_Window *wd ) noexcept
{
    // Rendering
    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();
    const bool is_minimized = ( draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f );
    if ( !is_minimized ) {
        ImVec4 clear_color = ImVec4( 0.45f, 0.55f, 0.60f, 1.00f );
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

bool renderFrame(
    GLFWwindow *window,
    ImGuiIO &io,
    ImGui_ImplVulkanH_Window *wd,
    crimild::concurrency::JobScheduler &jobScheduler,
    std::unique_ptr< crimild::Simulation > &simulation,
    VulkanObjects &vulkanObjects,
    std::unique_ptr< crimild::editor::LayoutManager > &layoutManager
) noexcept
{
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    glfwPollEvents();

    if ( !beginFrame( window, jobScheduler, simulation ) ) {
        return false;
    }

    vulkanObjects.renderDevice->setCurrentFrameIndex( wd->FrameIndex );
    vulkanObjects.renderDevice->getCache()->onBeforeFrame();

    if ( auto layout = layoutManager->getCurrentLayout() ) {
        layout->draw();
    }

    vulkanObjects.renderDevice->getCache()->onAfterFrame();

    endFrame( io, wd );

    return true;
}

void loadFonts( ImGui_ImplVulkanH_Window *wd ) noexcept
{
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

    // {
    //     ImFontConfig config;
    //     config.SizePixels = 18;
    //     config.OversampleH = config.OversampleV = 1;
    //     config.PixelSnapH = true;
    //     io.Fonts->AddFontDefault( &config );
    // }

    // Upload Fonts
    // Use any command queue
    VkCommandPool command_pool = wd->Frames[ wd->FrameIndex ].CommandPool;
    VkCommandBuffer command_buffer = wd->Frames[ wd->FrameIndex ].CommandBuffer;

    VkResult err = vkResetCommandPool( g_Device, command_pool, 0 );
    check_vk_result( err );
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    err = vkBeginCommandBuffer( command_buffer, &begin_info );
    check_vk_result( err );

    // ImGui_ImplVulkan_CreateFontsTexture( command_buffer );

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
    // ImGui_ImplVulkan_DestroyFontUploadObjects();
}

int main( int argc, char **argv )
{
    crimild::init();
    crimild::vulkan::init();

    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::editor::Layout );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::editor::MainMenu );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::editor::FileSystemWindow );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::editor::InspectorWindow );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::editor::GraphEditorWindow );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::editor::LogWindow );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::editor::BehaviorsWindow );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::editor::PlaybackControlsWindow );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::editor::Scene3DWindow );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::editor::SceneWindow );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::editor::SimulationWindow );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::editor::TimelineWindow );

    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::editor::Editor::State );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::editor::Project );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::PrefabNode );

    crimild::Log::setOutputHandlers(
        {
            //            std::make_shared< crimild::ConsoleOutputHandler >( crimild::Log::LOG_LEVEL_DEBUG ),
            std::make_shared< crimild::editor::LogWindow::OutputHandler >( crimild::Log::LOG_LEVEL_DEBUG ),
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

    // Loads layout manager early so we can get the window's dimensions from
    // the default layout, if any
    auto layoutManager = std::make_unique< crimild::editor::LayoutManager >();

    glfwSetErrorCallback( glfw_error_callback );
    if ( !glfwInit() )
        return 1;

    // Create window with Vulkan context
    glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
    const int windowWidth = [ & ] {
        int width = 2560;
        if ( auto layout = layoutManager->getCurrentLayout() ) {
            if ( layout->getExtent().width > 0 ) {
                width = layout->getExtent().width;
            }
        }
        return width;
    }();
    const int windowHeight = [ & ] {
        int height = 1440;
        if ( auto layout = layoutManager->getCurrentLayout() ) {
            if ( layout->getExtent().height > 0 ) {
                height = layout->getExtent().height;
            }
        }
        return height;
    }();
    const std::string windowTitle = [ & ] {
        std::string title = "Crimild Editor";
        if ( auto layout = layoutManager->getCurrentLayout() ) {
            title += " (" + layout->getName() + ")";
        }
        return title;
    }();
    GLFWwindow *window = glfwCreateWindow( windowWidth, windowHeight, windowTitle.c_str(), NULL, NULL );
    if ( !glfwVulkanSupported() ) {
        printf( "GLFW: Vulkan Not Supported\n" );
        return 1;
    }

    VulkanObjects vulkanObjects;
    vulkanObjects.init( window );
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
    init_info.RenderPass = wd->RenderPass;
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
    ImGui_ImplVulkan_Init( &init_info );

    setupImGuiStyles( true, 1.0f );

    loadFonts( wd );

    // Our state

    if ( auto layout = layoutManager->getCurrentLayout() ) {
        layout->makeCurrent();
    }

    // Start simulation to init all systems, but then pause it.
    simulation->start();
    simulation->pause();

    // Main loop
    while ( !glfwWindowShouldClose( window ) ) {
        // Get frame time
        // This will be used to limit frame rate if needed.
        const auto frameStartTime = glfwGetTime();

        if ( !glfwGetWindowAttrib( window, GLFW_VISIBLE ) || glfwGetWindowAttrib( window, GLFW_ICONIFIED ) ) {
            // If the application is minimized or not visible, block main thread until we
            // receive events
            glfwWaitEvents();
        }

        if ( !renderFrame( window, io, wd, jobScheduler, simulation, vulkanObjects, layoutManager ) ) {
            break;
        }

        // Limit frame rate
        constexpr uint32_t TARGET_FPS = 60;
        const double frameTime = glfwGetTime() - frameStartTime;
        const double targetFrameTime = 1.0 / TARGET_FPS;
        if ( frameTime < targetFrameTime ) {
            glfwWaitEventsTimeout( targetFrameTime - frameTime );
        }
    }

    crimild::PrefabNode::cleanup();

    jobScheduler.executeDelayedJobs();
    crimild::MessageQueue::getInstance()->dispatchDeferredMessages();

    jobScheduler.stop();
    crimild::MessageQueue::getInstance()->clear();

    // Destroy the layout manager
    // This will persist the current layout, if any.
    layoutManager = nullptr;

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
