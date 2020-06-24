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
#include "Rendering/PolygonState.hpp"
#include "Rendering/ViewportDimensions.hpp"
#include "Rendering/VulkanPhysicalDevice.hpp"
#include "Rendering/VulkanRenderDevice.hpp"

#include <set>

using namespace crimild::vulkan;

#define CRIMILD_VULKAN_ERROR_STRING( x ) case static_cast< int >( x ): return #x

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
		CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_INCOMPATIBLE_DISPLAY_KHR );
		CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_VALIDATION_FAILED_EXT );
		CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_INVALID_SHADER_NV );
		CRIMILD_VULKAN_ERROR_STRING( VK_RESULT_BEGIN_RANGE );
		CRIMILD_VULKAN_ERROR_STRING( VK_RESULT_RANGE_SIZE );
		default: return "UNKNOWN";
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
            return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
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

VkPolygonMode utils::getPolygonMode( PolygonState *polygonState ) noexcept
{
    if ( polygonState == nullptr ) {
        return VK_POLYGON_MODE_FILL;
    }

    switch ( polygonState->polygonMode ) {
        case PolygonState::PolygonMode::LINE:
            return VK_POLYGON_MODE_LINE;
        case PolygonState::PolygonMode::POINT:
            return VK_POLYGON_MODE_POINT;
        case PolygonState::PolygonMode::FILL:
        default:
            return VK_POLYGON_MODE_FILL;
    }
}

VkViewport utils::getViewport( const ViewportDimensions *viewport, const RenderDevice *renderDevice ) noexcept
{
    auto x = viewport->dimensions.getX();
    auto y = viewport->dimensions.getY();
    auto w = viewport->dimensions.getWidth();
    auto h = viewport->dimensions.getHeight();
    auto minD = viewport->depthRange.x();
    auto maxD = viewport->depthRange.y();

    if ( viewport->scalingMode == ScalingMode::SWAPCHAIN_RELATIVE ) {
        if ( renderDevice != nullptr ) {
            if ( auto swapchain = renderDevice->getSwapchain() ) {
                // Scale viewport if needed
                x *= swapchain->extent.width;
                y *= swapchain->extent.height;
                w *= swapchain->extent.width;
                h *= swapchain->extent.height;
            }
        }
    }

    // Because Vulkan's coordinate system is different from Crimild's one,
    // we need to specify the viewport in a different way than usual.
    // WARNING: This trick requires VK_KHR_maintenance1 support (which should
    // be part of the core spec at the time of this writing).
    // See: https://www.saschawillems.de/blog/2019/03/29/flipping-the-vulkan-viewport/
    //
    // Hernan: I tried the trick specified here:
    // https://matthewwellings.com/blog/the-new-vulkan-coordinate-system/
    // but didn't really worked for me. On one hand, things like computing
    // reflection or refraction required me to reverse the resulting vector
    // which is very error prone. On the other, the view is zoomed with respect
    // to other platforms like OpenGL (this might be a bug, though).
    // Also, don't forget to reverse face culling (see createRasterizer below)

    return VkViewport {
		.x = x,
        .y = h + y,
        .width = w,
        .height = -h,
        .minDepth = minD,
        .maxDepth = maxD,
    };
}

VkRect2D utils::getViewportRect( const ViewportDimensions *viewport, const RenderDevice *renderDevice ) noexcept
{
    auto x = viewport->dimensions.getX();
    auto y = viewport->dimensions.getY();
    auto w = viewport->dimensions.getWidth();
    auto h = viewport->dimensions.getHeight();

    if ( viewport->scalingMode == ScalingMode::SWAPCHAIN_RELATIVE ) {
        if ( renderDevice != nullptr ) {
            if ( auto swapchain = renderDevice->getSwapchain() ) {
                // Scale viewport if needed
                x *= swapchain->extent.width;
                y *= swapchain->extent.height;
                w *= swapchain->extent.width;
                h *= swapchain->extent.height;
            }
        }
    }

    return VkRect2D {
        .offset = {
            static_cast< crimild::Int32 >( x ),
            static_cast< crimild::Int32 >( y ),
        },
        .extent = VkExtent2D {
            static_cast< crimild::UInt32 >( w ),
            static_cast< crimild::UInt32 >( h ),
        },
    };
}

VkRect2D utils::getScissor( const ViewportDimensions *viewport, const RenderDevice *renderDevice ) noexcept
{
    return getViewportRect( viewport, renderDevice );
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

VkFormat utils::getFormat( RenderDevice *renderDevice, Format format ) noexcept
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
        case Format::COLOR_SWAPCHAIN_OPTIMAL:
            return renderDevice->getSwapchain()->format;
        case Format::DEPTH_STENCIL_DEVICE_OPTIMAL:
            return utils::findDepthFormat( renderDevice );
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

/*
VkImageLayout utils::getImageLayout( Image::Layout layout ) noexcept
{
    switch ( layout ) {
        case Image::Layout::GENERAL:
            return VK_IMAGE_LAYOUT_GENERAL;
        case Image::Layout::COLOR_ATTACHMENT_OPTIMAL:
            return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case Image::Layout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        case Image::Layout::DEPTH_STENCIL_READ_ONLY_OPTIMAL:
            return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
        case Image::Layout::SHADER_READ_ONLY_OPTIMAL:
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        case Image::Layout::TRANSFER_SRC_OPTIMAL:
            return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        case Image::Layout::TRANSFER_DST_OPTIMAL:
            return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        case Image::Layout::PREINITIALIZED:
            return VK_IMAGE_LAYOUT_PREINITIALIZED;
        case Image::Layout::DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
            return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;
        case Image::Layout::DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
            return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL;
        case Image::Layout::PRESENT_SRC:
            return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        case Image::Layout::SHARED_PRESENT:
            return VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR;
        default:
            return VK_IMAGE_LAYOUT_UNDEFINED;
    }
}
*/

VkExtent2D utils::getExtent( Extent2D extent, const RenderDevice *renderDevice ) noexcept
{
    auto width = extent.width;
    auto height = extent.height;
    if ( extent.scalingMode == ScalingMode::SWAPCHAIN_RELATIVE ) {
        auto swapchain = renderDevice->getSwapchain();
        width *= swapchain->extent.width;
        height *= swapchain->extent.height;
    }

    return VkExtent2D {
        .width = crimild::UInt32( width ),
        .height = crimild::UInt32( height ),
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
        "VK_LAYER_LUNARG_standard_validation",
    };
    return validationLayers;
}

crimild::Bool utils::checkValidationLayerSupport( const utils::ValidationLayerArray &validationLayers ) noexcept
{
    CRIMILD_LOG_TRACE( "Checking validation layer support" );

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
    CRIMILD_LOG_TRACE( "Getting required extensions" );

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
#endif

    if ( validationLayersEnabled ) {
        extensions.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
    }

    return extensions;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL crimild_vulkan_debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagsEXT type,
	const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
	void *pUserData )
{
    std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
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

const utils::ExtensionArray &utils::getDeviceExtensions( void ) noexcept
{
    static ExtensionArray deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,

        // This is required to flip the viewport (see getViewport() above)
        VK_KHR_MAINTENANCE1_EXTENSION_NAME,
    };
    return deviceExtensions;
}

crimild::Bool utils::checkDeviceExtensionSupport( const VkPhysicalDevice &device ) noexcept
{
    CRIMILD_LOG_TRACE( "Checking device extension support" );

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
    CRIMILD_LOG_TRACE( "Finding device queue families" );

    QueueFamilyIndices indices;

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
    CRIMILD_LOG_TRACE( "Query swapchain support details" );

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
    CRIMILD_LOG_TRACE( "Check swapchain support" );

    auto swapchainSupport = querySwapchainSupportDetails(
        device,
        surface
    );

    return !swapchainSupport.formats.empty()
    	&& !swapchainSupport.presentModes.empty();
}

crimild::UInt32 utils::findMemoryType( const VkPhysicalDevice &physicalDevice, crimild::UInt32 typeFilter, VkMemoryPropertyFlags properties ) noexcept
{
    CRIMILD_LOG_TRACE( "Finding memory type device" );

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

crimild::Bool utils::createBuffer( RenderDevice *renderDevice, BufferDescriptor const &descriptor, VkBuffer &bufferHandler, VkDeviceMemory &bufferMemory ) noexcept
{
    auto createInfo = VkBufferCreateInfo {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = descriptor.size,
        .usage = descriptor.usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    CRIMILD_VULKAN_CHECK(
         vkCreateBuffer(
            renderDevice->handler,
            &createInfo,
            nullptr,
            &bufferHandler
           )
     );

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements( renderDevice->handler, bufferHandler, &memRequirements );

    auto allocInfo = VkMemoryAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = findMemoryType(
            renderDevice->physicalDevice->handler,
            memRequirements.memoryTypeBits,
            descriptor.properties
        ),
    };

    CRIMILD_VULKAN_CHECK(
        vkAllocateMemory(
             renderDevice->handler,
             &allocInfo,
             nullptr,
             &bufferMemory
         )
     );

    CRIMILD_VULKAN_CHECK(
        vkBindBufferMemory(
               renderDevice->handler,
               bufferHandler,
               bufferMemory,
               0
           )
       );

    return true;
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

crimild::Bool utils::copyBuffer( RenderDevice *renderDevice, VkBuffer src, VkBuffer dst, VkDeviceSize size ) noexcept
{
    auto commandBuffer = beginSingleTimeCommands( renderDevice );

    auto copyRegion = VkBufferCopy {
        .size = size
    };

    vkCmdCopyBuffer( commandBuffer, src, dst, 1, &copyRegion );

    endSingleTimeCommands( renderDevice, commandBuffer );

    return true;
}

crimild::Bool utils::createImage( RenderDevice *renderDevice, ImageDescriptor const &descriptor, VkImage &image, VkDeviceMemory &imageMemory ) noexcept
{
    auto createInfo = VkImageCreateInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .extent = {
            .width = descriptor.width,
            .height = descriptor.height,
            .depth = 1,
        },
        .mipLevels = descriptor.mipLevels,
        .arrayLayers = descriptor.arrayLayers,
        .format = descriptor.format,
        .tiling = descriptor.tiling,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .usage = descriptor.usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .samples = descriptor.numSamples,
        .flags = descriptor.flags,
    };

    CRIMILD_VULKAN_CHECK(
        vkCreateImage(
            renderDevice->handler,
            &createInfo,
            nullptr,
            &image
          )
     );

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements( renderDevice->handler, image, &memRequirements );

    auto allocInfo = VkMemoryAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = utils::findMemoryType(
            renderDevice->physicalDevice->handler,
            memRequirements.memoryTypeBits,
            VK_MEMORY_HEAP_DEVICE_LOCAL_BIT
        ),
    };

    CRIMILD_VULKAN_CHECK(
        vkAllocateMemory(
            renderDevice->handler,
            &allocInfo,
            nullptr,
            &imageMemory
        )
    );

    CRIMILD_VULKAN_CHECK(
        vkBindImageMemory(
            renderDevice->handler,
            image,
            imageMemory,
            0
        )
    );

    return true;
}

void utils::transitionImageLayout( RenderDevice *renderDevice, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, crimild::UInt32 mipLevels, crimild::UInt32 layerCount ) noexcept
{
    auto commandBuffer = beginSingleTimeCommands( renderDevice );

    auto barrier = VkImageMemoryBarrier {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange.aspectMask = 0, // Defined below
        .subresourceRange.baseMipLevel = 0,
        .subresourceRange.levelCount = mipLevels,
        .subresourceRange.baseArrayLayer = 0,
        .subresourceRange.layerCount = layerCount,
        .srcAccessMask = 0, // See below
        .dstAccessMask = 0, // See below
    };

    VkPipelineStageFlags sourceStage = 0;
    VkPipelineStageFlags destinationStage = 0;

    if ( newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if ( hasStencilComponent( format ) ) {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    } else {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    if ( oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if ( oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if ( oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else {
        CRIMILD_LOG_ERROR( "Unsupported Vulkan Layout Transition" );
        CRIMILD_VULKAN_CHECK( VK_ERROR_FORMAT_NOT_SUPPORTED );
    }

    vkCmdPipelineBarrier(
    	commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1,
        &barrier
    );

    endSingleTimeCommands( renderDevice, commandBuffer );
}

void utils::copyBufferToImage( RenderDevice *renderDevice, VkBuffer buffer, VkImage image, crimild::UInt32 width, crimild::UInt32 height ) noexcept
{
    auto commandBuffer = beginSingleTimeCommands( renderDevice );

    auto region = VkBufferImageCopy {
		.bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .imageSubresource.mipLevel = 0,
        .imageSubresource.baseArrayLayer = 0,
        .imageSubresource.layerCount = 1,
        .imageOffset = { 0, 0 },
        .imageExtent = {
            .width = width,
            .height = height,
            .depth = 1,
        },
    };

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    endSingleTimeCommands( renderDevice, commandBuffer );
}

void utils::copyBufferToLayeredImage( RenderDevice *renderDevice, VkBuffer buffer, VkImage image, crimild::Size layerCount, crimild::Size layerSize, crimild::UInt32 layerWidth, crimild::UInt32 layerHeight ) noexcept
{
    auto commandBuffer = beginSingleTimeCommands( renderDevice );

    std::vector< VkBufferImageCopy > bufferCopyRegions( layerCount );
    for ( auto i = 0l; i < layerCount; i++ ) {
        bufferCopyRegions[ i ] = {
			.bufferOffset = i * layerSize,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .imageSubresource.mipLevel = 0,
            .imageSubresource.baseArrayLayer = static_cast< crimild::UInt32 >( i ),
            .imageSubresource.layerCount = 1,
            .imageOffset = { 0, 0 },
            .imageExtent = {
                .width = layerWidth,
                .height = layerHeight,
                .depth = 1,
            },
        };
    }

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        static_cast< crimild::UInt32 >( bufferCopyRegions.size() ),
        bufferCopyRegions.data()
    );

    endSingleTimeCommands( renderDevice, commandBuffer );
}

void utils::generateMipmaps( RenderDevice *renderDevice, VkImage image, VkFormat imageFormat, crimild::Int32 width, crimild::Int32 height, crimild::UInt32 mipLevels ) noexcept
{
    // Check if image format supports linear blitting
    auto physicalDevice = renderDevice->physicalDevice->handler;
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties( physicalDevice, imageFormat, &formatProperties );
    if ( !( formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT ) ) {
        CRIMILD_LOG_FATAL( "Texture image format does not support linear blitting!" );
        exit( -1 );
    }

    auto commandBuffer = beginSingleTimeCommands( renderDevice );

    auto barrier = VkImageMemoryBarrier {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .image = image,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .subresourceRange.baseArrayLayer = 0,
        .subresourceRange.layerCount = 1,
		.subresourceRange.levelCount = 1,
    };

    auto mipWidth = width;
    auto mipHeight = height;

    for ( crimild::UInt32 i = 1; i < mipLevels; i++ ) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier
        );

        auto blit = VkImageBlit {
            .srcOffsets[ 0 ] = { 0, 0, 0 },
            .srcOffsets[ 1 ] = { mipWidth, mipHeight, 1 },
            .srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .srcSubresource.mipLevel = i - 1,
            .srcSubresource.baseArrayLayer = 0,
            .srcSubresource.layerCount = 1,
            .dstOffsets[ 0 ] = { 0, 0, 0 },
            .dstOffsets[ 1 ] = {
                mipWidth > 1 ? mipWidth / 2 : 1,
                mipHeight > 1 ? mipHeight / 2 : 1,
                1
            },
            .dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .dstSubresource.mipLevel = i,
            .dstSubresource.baseArrayLayer = 0,
            .dstSubresource.layerCount = 1,
        };

        vkCmdBlitImage(
            commandBuffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &blit,
            VK_FILTER_LINEAR
        );

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier
        );

        if ( mipWidth > 1 ) mipWidth /= 2;
        if ( mipHeight > 1 ) mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier
    );

    endSingleTimeCommands( renderDevice, commandBuffer );
}

VkImageType utils::getImageType( Image *image ) noexcept
{
	switch ( image->type ) {
		case Image::Type::IMAGE_1D:
			return VK_IMAGE_TYPE_1D;
		case Image::Type::IMAGE_2D:
			return VK_IMAGE_TYPE_2D;
		case Image::Type::IMAGE_3D:
			return VK_IMAGE_TYPE_3D;
		default:
			return VK_IMAGE_TYPE_2D;		
	}
}

VkImageAspectFlags utils::getImageAspectFlags( Image *image ) noexcept
{
	if ( utils::formatIsDepthStencil( image->format ) ) {
		return VK_IMAGE_ASPECT_DEPTH_BIT;
	}
	return VK_IMAGE_ASPECT_COLOR_BIT;
}

VkImageViewType utils::getImageViewType( ImageView *imageView ) noexcept
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
		case Image::Type::IMAGE_1D:
			return VK_IMAGE_VIEW_TYPE_1D;
		case Image::Type::IMAGE_2D:
			return VK_IMAGE_VIEW_TYPE_2D;
		case Image::Type::IMAGE_3D:
			return VK_IMAGE_VIEW_TYPE_3D;
		default:
			return VK_IMAGE_VIEW_TYPE_2D;		
	}
}

VkFormat utils::getImageViewFormat( RenderDevice *renderDevice, ImageView *imageView ) noexcept
{
	auto format = imageView->format;
	if ( format == Format::UNDEFINED ) {
		format = imageView->image->format;
	}
	return getFormat( renderDevice, format );
}

VkImageAspectFlags utils::getImageViewAspectFlags( ImageView *imageView ) noexcept
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

VkSampleCountFlagBits utils::getMaxUsableSampleCount( VkPhysicalDevice physicalDevice ) noexcept
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties( physicalDevice, &physicalDeviceProperties );

    auto counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if ( counts & VK_SAMPLE_COUNT_64_BIT ) return VK_SAMPLE_COUNT_64_BIT;
    if ( counts & VK_SAMPLE_COUNT_32_BIT ) return VK_SAMPLE_COUNT_32_BIT;
    if ( counts & VK_SAMPLE_COUNT_16_BIT ) return VK_SAMPLE_COUNT_16_BIT;
    if ( counts & VK_SAMPLE_COUNT_8_BIT ) return VK_SAMPLE_COUNT_8_BIT;
    if ( counts & VK_SAMPLE_COUNT_4_BIT ) return VK_SAMPLE_COUNT_4_BIT;
    if ( counts & VK_SAMPLE_COUNT_2_BIT ) return VK_SAMPLE_COUNT_2_BIT;
    return VK_SAMPLE_COUNT_1_BIT;
}

VkFormat utils::findSupportedFormat( RenderDevice *renderDevice, const std::vector< VkFormat > &candidates, VkImageTiling tiling, VkFormatFeatureFlags features ) noexcept
{
    auto physicalDevice = renderDevice->physicalDevice->handler;

    for ( auto format : candidates ) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties( physicalDevice, format, &props );

        if ( tiling == VK_IMAGE_TILING_LINEAR && ( props.linearTilingFeatures & features ) == features ) {
            return format;
        }
        else if ( tiling == VK_IMAGE_TILING_OPTIMAL && ( props.optimalTilingFeatures & features ) == features ) {
            return format;
        }
    }

    CRIMILD_LOG_ERROR( "Failed to find supported format!" );
    return VK_FORMAT_UNDEFINED;
}

VkFormat utils::findDepthFormat( RenderDevice *renderDevice ) noexcept
{
    return findSupportedFormat(
        renderDevice,
    	{
        	VK_FORMAT_D32_SFLOAT,
        	VK_FORMAT_D32_SFLOAT_S8_UINT,
        	VK_FORMAT_D24_UNORM_S8_UINT
    	},
       	VK_IMAGE_TILING_OPTIMAL,
       	VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

crimild::Bool utils::hasStencilComponent( VkFormat format ) noexcept
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkCommandBuffer utils::beginSingleTimeCommands( RenderDevice *renderDevice ) noexcept
{
    auto commandPool = renderDevice->getCommandPool();

    auto allocInfo = VkCommandBufferAllocateInfo {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool = commandPool->handler,
        .commandBufferCount = 1,
    };

    VkCommandBuffer commandBuffer;

    CRIMILD_VULKAN_CHECK(
		vkAllocateCommandBuffers(
            renderDevice->handler,
            &allocInfo,
            &commandBuffer
        )
    );

    auto beginInfo = VkCommandBufferBeginInfo {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    vkBeginCommandBuffer( commandBuffer, &beginInfo );

    return commandBuffer;
}

void utils::endSingleTimeCommands( RenderDevice *renderDevice, VkCommandBuffer commandBuffer ) noexcept
{
    vkEndCommandBuffer( commandBuffer );

    auto submitInfo = VkSubmitInfo {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
    };

    auto graphicsQueue = renderDevice->graphicsQueue;
    auto commandPool = renderDevice->getCommandPool();

    CRIMILD_VULKAN_CHECK(
    	vkQueueSubmit(
            graphicsQueue,
            1,
            &submitInfo,
            nullptr
        )
    );

    CRIMILD_VULKAN_CHECK(
    	vkQueueWaitIdle(
			graphicsQueue
        )
    );

    vkFreeCommandBuffers(
        renderDevice->handler,
        commandPool->handler,
        1,
        &commandBuffer
    );
}

