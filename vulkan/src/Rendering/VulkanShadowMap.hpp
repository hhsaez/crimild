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

#ifndef CRIMILD_VULKAN_RENDERING_SHADOW_MAP_
#define CRIMILD_VULKAN_RENDERING_SHADOW_MAP_

#include "Foundation/Named.hpp"
#include "Foundation/SharedObject.hpp"
#include "Foundation/VulkanUtils.hpp"
#include "Mathematics/Matrix4_constants.hpp"
#include "SceneGraph/Light.hpp"

namespace crimild {

    class Light;

    namespace vulkan {

        class DescriptorSet;
        class Image;
        class ImageView;
        class RenderDevice;
        class Sampler;

        class ShadowMap
            : public SharedObject,
              public Named,
              public WithRenderDevice {
        public:
            static constexpr uint32_t DIRECTIONAL_LIGHT_CASCADES = 4;

        public:
            ShadowMap( RenderDevice *device, std::string name, Light::Type lightType ) noexcept;
            virtual ~ShadowMap( void ) = default;

            inline std::shared_ptr< Image > &getImage( void ) noexcept { return m_image; }
            inline std::shared_ptr< ImageView > &getImageView( void ) noexcept { return m_imageView; }
            inline std::shared_ptr< Sampler > &getSampler( void ) noexcept { return m_sampler; }

            inline uint32_t getLayerCount( void ) const noexcept { return m_layerCount; }

            inline const Matrix4f &getLightSpaceMatrix( uint32_t index ) const noexcept { return m_lightSpaceMatrices[ index ]; }
            inline void setLightSpaceMatrix( uint32_t index, const Matrix4f &m ) noexcept { m_lightSpaceMatrices[ index ] = m; }

            inline const float getSplit( uint32_t index ) const noexcept { return m_splits[ index ]; }
            inline void setSplit( uint32_t index, float value ) noexcept { m_splits[ index ] = value; }

        private:
            VkExtent3D m_extent;
            VkFormat m_format;
            VkImageAspectFlagBits m_aspect;
            uint32_t m_layerCount;

            std::shared_ptr< Image > m_image;
            std::shared_ptr< ImageView > m_imageView;
            std::shared_ptr< Sampler > m_sampler;
            std::shared_ptr< DescriptorSet > m_descriptorSet;

            std::vector< Matrix4f > m_lightSpaceMatrices;
            std::vector< float > m_splits;
        };

        //////////////////////////
        // Deprecated from Here //
        //////////////////////////

        class [[deprecated]] ShadowMapDEPRECATED
            : public SharedObject,
              public WithRenderDevice {
        public:
            static constexpr uint32_t DIRECTIONAL_LIGHT_CASCADES = 4;

        public:
            ShadowMapDEPRECATED( RenderDevice *rd, const Light *light ) noexcept;
            virtual ~ShadowMapDEPRECATED( void ) noexcept;

            VkFormat imageFormat = VK_FORMAT_D32_SFLOAT;
            VkImageAspectFlags imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT;

            uint32_t imageLayerCount = 1;

            std::vector< SharedPointer< Image > > images;
            std::vector< SharedPointer< ImageView > > imageViews;

            // Sampler does not change in between frames, so we only need one.
            VkSampler sampler = VK_NULL_HANDLE;

            VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
            VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
            std::vector< VkDescriptorSet > descriptorSets;

            std::vector< Matrix4f > lightSpaceMatrices;
            std::vector< Real > splits;
        };

    }

}

#endif
