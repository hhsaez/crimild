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

#include "Rendering/VulkanShadowMap.hpp"

#include "Crimild_Foundation.hpp"
#include "Rendering/VulkanDescriptorSet.hpp"
#include "Rendering/VulkanImage.hpp"
#include "Rendering/VulkanImageView.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanSampler.hpp"

#include <array>

using namespace crimild;
using namespace crimild::vulkan;

vulkan::ShadowMap::ShadowMap( RenderDevice *device, std::string name, Light::Type lightType ) noexcept
    : WithRenderDevice( device ),
      Named( name )
{
    const bool lightIsPoint = lightType == Light::Type::POINT;
    const bool lightIsDirectional = lightType == Light::Type::DIRECTIONAL;
    const bool lightIsSpot = lightType == Light::Type::SPOT;

    m_extent = lightIsPoint ? VkExtent3D { 1024, 1024, 1 } : VkExtent3D { 2048, 2048, 1 };
    m_format = lightIsPoint ? VK_FORMAT_R32_SFLOAT : VK_FORMAT_D32_SFLOAT;
    m_layerCount = lightIsPoint ? 6 : ( lightIsDirectional ? DIRECTIONAL_LIGHT_CASCADES : 1 );
    m_aspect = device->formatIsColor( m_format ) ? VK_IMAGE_ASPECT_COLOR_BIT : VK_IMAGE_ASPECT_DEPTH_BIT;
    m_lightSpaceMatrices.resize( m_layerCount, Matrix4f::Constants::IDENTITY );
    m_splits.resize( m_layerCount, 1 );

    m_image = crimild::alloc< vulkan::Image >(
        device,
        [ & ] {
            auto createInfo = vulkan::Image::createInfo();
            createInfo.extent = m_extent;
            createInfo.format = m_format;
            // This image should not be used as an attachment. We'll render into a temp attachment and then
            // copy the resulting image into this one.
            createInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            createInfo.arrayLayers = m_layerCount;
            if ( lightIsPoint ) {
                createInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
            } else if ( lightIsDirectional || lightIsSpot ) {
                // For depth-stencil images, the Vulkan spec requires us to add the attachment use flag
                createInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            }
            return createInfo;
        }(),
        getName() + "/Image"
    );
    m_image->allocateMemory();
    m_image->transitionLayout( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );

    m_imageView = crimild::alloc< vulkan::ImageView >(
        device,
        getName() + "/ImageView",
        m_image,
        [ & ] {
            auto createInfo = vulkan::ImageView::createInfo();
            createInfo.viewType = [ & ] {
                if ( lightIsDirectional ) {
                    return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
                } else if ( lightIsPoint ) {
                    return VK_IMAGE_VIEW_TYPE_CUBE;
                } else {
                    return VK_IMAGE_VIEW_TYPE_2D;
                }
            }();
            createInfo.image = m_image->getHandle();
            createInfo.format = m_format;
            createInfo.subresourceRange.aspectMask = m_aspect;
            createInfo.subresourceRange.layerCount = m_layerCount;
            if ( lightIsPoint ) {
                createInfo.components = { .r = VK_COMPONENT_SWIZZLE_R };
            }
            return createInfo;
        }()
    );

    m_sampler = crimild::alloc< Sampler >(
        device,
        getName() + "/Sampler",
        VK_FILTER_LINEAR,
        VK_SAMPLER_MIPMAP_MODE_LINEAR,
        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
    );

    m_descriptorSet = crimild::alloc< DescriptorSet >(
        device,
        getName() + "/DescriptorSet",
        std::vector< Descriptor > {
            {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .imageView = m_imageView,
                .sampler = m_sampler,
            },
        }
    );
}

//////////////////////////
// Deprecated from Here //
//////////////////////////

vulkan::ShadowMapDEPRECATED::ShadowMapDEPRECATED( RenderDevice *renderDevice, const Light *light ) noexcept
    : WithRenderDevice( renderDevice )
{
    CRIMILD_LOG_TRACE();

    const bool lightIsValid = light != nullptr;
    const bool lightIsPoint = lightIsValid && light->getType() == Light::Type::POINT;
    const bool lightIsDirectional = lightIsValid && light->getType() == Light::Type::DIRECTIONAL;

    const auto IN_FLIGHT_FRAME_COUNT = getRenderDevice()->getInFlightFrameCount();

    const auto EXTENT = [ & ] {
        if ( !lightIsValid ) {
            // This will create a minimum-sized shadow map by default.
            // This way we don't need special checks later.
            return VkExtent3D { 1, 1, 1 };
        } else if ( lightIsPoint ) {
            return VkExtent3D { 1024, 1024, 1 };
        } else {
            return VkExtent3D { 2048, 2048, 1 };
        }
    }();

    // Point light shadows are rendered into the color buffer.
    imageFormat = lightIsPoint ? VK_FORMAT_R32_SFLOAT : VK_FORMAT_D32_SFLOAT;

    images.resize( IN_FLIGHT_FRAME_COUNT );
    imageViews.resize( IN_FLIGHT_FRAME_COUNT );
    descriptorSets.resize( IN_FLIGHT_FRAME_COUNT, VK_NULL_HANDLE );

    // Directional lights use cascade shadow mapping, which requires an image with multiple
    // layers, each of which is used in different render passes.
    imageLayerCount = [ & ]() -> uint32_t {
        if ( lightIsDirectional ) {
            return DIRECTIONAL_LIGHT_CASCADES;
        } else if ( lightIsPoint ) {
            // Cubemaps have 6 sides
            return 6;
        } else {
            return 1;
        }
    }();

    // Point light shadow mpas render to cubemaps, so use floats.
    imageFormat = lightIsPoint ? VK_FORMAT_R32_SFLOAT : VK_FORMAT_D32_SFLOAT;
    imageAspect = renderDevice->formatIsColor( imageFormat ) ? VK_IMAGE_ASPECT_COLOR_BIT : VK_IMAGE_ASPECT_DEPTH_BIT;

    lightSpaceMatrices.resize( imageLayerCount, Matrix4f::Constants::IDENTITY );
    splits.resize( imageLayerCount, 1 );

    // Create images and image views for all frames
    for ( size_t frameId = 0; frameId < IN_FLIGHT_FRAME_COUNT; ++frameId ) {
        images[ frameId ] = [ & ] {
            auto image = crimild::alloc< vulkan::Image >(
                renderDevice,
                [ & ] {
                    auto createInfo = vulkan::initializers::imageCreateInfo();
                    createInfo.extent = EXTENT;
                    createInfo.format = imageFormat;
                    // This image should not be used as an attachment. We'll render into a temp attachment and then
                    // copy the resulting image into this one.
                    createInfo.usage =
                        VK_IMAGE_USAGE_SAMPLED_BIT
                        | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
                    createInfo.arrayLayers = imageLayerCount;
                    if ( lightIsPoint ) {
                        createInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
                    }
                    return createInfo;
                }()
            );
            image->allocateMemory();
            image->setName( "Light/ShadowMap/Image" );

            // Ensures image is in the correct layout
            image->transitionLayout( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
            return image;
        }();

        imageViews[ frameId ] = [ & ] {
            auto imageView = crimild::alloc< vulkan::ImageView >(
                renderDevice,
                "Light/ShadowMap/ImageView",
                images[ frameId ],
                [ & ] {
                    auto createInfo = vulkan::initializers::imageViewCreateInfo(
                        [ & ] {
                            if ( lightIsDirectional ) {
                                return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
                            } else if ( lightIsPoint ) {
                                return VK_IMAGE_VIEW_TYPE_CUBE;
                            } else {
                                return VK_IMAGE_VIEW_TYPE_2D;
                            }
                        }()
                    );
                    createInfo.image = images[ frameId ]->getHandle();
                    createInfo.format = imageFormat;
                    createInfo.subresourceRange.aspectMask = imageAspect;
                    createInfo.subresourceRange.layerCount = imageLayerCount;
                    if ( lightIsPoint ) {
                        createInfo.components = { .r = VK_COMPONENT_SWIZZLE_R };
                    }
                    return createInfo;
                }()
            );
            return imageView;
        }();
    }

    // Sampler
    renderDevice->createSampler( vulkan::Sampler::createInfo(), sampler );
    renderDevice->setObjectName( sampler, "Light/ShadowMap/Sampler" );

    getRenderDevice()->createDescriptorSetLayout(
        {
            VkDescriptorSetLayoutBinding {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr,
            },
        },
        descriptorSetLayout,
        "Light/ShadowMap/DescriptorSetLayout"
    );

    getRenderDevice()->createDescriptorPool(
        {
            VkDescriptorPoolSize {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = static_cast< uint32_t >( IN_FLIGHT_FRAME_COUNT ),
            },
        },
        light != nullptr && light->getType() == Light::Type::DIRECTIONAL ? imageLayerCount : 0,
        descriptorPool,
        "Light/ShadowMap/DescriptorPool"
    );

    std::vector< VkDescriptorSetLayout > layouts( IN_FLIGHT_FRAME_COUNT, descriptorSetLayout );

    const auto allocInfo = VkDescriptorSetAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = static_cast< uint32_t >( layouts.size() ),
        .pSetLayouts = layouts.data(),
    };

    CRIMILD_VULKAN_CHECK(
        vkAllocateDescriptorSets(
            getRenderDevice()->getHandle(),
            &allocInfo,
            descriptorSets.data()
        )
    );

    for ( size_t i = 0; i < descriptorSets.size(); ++i ) {
        const auto imageInfo = VkDescriptorImageInfo {
            .sampler = sampler,
            .imageView = imageViews[ i ]->getHandle(),
            .imageLayout = renderDevice->formatIsColor( imageFormat ) ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
        };

        const auto writes = std::array< VkWriteDescriptorSet, 1 > {
            VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = descriptorSets[ i ],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &imageInfo,
                .pBufferInfo = nullptr,
                .pTexelBufferView = nullptr,
            },
        };

        vkUpdateDescriptorSets(
            getRenderDevice()->getHandle(),
            writes.size(),
            writes.data(),
            0,
            nullptr
        );
    }
}

vulkan::ShadowMapDEPRECATED::~ShadowMapDEPRECATED( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    getRenderDevice()->destroyDescriptorPool( descriptorPool );
    getRenderDevice()->destroyDescriptorSetLayout( descriptorSetLayout );

    // Descritor sets are destroyed when the pool gets destroyed, so
    // just clear the collection
    descriptorSets.clear();

    getRenderDevice()->destroySampler( sampler );

    imageViews.clear();
    images.clear();
}
