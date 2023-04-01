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

#ifndef CRIMILD_EDITOR_IMGUI_UTILS_
#define CRIMILD_EDITOR_IMGUI_UTILS_

// Disable clang-format in order to ensure that all header files are included in the right order

// clang-format off
#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "ImGuizmo.h"
#include "ImGuiFileDialog.h"
#include "ImSequencer.h"
#include "ImCurveEdit.h"
#include "GraphEditor.h"
// clang-format on

#include "Foundation/Named.hpp"
#include "Foundation/SharedObject.hpp"

namespace crimild {

    namespace vulkan {

        class ImageView;
        class Sampler;

    }

    /**
     * Retains ownership of bound resources so they don't get deleted before time.
     */
    class ImGuiVulkanTexture
        : public SharedObject,
          public Named {
    public:
        ImGuiVulkanTexture(
            std::string name,
            std::shared_ptr< vulkan::ImageView > const &imageView
        ) noexcept;

        ImGuiVulkanTexture(
            std::string name,
            std::shared_ptr< vulkan::ImageView > const &imageView,
            std::shared_ptr< vulkan::Sampler > const &sampelr
        ) noexcept;

        ~ImGuiVulkanTexture( void ) = default;

        inline VkDescriptorSet getDescriptorSet( void ) const noexcept { return m_descriptorSet; }

    private:
        std::shared_ptr< vulkan::ImageView > m_imageView;
        std::shared_ptr< vulkan::Sampler > m_sampler;
        VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
    };

}

#endif
