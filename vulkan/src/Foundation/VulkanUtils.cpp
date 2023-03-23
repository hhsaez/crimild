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

#include "VulkanUtils.hpp"

#include "Rendering/IndexBuffer.hpp"
#include "Rendering/ViewportDimensions.hpp"

#include <set>

using namespace crimild::vulkan;

#define CRIMILD_VULKAN_ERROR_STRING( x ) \
    case static_cast< int >( x ):        \
        return #x

const char *utils::errorToString( VkResult result ) noexcept
{
    switch ( result ) {
        CRIMILD_VULKAN_ERROR_STRING( VK_SUCCESS );
        CRIMILD_VULKAN_ERROR_STRING( VK_NOT_READY );
        CRIMILD_VULKAN_ERROR_STRING( VK_TIMEOUT );
        CRIMILD_VULKAN_ERROR_STRING( VK_EVENT_SET );
        CRIMILD_VULKAN_ERROR_STRING( VK_EVENT_RESET );
        CRIMILD_VULKAN_ERROR_STRING( VK_INCOMPLETE );
        CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_OUT_OF_HOST_MEMORY );
        CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_OUT_OF_DEVICE_MEMORY );
        CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_INITIALIZATION_FAILED );
        CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_DEVICE_LOST );
        CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_MEMORY_MAP_FAILED );
        CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_LAYER_NOT_PRESENT );
        CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_EXTENSION_NOT_PRESENT );
        CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_FEATURE_NOT_PRESENT );
        CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_INCOMPATIBLE_DRIVER );
        CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_TOO_MANY_OBJECTS );
        CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_FORMAT_NOT_SUPPORTED );
        CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_SURFACE_LOST_KHR );
        CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_NATIVE_WINDOW_IN_USE_KHR );
        CRIMILD_VULKAN_ERROR_STRING( VK_SUBOPTIMAL_KHR );
        CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_OUT_OF_DATE_KHR );
        CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_OUT_OF_POOL_MEMORY );
        CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_INCOMPATIBLE_DISPLAY_KHR );
        CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_VALIDATION_FAILED_EXT );
        CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_INVALID_SHADER_NV );
        default:
            return "UNKNOWN";
    };
}

VkShaderStageFlagBits utils::getVulkanShaderStageFlag( Shader::Stage stage ) noexcept
{
    switch ( stage ) {
        case Shader::Stage::VERTEX:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case Shader::Stage::TESSELLATION_CONTROL:
            return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case Shader::Stage::TESSELLATION_EVALUATION:
            return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        case Shader::Stage::GEOMETRY:
            return VK_SHADER_STAGE_GEOMETRY_BIT;
        case Shader::Stage::FRAGMENT:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case Shader::Stage::COMPUTE:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        case Shader::Stage::ALL_GRAPHICS:
            return VK_SHADER_STAGE_ALL_GRAPHICS;
        case Shader::Stage::ALL:
            return VK_SHADER_STAGE_ALL;
        default:
            return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    }
}

VkDescriptorType utils::getVulkanDescriptorType( DescriptorType type ) noexcept
{
    switch ( type ) {
        case DescriptorType::UNIFORM_BUFFER:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case DescriptorType::TEXTURE:
        case DescriptorType::DIFFUSE_MAP:
        case DescriptorType::SPECULAR_MAP:
        case DescriptorType::NORMAL_MAP:
        case DescriptorType::ALBEDO_MAP:
        case DescriptorType::METALLIC_MAP:
        case DescriptorType::ROUGHNESS_MAP:
        case DescriptorType::AMBIENT_OCCLUSION_MAP:
        case DescriptorType::COMBINED_ROUGHNESS_METALLIC_MAP:
        case DescriptorType::SHADOW_ATLAS:
        case DescriptorType::REFLECTION_ATLAS:
        case DescriptorType::IRRADIANCE_ATLAS:
        case DescriptorType::PREFILTER_ATLAS:
        case DescriptorType::BRDF_LUT:
            return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case DescriptorType::STORAGE_BUFFER:
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case DescriptorType::STORAGE_IMAGE:
            return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        default:
            return VK_DESCRIPTOR_TYPE_MAX_ENUM;
    }
}

VkFilter utils::getVulkanFilter( Texture::Filter filter ) noexcept
{
    // TODO: This is not complete
    switch ( filter ) {
        case Texture::Filter::NEAREST:
            return VK_FILTER_NEAREST;
        default:
            return VK_FILTER_LINEAR;
    }
}

VkFilter utils::getSamplerFilter( Sampler::Filter filter ) noexcept
{
    // TODO: This is not complete
    switch ( filter ) {
        case Sampler::Filter::NEAREST:
            return VK_FILTER_NEAREST;
        default:
            return VK_FILTER_LINEAR;
    }
}

VkIndexType utils::getIndexType( const IndexBuffer *indexBuffer ) noexcept
{
    switch ( indexBuffer->getFormat() ) {
        case Format::INDEX_16_UINT:
            return VK_INDEX_TYPE_UINT16;
        default:
            return VK_INDEX_TYPE_UINT32;
    }
}

VkCompareOp utils::getCompareOp( const CompareOp &compareOp ) noexcept
{
    switch ( compareOp ) {
        case CompareOp::NEVER:
            return VK_COMPARE_OP_NEVER;
        case CompareOp::LESS:
            return VK_COMPARE_OP_LESS;
        case CompareOp::EQUAL:
            return VK_COMPARE_OP_EQUAL;
        case CompareOp::LESS_OR_EQUAL:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case CompareOp::GREATER:
            return VK_COMPARE_OP_GREATER;
        case CompareOp::NOT_EQUAL:
            return VK_COMPARE_OP_NOT_EQUAL;
        case CompareOp::GREATER_OR_EQUAL:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case CompareOp::ALWAYS:
        default:
            return VK_COMPARE_OP_ALWAYS;
    }
}

VkSamplerAddressMode utils::getSamplerAddressMode( Texture::WrapMode wrapMode ) noexcept
{
    switch ( wrapMode ) {
        case Texture::WrapMode::MIRRORED_REPEAT:
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case Texture::WrapMode::CLAMP_TO_EDGE:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case Texture::WrapMode::CLAMP_TO_BORDER:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        case Texture::WrapMode::REPEAT:
        default:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }
}

VkBorderColor utils::getBorderColor( Texture::BorderColor borderColor ) noexcept
{
    switch ( borderColor ) {
        case Texture::BorderColor::FLOAT_TRANSPARENT_BLACK:
            return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        case Texture::BorderColor::INT_TRANSPARENT_BLACK:
            return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
        case Texture::BorderColor::FLOAT_OPAQUE_BLACK:
            return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        case Texture::BorderColor::INT_OPAQUE_BLACK:
            return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        case Texture::BorderColor::FLOAT_OPAQUE_WHITE:
            return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        case Texture::BorderColor::INT_OPAQUE_WHITE:
            return VK_BORDER_COLOR_INT_OPAQUE_WHITE;
        default:
            return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    }
}

VkSamplerAddressMode utils::getSamplerAddressMode( Sampler::WrapMode wrapMode ) noexcept
{
    switch ( wrapMode ) {
        case Sampler::WrapMode::MIRRORED_REPEAT:
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case Sampler::WrapMode::CLAMP_TO_EDGE:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case Sampler::WrapMode::CLAMP_TO_BORDER:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        case Sampler::WrapMode::REPEAT:
        default:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }
}

VkBorderColor utils::getBorderColor( Sampler::BorderColor borderColor ) noexcept
{
    switch ( borderColor ) {
        case Sampler::BorderColor::FLOAT_TRANSPARENT_BLACK:
            return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        case Sampler::BorderColor::INT_TRANSPARENT_BLACK:
            return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
        case Sampler::BorderColor::FLOAT_OPAQUE_BLACK:
            return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        case Sampler::BorderColor::INT_OPAQUE_BLACK:
            return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        case Sampler::BorderColor::FLOAT_OPAQUE_WHITE:
            return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        case Sampler::BorderColor::INT_OPAQUE_WHITE:
            return VK_BORDER_COLOR_INT_OPAQUE_WHITE;
        default:
            return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    }
}

VkFormat utils::getFormat( Format format ) noexcept
{
    switch ( format ) {
        case Format::R8_UNORM:
            return VK_FORMAT_R8_UNORM;
        case Format::R8_SNORM:
            return VK_FORMAT_R8_SNORM;
        case Format::R8_UINT:
            return VK_FORMAT_R8_UINT;
        case Format::R8_SINT:
            return VK_FORMAT_R8_SINT;
        case Format::R32_UINT:
            return VK_FORMAT_R32_UINT;
        case Format::R32_SINT:
            return VK_FORMAT_R32_SINT;
        case Format::R16_SFLOAT:
            return VK_FORMAT_R16_SFLOAT;
        case Format::R16G16_SFLOAT:
            return VK_FORMAT_R16G16_SFLOAT;
        case Format::R16G16B16_SFLOAT:
            return VK_FORMAT_R16G16B16_SFLOAT;
        case Format::R16G16B16A16_SFLOAT:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
        case Format::R32_SFLOAT:
            return VK_FORMAT_R32_SFLOAT;
        case Format::R64_UINT:
            return VK_FORMAT_R64_UINT;
        case Format::R64_SINT:
            return VK_FORMAT_R64_SINT;
        case Format::R64_SFLOAT:
            return VK_FORMAT_R64_SFLOAT;
        case Format::R8G8B8_UINT:
            return VK_FORMAT_R8G8B8_UINT;
        case Format::R8G8B8A8_UINT:
            return VK_FORMAT_R8G8B8A8_UINT;
        case Format::R8G8B8_UNORM:
            return VK_FORMAT_R8G8B8_UNORM;
        case Format::R8G8B8A8_UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case Format::B8G8R8A8_UNORM:
            return VK_FORMAT_B8G8R8A8_UNORM;
        case Format::R32G32_SFLOAT:
            return VK_FORMAT_R32G32_SFLOAT;
        case Format::R32G32B32_SFLOAT:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case Format::R32G32B32A32_SFLOAT:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case Format::DEPTH_16_UNORM:
            return VK_FORMAT_D16_UNORM;
        case Format::DEPTH_32_SFLOAT:
            return VK_FORMAT_D32_SFLOAT;
        case Format::DEPTH_16_UNORM_STENCIL_8_UINT:
            return VK_FORMAT_D16_UNORM_S8_UINT;
        case Format::DEPTH_24_UNORM_STENCIL_8_UINT:
            return VK_FORMAT_D24_UNORM_S8_UINT;
        case Format::DEPTH_32_SFLOAT_STENCIL_8_UINT:
            return VK_FORMAT_D32_SFLOAT_S8_UINT;
        default:
            return VK_FORMAT_UNDEFINED;
    }
}

crimild::Format utils::getFormat( VkFormat format ) noexcept
{
    switch ( format ) {
        case VK_FORMAT_R8_UNORM:
            return Format::R8_UNORM;
        case VK_FORMAT_R8_SNORM:
            return Format::R8_SNORM;
        case VK_FORMAT_R8_UINT:
            return Format::R8_UINT;
        case VK_FORMAT_R8_SINT:
            return Format::R8_SINT;
        case VK_FORMAT_R32_UINT:
            return Format::R32_UINT;
        case VK_FORMAT_R32_SINT:
            return Format::R32_SINT;
        case VK_FORMAT_R32_SFLOAT:
            return Format::R32_SFLOAT;
        case VK_FORMAT_R64_UINT:
            return Format::R64_UINT;
        case VK_FORMAT_R64_SINT:
            return Format::R64_SINT;
        case VK_FORMAT_R64_SFLOAT:
            return Format::R64_SFLOAT;
        case VK_FORMAT_R8G8B8_UINT:
            return Format::R8G8B8_UINT;
        case VK_FORMAT_R8G8B8A8_UINT:
            return Format::R8G8B8A8_UINT;
        case VK_FORMAT_R8G8B8_UNORM:
            return Format::R8G8B8_UNORM;
        case VK_FORMAT_R8G8B8A8_UNORM:
            return Format::R8G8B8A8_UNORM;
        case VK_FORMAT_B8G8R8A8_UNORM:
            return Format::B8G8R8A8_UNORM;
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return Format::R32G32B32A32_SFLOAT;
        case VK_FORMAT_D16_UNORM:
            return Format::DEPTH_16_UNORM;
        case VK_FORMAT_D32_SFLOAT:
            return Format::DEPTH_32_SFLOAT;
        case VK_FORMAT_D16_UNORM_S8_UINT:
            return Format::DEPTH_16_UNORM_STENCIL_8_UINT;
        case VK_FORMAT_D24_UNORM_S8_UINT:
            return Format::DEPTH_24_UNORM_STENCIL_8_UINT;
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return Format::DEPTH_32_SFLOAT_STENCIL_8_UINT;
        default:
            return Format::UNDEFINED;
    }
}

crimild::Bool utils::formatIsColor( Format format ) noexcept
{
    switch ( format ) {
        case Format::R8_UNORM:
        case Format::R8_SNORM:
        case Format::R8_UINT:
        case Format::R8_SINT:
        case Format::R32_UINT:
        case Format::R32_SINT:
        case Format::R32_SFLOAT:
        case Format::R64_UINT:
        case Format::R64_SINT:
        case Format::R64_SFLOAT:
        case Format::R8G8B8_UINT:
        case Format::R8G8B8A8_UINT:
        case Format::R8G8B8_UNORM:
        case Format::R8G8B8A8_UNORM:
        case Format::B8G8R8A8_UNORM:
        case Format::R32G32B32A32_SFLOAT:
        case Format::COLOR_SWAPCHAIN_OPTIMAL:
            return true;
        default:
            return false;
    }
}

crimild::Bool utils::formatIsDepthStencil( Format format ) noexcept
{
    switch ( format ) {
        case Format::DEPTH_16_UNORM:
        case Format::DEPTH_32_SFLOAT:
        case Format::DEPTH_16_UNORM_STENCIL_8_UINT:
        case Format::DEPTH_24_UNORM_STENCIL_8_UINT:
        case Format::DEPTH_32_SFLOAT_STENCIL_8_UINT:
        case Format::DEPTH_STENCIL_DEVICE_OPTIMAL:
            return true;
        default:
            return false;
    }
}

VkImageUsageFlags utils::getAttachmentUsage( Attachment::Usage usage ) noexcept
{
    switch ( usage ) {
        case Attachment::Usage::COLOR_ATTACHMENT:
            return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        case Attachment::Usage::DEPTH_STENCIL_ATTACHMENT:
            return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

        default:
            return 0;
    }
}

crimild::Extent2D utils::getExtent( const VkExtent2D &extent ) noexcept
{
    return Extent2D {
        .scalingMode = ScalingMode::FIXED,
        .width = Real( extent.width ),
        .height = Real( extent.height ),
    };
}

VkAttachmentLoadOp utils::getLoadOp( Attachment::LoadOp loadOp ) noexcept
{
    switch ( loadOp ) {
        case Attachment::LoadOp::LOAD:
            return VK_ATTACHMENT_LOAD_OP_LOAD;
        case Attachment::LoadOp::CLEAR:
            return VK_ATTACHMENT_LOAD_OP_CLEAR;
        case Attachment::LoadOp::DONT_CARE:
        default:
            return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }
}

VkAttachmentStoreOp utils::getStoreOp( Attachment::StoreOp storeOp ) noexcept
{
    switch ( storeOp ) {
        case Attachment::StoreOp::STORE:
            return VK_ATTACHMENT_STORE_OP_STORE;
        case Attachment::StoreOp::DONT_CARE:
        default:
            return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    }
}

crimild::Bool utils::checkValidationLayersEnabled( void ) noexcept
{
#if defined( CRIMILD_DEBUG )
    return true;
#else
    return false;
#endif
}

const utils::ValidationLayerArray &utils::getValidationLayers( void ) noexcept
{
    static ValidationLayerArray validationLayers = {
        "VK_LAYER_KHRONOS_validation",
    };
    return validationLayers;
}

crimild::Bool utils::checkValidationLayerSupport( const utils::ValidationLayerArray &validationLayers ) noexcept
{
    CRIMILD_LOG_TRACE();

    crimild::UInt32 layerCount;
    vkEnumerateInstanceLayerProperties( &layerCount, nullptr );

    std::vector< VkLayerProperties > availableLayers( layerCount );
    vkEnumerateInstanceLayerProperties( &layerCount, availableLayers.data() );

    for ( const auto &layerName : validationLayers ) {
        auto layerFound = false;
        for ( const auto &layerProperties : availableLayers ) {
            if ( strcmp( layerName, layerProperties.layerName ) == 0 ) {
                CRIMILD_LOG_DEBUG( "Found validation layer: ", layerName );
                layerFound = true;
                break;
            }
        }

        if ( !layerFound ) {
            CRIMILD_LOG_ERROR( "Validation layer not found: ", layerName );
            return false;
        }
    }

    return true;
}

utils::ExtensionArray utils::getRequiredExtensions( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    auto validationLayersEnabled = checkValidationLayersEnabled();

    if ( validationLayersEnabled ) {
        // list all available extensions
        crimild::UInt32 extensionCount = 0;
        vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, nullptr );
        std::vector< VkExtensionProperties > extensions( extensionCount );
        vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, extensions.data() );
        for ( const auto &extension : extensions ) {
            CRIMILD_LOG_DEBUG( "Found extension: ", extension.extensionName );
        }
    }

    // Add extensions to create a presentation surface
    // TODO: It would be great to support "headless" Vulkan in the future. Not
    // only for compute operations, but also for testing. This is definitely something
    // I should try
    auto extensions = ExtensionArray {
        VK_KHR_SURFACE_EXTENSION_NAME,
    };

#if defined( CRIMILD_PLATFORM_OSX )
    // TODO: no macro for platform extensions?
    extensions.push_back( "VK_MVK_macos_surface" );
    extensions.push_back( VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME );
#elif defined( CRIMILD_PLATFORM_WIN32 )
    extensions.push_back( "VK_KHR_win32_surface" );
#endif

    if ( validationLayersEnabled ) {
        extensions.push_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );
        extensions.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
    }

    return extensions;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL crimild_vulkan_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData
)
{
    CRIMILD_LOG_DEBUG( "Vulkan validation layer: ", pCallbackData->pMessage );
    return VK_FALSE;
}

void utils::populateDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT &createInfo ) noexcept
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                 | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                 | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = crimild_vulkan_debug_callback;
    createInfo.pUserData = nullptr;
}

void utils::setObjectName( VkDevice device, UInt64 object, VkDebugReportObjectTypeEXT objectType, const char *name ) noexcept
{
#if defined( CRIMILD_PLATFORM_OSX )
    static auto vkDebugMarkerSetObjectName = ( PFN_vkDebugMarkerSetObjectNameEXT ) vkGetDeviceProcAddr( device, "vkDebugMarkerSetObjectNameEXT" );
    if ( vkDebugMarkerSetObjectName == VK_NULL_HANDLE ) {
        CRIMILD_LOG_WARNING( "Cannot get procedure address for vkDebugMarkerSetObjectName" );
        return;
    }

    VkDebugMarkerObjectNameInfoEXT nameInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT,
        .objectType = objectType,
        .object = object,
        .pObjectName = name,
    };

    vkDebugMarkerSetObjectName( device, &nameInfo );
#endif
}

const utils::ExtensionArray &utils::getDeviceExtensions( void ) noexcept
{
    static ExtensionArray deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,

        // This is required to flip the viewport (see getViewport() above)
        VK_KHR_MAINTENANCE1_EXTENSION_NAME,

#if defined( CRIMILD_PLATFORM_OSX )
        // Require to name objects for debug
        VK_EXT_DEBUG_MARKER_EXTENSION_NAME,

        // Require portability subset
        "VK_KHR_portability_subset",
#endif
    };
    return deviceExtensions;
}

crimild::Bool utils::checkDeviceExtensionSupport( const VkPhysicalDevice &device ) noexcept
{
    CRIMILD_LOG_TRACE();

    const auto &deviceExtensions = getDeviceExtensions();

    crimild::UInt32 extensionCount;
    vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, nullptr );
    std::vector< VkExtensionProperties > availableExtensions( extensionCount );
    vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, availableExtensions.data() );
    for ( const auto &extension : availableExtensions ) {
        CRIMILD_LOG_DEBUG( "Found device extension: ", extension.extensionName );
    }

    std::set< std::string > requiredExtensions( std::begin( deviceExtensions ), std::end( deviceExtensions ) );

    for ( const auto &extension : availableExtensions ) {
        requiredExtensions.erase( extension.extensionName );
    }

    if ( !requiredExtensions.empty() ) {
        std::stringstream ss;
        for ( const auto &name : requiredExtensions ) {
            ss << "\n\t" << name;
        }
        CRIMILD_LOG_ERROR( "Required extensions not met: ", ss.str() );
        return false;
    }

    CRIMILD_LOG_DEBUG( "All required extensions met" );
    return true;
}

utils::QueueFamilyIndices utils::findQueueFamilies( const VkPhysicalDevice &device, const VkSurfaceKHR &surface ) noexcept
{
    CRIMILD_LOG_TRACE();

    QueueFamilyIndices indices;

    if ( device == VK_NULL_HANDLE ) {
        CRIMILD_LOG_WARNING( "Invalid physical device handle" );
        return indices;
    }

    crimild::UInt32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, nullptr );
    if ( queueFamilyCount == 0 ) {
        CRIMILD_LOG_ERROR( "No queue family found for device" );
        return indices;
    }

    std::vector< VkQueueFamilyProperties > queueFamilies( queueFamilyCount );
    vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, queueFamilies.data() );

    crimild::UInt32 i = 0;
    for ( const auto &queueFamily : queueFamilies ) {
        if ( queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT ) {
            indices.graphicsFamily.push_back( i );
        }

        if ( queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT ) {
            indices.computeFamily.push_back( i );
        }

        // Find a queue family that supports presenting to the VUlkan surface
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR( device, i, surface, &presentSupport );
        if ( queueFamily.queueCount > 0 && presentSupport ) {
            // This might probably be same as the graphics queue in most cases
            indices.presentFamily.push_back( i );
        }

        if ( indices.isComplete() ) {
            break;
        }

        ++i;
    }

    return indices;
}

utils::SwapchainSupportDetails utils::querySwapchainSupportDetails( const VkPhysicalDevice &device, const VkSurfaceKHR &surface ) noexcept
{
    CRIMILD_LOG_TRACE();

    SwapchainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        device,
        surface,
        &details.capabilities
    );

    crimild::UInt32 formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        device,
        surface,
        &formatCount,
        nullptr
    );
    if ( formatCount > 0 ) {
        details.formats.resize( formatCount );
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            device,
            surface,
            &formatCount,
            details.formats.data()
        );
    }

    crimild::UInt32 presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device,
        surface,
        &presentModeCount,
        nullptr
    );
    if ( presentModeCount > 0 ) {
        details.presentModes.resize( presentModeCount );
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device,
            surface,
            &presentModeCount,
            details.presentModes.data()
        );
    }

    return details;
}

crimild::Bool utils::checkSwapchainSupport( const VkPhysicalDevice &device, const VkSurfaceKHR &surface ) noexcept
{
    CRIMILD_LOG_TRACE();

    auto swapchainSupport = querySwapchainSupportDetails(
        device,
        surface
    );

    return !swapchainSupport.formats.empty()
           && !swapchainSupport.presentModes.empty();
}

VkSurfaceFormatKHR utils::chooseSurfaceFormat( const std::vector< VkSurfaceFormatKHR > &availableFormats ) noexcept
{
    CRIMILD_LOG_TRACE();

    // If no format is available, force what we need
    if ( availableFormats.size() == 1 && availableFormats[ 0 ].format == VK_FORMAT_UNDEFINED ) {
        return {
            VK_FORMAT_B8G8R8A8_UNORM,
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        };
    }

    // Favor 32-bit RGBA and sRGBA non-linear colorspace
    for ( const auto &availableFormat : availableFormats ) {
        if ( availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ) {
            return availableFormat;
        }
    }

    // If nothing is found matching what we need, return whatever is available
    return availableFormats[ 0 ];
}

VkPresentModeKHR utils::choosePresentationMode( const std::vector< VkPresentModeKHR > &availablePresentModes ) noexcept
{
    CRIMILD_LOG_TRACE();

    // VSync by default, but may introduce latency
    // FIFO mode is always available (defined in standard)
    VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

    for ( const auto &availablePresentMode : availablePresentModes ) {
        if ( availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR ) {
            // VK_PRESENT_MODE_MAILBOX_KHR is the vulkan version of triple buffer
            return availablePresentMode;
        }
    }

    return bestMode;
}

VkExtent2D utils::chooseExtent( const VkSurfaceCapabilitiesKHR &capabilities, VkExtent2D requestedExtent ) noexcept
{
    CRIMILD_LOG_TRACE();

    if ( capabilities.currentExtent.width != std::numeric_limits< uint32_t >::max() ) {
        // Capabilites are enforcing a given extent. Return that one
        return capabilities.currentExtent;
    }

    // Keep width/heigth values within the allowed ones, though.
    requestedExtent.width = std::max(
        capabilities.minImageExtent.width,
        std::min( capabilities.maxImageExtent.width, requestedExtent.width )
    );
    requestedExtent.height = std::max(
        capabilities.minImageExtent.height,
        std::min( capabilities.maxImageExtent.height, requestedExtent.height )
    );

    return requestedExtent;
}

crimild::UInt32 utils::findMemoryType( const VkPhysicalDevice &physicalDevice, crimild::UInt32 typeFilter, VkMemoryPropertyFlags properties ) noexcept
{
    CRIMILD_LOG_TRACE();

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties( physicalDevice, &memProperties );

    for ( crimild::UInt32 i = 0; i < memProperties.memoryTypeCount; ++i ) {
        if ( typeFilter & ( 1 << i )
             && ( memProperties.memoryTypes[ i ].propertyFlags & properties ) == properties ) {
            return i;
        }
    }

    CRIMILD_LOG_ERROR( "Failed to find suitable memory type" );
    return -1;
}

crimild::Bool utils::copyToBuffer( const VkDevice &device, VkDeviceMemory &bufferMemory, const void *data, VkDeviceSize size ) noexcept
{
    void *dstData = nullptr;

    CRIMILD_VULKAN_CHECK(
        vkMapMemory(
            device,
            bufferMemory,
            0,
            size,
            0,
            &dstData
        )
    );

    memcpy( dstData, data, ( size_t ) size );

    vkUnmapMemory( device, bufferMemory );

    return true;
}

crimild::Bool utils::copyToBuffer( const VkDevice &device, VkDeviceMemory &bufferMemory, const void **data, crimild::UInt32 count, VkDeviceSize size ) noexcept
{
    void *dstData = nullptr;

    CRIMILD_VULKAN_CHECK(
        vkMapMemory(
            device,
            bufferMemory,
            0,
            size,
            0,
            &dstData
        )
    );

    for ( auto i = 0l; i < count; i++ ) {
        memcpy(
            static_cast< crimild::UChar * >( dstData ) + i * size,
            data[ i ],
            size
        );
    }

    vkUnmapMemory( device, bufferMemory );

    return true;
}

VkImageType utils::getImageType( crimild::Image *image ) noexcept
{
    switch ( image->type ) {
        case crimild::Image::Type::IMAGE_1D:
            return VK_IMAGE_TYPE_1D;
        case crimild::Image::Type::IMAGE_2D:
            return VK_IMAGE_TYPE_2D;
        case crimild::Image::Type::IMAGE_3D:
            return VK_IMAGE_TYPE_3D;
        default:
            return VK_IMAGE_TYPE_2D;
    }
}

VkImageAspectFlags utils::getImageAspectFlags( crimild::Image *image ) noexcept
{
    VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT;
    if ( utils::formatIsDepthStencil( image->format ) ) {
        aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
        switch ( image->format ) {
            case Format::DEPTH_16_UNORM_STENCIL_8_UINT:
            case Format::DEPTH_24_UNORM_STENCIL_8_UINT:
            case Format::DEPTH_32_SFLOAT_STENCIL_8_UINT:
            case Format::DEPTH_STENCIL_DEVICE_OPTIMAL:
                aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
                break;
            default:
                break;
        }
    }
    return aspect;
}

VkImageViewType utils::getImageViewType( const ImageView *imageView ) noexcept
{
    switch ( imageView->type ) {
        case ImageView::Type::IMAGE_VIEW_1D:
            return VK_IMAGE_VIEW_TYPE_1D;
        case ImageView::Type::IMAGE_VIEW_2D:
            return VK_IMAGE_VIEW_TYPE_2D;
        case ImageView::Type::IMAGE_VIEW_3D:
            return VK_IMAGE_VIEW_TYPE_3D;
        case ImageView::Type::IMAGE_VIEW_CUBE:
            return VK_IMAGE_VIEW_TYPE_CUBE;
        default:
            break;
    }

    // Match image style
    switch ( imageView->image->type ) {
        case crimild::Image::Type::IMAGE_1D:
            return VK_IMAGE_VIEW_TYPE_1D;
        case crimild::Image::Type::IMAGE_2D:
            return VK_IMAGE_VIEW_TYPE_2D;
        case crimild::Image::Type::IMAGE_2D_CUBEMAP:
            return VK_IMAGE_VIEW_TYPE_CUBE;
        case crimild::Image::Type::IMAGE_3D:
            return VK_IMAGE_VIEW_TYPE_3D;
        default:
            return VK_IMAGE_VIEW_TYPE_2D;
    }
}

VkImageAspectFlags utils::getImageViewAspectFlags( const ImageView *imageView ) noexcept
{
    auto format = imageView->format;
    if ( format == Format::UNDEFINED ) {
        format = imageView->image->format;
    }

    if ( utils::formatIsDepthStencil( format ) ) {
        return VK_IMAGE_ASPECT_DEPTH_BIT;
    }
    return VK_IMAGE_ASPECT_COLOR_BIT;
}

void utils::createImageView( VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView *imageView ) noexcept
{
    const auto imageViewInfo = VkImageViewCreateInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .subresourceRange = {
            .aspectMask = aspectFlags,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };

    CRIMILD_VULKAN_CHECK( vkCreateImageView( device, &imageViewInfo, nullptr, imageView ) );
}

VkSampleCountFlagBits utils::getMaxUsableSampleCount( VkPhysicalDevice physicalDevice ) noexcept
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties( physicalDevice, &physicalDeviceProperties );

    auto counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if ( counts & VK_SAMPLE_COUNT_64_BIT )
        return VK_SAMPLE_COUNT_64_BIT;
    if ( counts & VK_SAMPLE_COUNT_32_BIT )
        return VK_SAMPLE_COUNT_32_BIT;
    if ( counts & VK_SAMPLE_COUNT_16_BIT )
        return VK_SAMPLE_COUNT_16_BIT;
    if ( counts & VK_SAMPLE_COUNT_8_BIT )
        return VK_SAMPLE_COUNT_8_BIT;
    if ( counts & VK_SAMPLE_COUNT_4_BIT )
        return VK_SAMPLE_COUNT_4_BIT;
    if ( counts & VK_SAMPLE_COUNT_2_BIT )
        return VK_SAMPLE_COUNT_2_BIT;
    return VK_SAMPLE_COUNT_1_BIT;
}

crimild::Bool utils::hasStencilComponent( VkFormat format ) noexcept
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

std::ostream &operator<<( std::ostream &out, VkImageLayout layout ) noexcept
{
    switch ( layout ) {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            out << "VK_IMAGE_LAYOUT_UNDEFINED";
            break;
        case VK_IMAGE_LAYOUT_GENERAL:
            out << "VK_IMAGE_LAYOUT_GENERAL";
            break;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            out << "VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL";
            break;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            out << "VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL";
            break;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
            out << "VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL";
            break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            out << "VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL";
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            out << "VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL";
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            out << "VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL";
            break;
        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            out << "VK_IMAGE_LAYOUT_PREINITIALIZED";
            break;
        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
            out << "VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL";
            break;
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
            out << "VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL";
            break;
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
            out << "VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL";
            break;
        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:
            out << "VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL";
            break;
        case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:
            out << "VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL";
            break;
        case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL:
            out << "VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL";
            break;
        case VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL:
            out << "VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL";
            break;
        case VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL:
            out << "VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL";
            break;
        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
            out << "VK_IMAGE_LAYOUT_PRESENT_SRC_KHR";
            break;
        default:
            out << "Unknown (" << uint32_t( layout ) << ")";
            break;
    }

    return out;
}
