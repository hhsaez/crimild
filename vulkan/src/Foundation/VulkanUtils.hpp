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

#ifndef CRIMILD_VULKAN_UTILS_
#define CRIMILD_VULKAN_UTILS_

#include "Exceptions/VulkanException.hpp"
#include "Foundation/Log.hpp"
#include "Foundation/Types.hpp"

#include <vulkan/vulkan.h>

namespace crimild {

	namespace vulkan {

		namespace utils {

			static const VkShaderStageFlagBits VULKAN_SHADER_STAGES[] = {
				VK_SHADER_STAGE_VERTEX_BIT,
				VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
				VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
				VK_SHADER_STAGE_GEOMETRY_BIT,
				VK_SHADER_STAGE_FRAGMENT_BIT,
				VK_SHADER_STAGE_COMPUTE_BIT,
				VK_SHADER_STAGE_ALL_GRAPHICS,
				VK_SHADER_STAGE_ALL,
			};
			
			static const VkPrimitiveTopology VULKAN_PRIMITIVE_TOPOLOGIES[] = {
				VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
				VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
				VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
				VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
				VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
				VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,
				VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY,
				VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY,
				VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY,
				VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY,
				VK_PRIMITIVE_TOPOLOGY_PATCH_LIST,
			};

			static const VkCommandBufferUsageFlagBits VULKAN_COMMAND_BUFFER_USAGE[] = {
				VkCommandBufferUsageFlagBits(),
				VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
				VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
				VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
			};

			const char *errorToString( VkResult result ) noexcept;

            /**
               \brief Used for checking if validation layers should be enabled
             */
            crimild::Bool areValidationLayersEnabled( void ) noexcept;

            using ValidationLayerArray = std::vector< const char * >;

            /**
               \brief The list of validation layers (only valid if they're enabled)
               \see areValidationLayersEnabled()
             */
            const ValidationLayerArray &getValidationLayers( void ) noexcept;

            crimild::Bool checkValidationLayerSupport( const ValidationLayerArray &validationLayers ) noexcept;

            using ExtensionArray = std::vector< const char * >;
            ExtensionArray getRequiredExtensions( void ) noexcept;

            void populateDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT &createInfo ) noexcept;

		}

	}

}

#define CRIMILD_VULKAN_CHECK( x ) {\
	VkResult ret = x; \
	if ( ret != VK_SUCCESS ) {\
	    auto errorStr = crimild::vulkan::utils::errorToString( ret ); \
		std::stringstream ss; \
		ss << "Vulkan Error:" \
		   << "\n\tFile: " << __FILE__ \
		   << "\n\tLine: " << __LINE__ \
		   << "\n\tResult: " << errorStr \
		   << "\n\tCaller: " << #x; \
		throw VulkanException( ss.str() ); \
	}\
}

#endif
