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
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Rendering/RenderPasses/VulkanSoftRTPass.hpp"

#include "Mathematics/Box_normal.hpp"
#include "Mathematics/ColorRGBOps.hpp"
#include "Mathematics/ColorRGB_isZero.hpp"
#include "Mathematics/Cylinder_normal.hpp"
#include "Mathematics/Point3Ops.hpp"
#include "Mathematics/Random.hpp"
#include "Mathematics/Sphere_normal.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Vector2Ops.hpp"
#include "Mathematics/Vector3_isZero.hpp"
#include "Mathematics/intersect.hpp"
#include "Mathematics/isNaN.hpp"
#include "Mathematics/reflect.hpp"
#include "Mathematics/refract.hpp"
#include "Mathematics/swizzle.hpp"
#include "Mathematics/whichSide.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/VulkanGraphicsPipeline.hpp"
#include "Rendering/VulkanImage.hpp"
#include "Rendering/VulkanImageView.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/BinTreeScene.hpp"
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild;
using namespace crimild::vulkan;

SoftRTPass::SoftRTPass( RenderDevice *renderDevice ) noexcept
    : RenderPassBase( renderDevice )
{
    m_renderArea = VkRect2D {
        .offset = {
            0,
            0,
        },
        .extent = {
            .width = 320,
            .height = 200,
        },
    };

    init();
}

SoftRTPass::~SoftRTPass( void ) noexcept
{
    clear();
}

Event SoftRTPass::handle( const Event &e ) noexcept
{
    switch ( e.type ) {
        case Event::Type::WINDOW_RESIZE: {
            // m_renderArea.extent = {
            //     .width = uint32_t( e.extent.width ),
            //     .height = uint32_t( e.extent.height ),
            // };
            clear();
            init();
            break;
        }

        default:
            break;
    }

    return e;
}

void SoftRTPass::render( Node *scene, Camera *camera ) noexcept
{
    const auto currentFrameIndex = getRenderDevice()->getCurrentFrameIndex();
    auto commandBuffer = getRenderDevice()->getCurrentCommandBuffer();

    renderScene( scene, camera );
    updateImage( currentFrameIndex );

    const auto clearValues = std::array< VkClearValue, 1 > {
        VkClearValue {
            .color = {
                .float32 = {
                    0.0f,
                    0.0f,
                    0.0f,
                    1.0f,
                },
            },
        },
    };

    auto renderPassInfo = VkRenderPassBeginInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = m_renderPass,
        .framebuffer = m_framebuffers[ currentFrameIndex ],
        .renderArea = m_renderArea,
        .clearValueCount = clearValues.size(),
        .pClearValues = clearValues.data(),
    };

    vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );

    vkCmdBindPipeline(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline->getHandle()
    );

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline->getPipelineLayout(),
        0,
        1,
        &m_renderPassObjects.descriptorSets[ currentFrameIndex ],
        0,
        nullptr
    );

    vkCmdDraw( commandBuffer, 6, 1, 0, 0 );

    vkCmdEndRenderPass( commandBuffer );

    getRenderDevice()->transitionImageLayout(
        commandBuffer,
        *m_colorAttachment.images[ currentFrameIndex ],
        m_colorAttachment.format,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        m_colorAttachment.mipLevels,
        m_colorAttachment.layerCount
    );
}

void SoftRTPass::init( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    const auto extent = m_renderArea.extent;

    getRenderDevice()->createFramebufferAttachment( "SoftRT", extent, VK_FORMAT_R32G32B32A32_SFLOAT, m_colorAttachment );

    auto attachments = std::array< VkAttachmentDescription, 1 > {
        VkAttachmentDescription {
            .format = m_colorAttachment.format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        }
    };

    auto colorReferences = std::array< VkAttachmentReference, 1 > {
        VkAttachmentReference {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        },
    };

    auto subpass = VkSubpassDescription {
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = crimild::UInt32( colorReferences.size() ),
        .pColorAttachments = colorReferences.data(),
        .pResolveAttachments = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = nullptr,
    };

    auto dependencies = std::array< VkSubpassDependency, 2 > {
        VkSubpassDependency {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        },
        VkSubpassDependency {
            .srcSubpass = 0,
            .dstSubpass = VK_SUBPASS_EXTERNAL,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        }
    };

    auto createInfo = VkRenderPassCreateInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = static_cast< crimild::UInt32 >( attachments.size() ),
        .pAttachments = attachments.data(),
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = crimild::UInt32( dependencies.size() ),
        .pDependencies = dependencies.data(),
    };

    CRIMILD_VULKAN_CHECK(
        vkCreateRenderPass(
            getRenderDevice()->getHandle(),
            &createInfo,
            nullptr,
            &m_renderPass
        )
    );

    m_framebuffers.resize( getRenderDevice()->getSwapchainImageViews().size() );
    for ( uint8_t i = 0; i < m_framebuffers.size(); ++i ) {
        auto attachments = std::array< VkImageView, 1 > {
            *m_colorAttachment.imageViews[ i ],
        };

        auto createInfo = VkFramebufferCreateInfo {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .renderPass = m_renderPass,
            .attachmentCount = uint32_t( attachments.size() ),
            .pAttachments = attachments.data(),
            .width = extent.width,
            .height = extent.height,
            .layers = 1,
        };

        CRIMILD_VULKAN_CHECK(
            vkCreateFramebuffer(
                getRenderDevice()->getHandle(),
                &createInfo,
                nullptr,
                &m_framebuffers[ i ]
            )
        );
    }

    createImageResources();
    createRenderPassObjects();

    m_pipeline = [ & ] {
        auto program = crimild::alloc< ShaderProgram >();
        program->setShaders(
            Array< SharedPointer< Shader > > {
                crimild::alloc< Shader >(
                    Shader::Stage::VERTEX,
                    R"(
                        layout ( location = 0 ) out vec2 outUV;

                        void main()
                        {
                            outUV = vec2( ( gl_VertexIndex << 1 ) & 2, gl_VertexIndex & 2 );
                            gl_Position = vec4( outUV * 2.0f - 1.0f, 0.0f, 1.0f );
                            outUV.y = 1 - outUV.y;
                        }

                    )"
                ),
                crimild::alloc< Shader >(
                    Shader::Stage::FRAGMENT,
                    R"(
                        layout ( location = 0 ) in vec2 inUV;

                        layout ( set = 0, binding = 0 ) uniform sampler2D samplerColor;

                        layout ( location = 0 ) out vec4 outFragColor;

                        void main()
                        {
                            vec4 color = texture( samplerColor, inUV );
                            // if (color.a < 0.01 ) {
                            //     discard;
                            // }
                            outFragColor = color;
                            // outFragColor = vec4( inUV, 0.0, 1.0 );
                        }
                    )"
                ),
            }
        );

        const auto viewport = ViewportDimensions::fromExtent( m_renderArea.extent.width, m_renderArea.extent.height );

        return std::make_unique< GraphicsPipeline >(
            getRenderDevice(),
            m_renderPass,
            GraphicsPipeline::Descriptor {
                .descriptorSetLayouts = std::vector< VkDescriptorSetLayout > {
                    m_renderPassObjects.layout,
                },
                .program = program.get(),
                std::vector< VertexLayout > {},
                .colorAttachmentCount = 1,
                .viewport = viewport,
                .scissor = viewport,
            }
        );
    }();
}

void SoftRTPass::clear( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDeviceWaitIdle( getRenderDevice()->getHandle() );

    m_pipeline = nullptr;

    destroyRenderPassObjects();
    destroyImageResources();

    for ( auto &fb : m_framebuffers ) {
        vkDestroyFramebuffer( getRenderDevice()->getHandle(), fb, nullptr );
    }
    m_framebuffers.clear();

    getRenderDevice()->destroyFramebufferAttachment( m_colorAttachment );

    vkDestroyRenderPass( getRenderDevice()->getHandle(), m_renderPass, nullptr );
    m_renderPass = VK_NULL_HANDLE;
}

void SoftRTPass::createRenderPassObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    const auto poolSizes = std::array< VkDescriptorPoolSize, 1 > {
        VkDescriptorPoolSize {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
        },
    };

    auto poolCreateInfo = VkDescriptorPoolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = uint32_t( poolSizes.size() ),
        .pPoolSizes = poolSizes.data(),
        .maxSets = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
    };

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorPool( getRenderDevice()->getHandle(), &poolCreateInfo, nullptr, &m_renderPassObjects.pool ) );

    const auto bindings = std::array< VkDescriptorSetLayoutBinding, 1 > {
        VkDescriptorSetLayoutBinding {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
        },
    };

    auto layoutCreateInfo = VkDescriptorSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = uint32_t( bindings.size() ),
        .pBindings = bindings.data(),
    };

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorSetLayout( getRenderDevice()->getHandle(), &layoutCreateInfo, nullptr, &m_renderPassObjects.layout ) );

    std::vector< VkDescriptorSetLayout > layouts( getRenderDevice()->getSwapchainImageCount(), m_renderPassObjects.layout );

    const auto allocInfo = VkDescriptorSetAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = m_renderPassObjects.pool,
        .descriptorSetCount = uint32_t( layouts.size() ),
        .pSetLayouts = layouts.data(),
    };

    m_renderPassObjects.descriptorSets.resize( getRenderDevice()->getSwapchainImageCount() );
    CRIMILD_VULKAN_CHECK( vkAllocateDescriptorSets( getRenderDevice()->getHandle(), &allocInfo, m_renderPassObjects.descriptorSets.data() ) );

    for ( size_t i = 0; i < m_renderPassObjects.descriptorSets.size(); ++i ) {
        const auto imageInfo = VkDescriptorImageInfo {
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .imageView = m_imageResources[ i ].imageView,
            .sampler = m_imageResources[ i ].sampler,
        };

        const auto writes = std::array< VkWriteDescriptorSet, 1 > {
            VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = m_renderPassObjects.descriptorSets[ i ],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .pBufferInfo = nullptr,
                .pImageInfo = &imageInfo,
                .pTexelBufferView = nullptr,
            },
        };
        vkUpdateDescriptorSets( getRenderDevice()->getHandle(), writes.size(), writes.data(), 0, nullptr );
    }
}

void SoftRTPass::destroyRenderPassObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDestroyDescriptorSetLayout( getRenderDevice()->getHandle(), m_renderPassObjects.layout, nullptr );
    m_renderPassObjects.layout = VK_NULL_HANDLE;

    vkDestroyDescriptorPool( getRenderDevice()->getHandle(), m_renderPassObjects.pool, nullptr );
    m_renderPassObjects.pool = VK_NULL_HANDLE;
}

void SoftRTPass::createImageResources( void ) noexcept
{
    m_imageFormat = VK_FORMAT_R32G32B32A32_SFLOAT;

    m_imageData.resize( m_renderArea.extent.width * m_renderArea.extent.height * sizeof( ColorRGBA ) );

    auto samplerInfo = VkSamplerCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .mipLodBias = 0,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy = 1,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .minLod = 0,
        .maxLod = 1,
        .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
        .unnormalizedCoordinates = VK_FALSE,
    };

    m_imageResources.resize( getRenderDevice()->getSwapchainImageCount() );
    for ( size_t i = 0; i < m_imageResources.size(); ++i ) {
        getRenderDevice()->createImage(
            m_renderArea.extent.width,
            m_renderArea.extent.height,
            m_imageFormat,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_HEAP_DEVICE_LOCAL_BIT,
            1,
            VK_SAMPLE_COUNT_1_BIT,
            1,
            0,
            m_imageResources[ i ].image,
            m_imageResources[ i ].memory
        );
        getRenderDevice()->setObjectName( m_imageResources[ i ].image, "SoftRT/Image" );

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements( getRenderDevice()->getHandle(), m_imageResources[ i ].image, &memRequirements );

        VkDeviceSize imageSize = memRequirements.size;
        getRenderDevice()->createBuffer(
            imageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            m_imageResources[ i ].stagingBuffer,
            m_imageResources[ i ].stagingBufferMemory
        );

        // Image View
        getRenderDevice()->createImageView(
            m_imageResources[ i ].image,
            m_imageFormat,
            VK_IMAGE_ASPECT_COLOR_BIT,
            0,
            m_imageResources[ i ].imageView
        );
        getRenderDevice()->setObjectName( m_imageResources[ i ].imageView, "SoftRT/ImageView" );

        CRIMILD_VULKAN_CHECK(
            vkCreateSampler(
                getRenderDevice()->getHandle(),
                &samplerInfo,
                nullptr,
                &m_imageResources[ i ].sampler
            )
        );
        getRenderDevice()->setObjectName( m_imageResources[ i ].sampler, "SoftRT/Sampler" );
    }
}

void SoftRTPass::destroyImageResources( void ) noexcept
{
    for ( auto &res : m_imageResources ) {
        vkDestroySampler( getRenderDevice()->getHandle(), res.sampler, nullptr );
        vkDestroyImageView( getRenderDevice()->getHandle(), res.imageView, nullptr );
        vkDestroyImage( getRenderDevice()->getHandle(), res.image, nullptr );
        vkFreeMemory( getRenderDevice()->getHandle(), res.memory, nullptr );
        vkDestroyBuffer( getRenderDevice()->getHandle(), res.stagingBuffer, nullptr );
        vkFreeMemory( getRenderDevice()->getHandle(), res.stagingBufferMemory, nullptr );
    }
    m_imageResources.clear();
}

void SoftRTPass::initializeRays( void ) noexcept
{
    // Assumes images are 2D
    for ( Real y = 0; y < m_renderArea.extent.height; ++y ) {
        for ( Real x = 0; x < m_renderArea.extent.width; ++x ) {
            m_rays.push_back(
                RayInfo {
                    .ray = Ray3 {},
                    .sampleColor = ColorRGB { 1, 1, 1 },
                    .accumColor = ColorRGB { 0, 0, 0 },
                    .uv = Vector2 { x, y },
                    .bounces = 0,
                    .samples = 0,
                }
            );
        }
    }

    // // TODO(hernan): Add a setting for suffling rays.
    // if ( getSettings()->get< Bool >( "rt.shuffle", true ) ) {
    //     std::shuffle( m_rays.begin(), m_rays.end(), random::defaultGenerator() );
    // }
}

void SoftRTPass::setup( Node *scene, Camera *camera ) noexcept
{
    auto settings = Settings::getInstance();

    // Memoize scene
    [ & ] {
        if ( m_scene != nullptr ) {
            return;
        }

        // We want to avoid blocking the main thread so we can get a progressive result,
        // but scenes can be updated in between samples. During an update, a scene could
        // reset its transform during the world update process, which might also affect
        // the camera. So, the easiest way to avoid artifacts during rendering is to
        // clone the entire scene. This is very much like collecting renderables and
        // forwarding them to the GPU, just much slower.
        // TODO: should I build an acceleration structor (BVH?) as part of this process?
        // TODO: maybe just collect geometries...

        CRIMILD_LOG_DEBUG( "Cloning scene before rendering" );

        scene->perform( UpdateWorldState() );

        m_scene = scene->perform< ShallowCopy >();
        m_scene->perform( UpdateWorldState() );

        auto splitStrategy = [ & ] {
            auto splitStrategyStr = settings->get< std::string >( "rt.split_axis", "random" );
            if ( splitStrategyStr == "random" ) {
                return BinTreeScene::SplitStrategy::RANDOM_AXIS;
            } else if ( splitStrategyStr == "x" ) {
                return BinTreeScene::SplitStrategy::X_AXIS;
            } else if ( splitStrategyStr == "y" ) {
                return BinTreeScene::SplitStrategy::Y_AXIS;
            } else if ( splitStrategyStr == "z" ) {
                return BinTreeScene::SplitStrategy::Z_AXIS;
            } else {
                return BinTreeScene::SplitStrategy::NONE;
            }
        }();
        m_acceleratedScene = scene->perform< BinTreeScene >( splitStrategy )->perform< RTAcceleration >();
    }();

    // Memoize camera
    [ & ] {
        if ( m_camera != nullptr ) {
            return;
        }

        m_camera = crimild::alloc< Camera >( 45, m_renderArea.extent.width / m_renderArea.extent.height, 0.1f, 1000.0f );
        m_camera->setLocal( camera->getLocal() );
        m_camera->setWorld( camera->getWorld() );
        m_camera->setWorldIsCurrent( true );

        // ShallowCopy copy;
        // camera->perform( copy );
        // m_camera = copy.getResult< Camera >();
    }();

    if ( m_scene == nullptr || m_camera == nullptr ) {
        CRIMILD_LOG_ERROR( "No scene or camera" );
        return;
    }

    initializeRays();
}

void SoftRTPass::onSampleCompleted( RayInfo &rayInfo ) noexcept
{
    // Check for NaN values before accumulating color. That way
    // we avoid "dead" pixels (either black or white) due to
    // invalid values.
    rayInfo.accumColor = rayInfo.accumColor + ( isNaN( rayInfo.sampleColor ) ? ColorRGB { 0, 0, 0 } : rayInfo.sampleColor );
    rayInfo.samples++;

    ColorRGB color = rayInfo.accumColor / Real( rayInfo.samples );
    m_imageData[ rayInfo.uv.y * m_renderArea.extent.width + rayInfo.uv.x ] = rgba( color );

    rayInfo.sampleColor = ColorRGB { 1, 1, 1 };
    rayInfo.bounces = 0;
}

Ray3 SoftRTPass::getRay( RayInfo &rayInfo ) noexcept
{
    if ( rayInfo.bounces > 0 ) {
        // Not the first bounce, so info already contains a valid ray
        return rayInfo.ray;
    }

    // First bounce. Generate a new ray from the camera

    // Basic antialliasing by offseting the UV coordinates
    auto uv = rayInfo.uv;
    uv = uv + random::nextVector2( -1, 1 );
    uv = uv / Vector2 { m_renderArea.extent.width - 1.0f, m_renderArea.extent.height - 1.0f };

    Ray3 ray;
    m_camera->getPickRay( uv.x, uv.y, ray );

    // Calculate depth-of-field based on camera properties
    // TODO(hernan): avoid doing these calculations every time
    auto focusDist = m_camera->getFocusDistance();
    auto aperture = m_camera->getAperture();
    const auto cameraFocusDistance = focusDist;
    const auto cameraAperture = aperture;
    const auto cameraLensRadius = 0.5f * cameraAperture;
    const auto cameraRight = right( m_camera->getWorld() );
    const auto cameraUp = up( m_camera->getWorld() );

    const auto rd = cameraLensRadius * random::nextInUnitDisk();
    const auto offset = cameraRight * rd.x + cameraUp * rd.y;

    rayInfo.ray = Ray3 {
        origin( ray ) + offset,
        cameraFocusDistance * direction( ray ) - offset,
    };

    return rayInfo.ray;
}

Bool SoftRTPass::intersectPrim( const Ray3 &R, const RTAcceleration::Result &scene, Int32 nodeId, Real minT, Real maxT, IntersectionResult &result ) noexcept
{
    Bool hasResult = false;

    const auto &node = scene.nodes[ nodeId ];
    const auto primIdx = node.primitiveIndex;
    if ( primIdx < 0 ) {
        return false;
    }

    struct FrontierNode {
        Int32 id;
        Real t0;
        Real t1;
    };

    FrontierNode frontier[ 64 ] = { { primIdx, minT, maxT } };
    Int32 currentIdx = 0;

    while ( currentIdx >= 0 ) {
        const auto expanded = frontier[ currentIdx-- ];
        if ( expanded.id < 0 ) {
            continue;
        }

        const auto &prim = scene.primitives.primTree[ expanded.id ];
        if ( prim.isLeaf() ) {
            const auto idxOffset = prim.primitiveIndicesOffset;
            const auto primCount = prim.getPrimCount();
            for ( auto i = 0; i < primCount; ++i ) {
                const auto baseIdx = scene.primitives.indexOffsets[ idxOffset + i ];

                const auto &v0 = scene.primitives.triangles[ scene.primitives.indices[ baseIdx + 0 ] ];
                const auto &v1 = scene.primitives.triangles[ scene.primitives.indices[ baseIdx + 1 ] ];
                const auto &v2 = scene.primitives.triangles[ scene.primitives.indices[ baseIdx + 2 ] ];

                const auto T = Triangle {
                    v0.position,
                    v1.position,
                    v2.position,
                };

                Real t;
                // TODO(hernan): Pre-compute inverse world ray to avoid doing it every loop
                if ( intersect( R, T, node.world, t ) ) {
                    if ( t >= numbers::EPSILON && !isNaN( t ) && !isEqual( t, numbers::POSITIVE_INFINITY ) && ( !hasResult || t < result.t ) ) {
                        result.t = t;
                        result.point = R( result.t );
                        result.setFaceNormal( R, normal3( v0.normal ) );
                        result.materialId = node.materialIndex;
                        hasResult = true;
                    }
                }
            }
        } else {
            const auto P = Plane3 {
                [ & ] {
                    auto N = Normal3 { 0, 0, 0 };
                    N[ prim.getSplitAxis() ] = 1;
                    return N;
                }(),
                prim.getSplitPos()
            };

            // TODO: Do this earlier
            const auto R1 = inverse( node.world )( R );

            Real t;
            if ( intersect( R1, P, t ) ) {
                if ( prim.getAboveChild() > ( expanded.id + 1 ) ) {
                    const bool belowFirst = whichSide( P, origin( R1 ) ) < 0;
                    if ( belowFirst ) {
                        frontier[ ++currentIdx ] = {
                            prim.getAboveChild(),
                            t,
                            expanded.t1,
                        };
                        frontier[ ++currentIdx ] = {
                            expanded.id + 1,
                            expanded.t0,
                            t,
                        };
                    } else {
                        frontier[ ++currentIdx ] = {
                            expanded.id + 1,
                            expanded.t0,
                            t,
                        };
                        frontier[ ++currentIdx ] = {
                            prim.getAboveChild(),
                            t,
                            expanded.t1,
                        };
                    }
                } else {
                    // visit first child
                    frontier[ ++currentIdx ] = {
                        expanded.id + 1,
                        expanded.t0,
                        t,
                    };
                }
            }
        }
    }

    return hasResult;
}

Bool SoftRTPass::intersectNR( const Ray3 &R, const RTAcceleration::Result &scene, IntersectionResult &result ) noexcept
{
    if ( scene.nodes.empty() ) {
        return false;
    }

    Bool hasResult = false;

    Index frontier[ 64 ] = { 0 };
    Int32 currentIndex = 0;

    auto computeVolume = [ & ]( const auto &node, auto t0, auto t1 ) {
        const auto &material = scene.materials[ node.materialIndex ];
        const auto isVolume = material.density >= 0;
        if ( !isVolume ) {
            return false;
        }

        const bool enableDebug = true;
        const bool debugging = enableDebug && random::next() < 0.00001;
        if ( debugging )
            std::cerr << "\nt_min=" << t0 << ", t_max=" << t1 << '\n';

        if ( t0 < 0 ) {
            t0 = 0;
        }

        if ( t1 > t0 ) {
            Real d = length( direction( R ) );
            Real distanceInsideBoundary = ( t1 - t0 ) * d;
            Real density = material.density;
            Real negInvDensity = Real( -1 ) / density;
            Real hitDistance = negInvDensity * std::log( random::next() );
            if ( hitDistance <= distanceInsideBoundary ) {
                Real t = t0 + hitDistance / d;
                const auto P = R( t );

                if ( debugging ) {
                    std::cerr << "hit_distance = " << hitDistance << '\n'
                              << "rec.t = " << t << '\n'
                              << "rec.p = " << P.x << ", " << P.y << ", " << P.z << '\n';
                }

                result.t = t;
                result.point = P;
                result.normal = Normal3 { 1, 0, 0 }; // arbitrary value
                result.frontFace = true;             // arbitrary value
                result.materialId = node.materialIndex;
                hasResult = true;
            }
        }

        return true;
    };

    while ( currentIndex >= 0 ) {
        const auto nodeIdx = frontier[ currentIndex ];
        --currentIndex;

        const auto &node = scene.nodes[ nodeIdx ];

        switch ( node.type ) {
            case RTAcceleratedNode::Type::GROUP: {
                const auto B = Box {};
                Real t0, t1;
                if ( intersect( R, B, node.world, t0, t1 ) ) {
                    // A very simple test: only expand child nodes if the intersection point
                    // is less than the current result. Otherwise, the entire group is occluded
                    // by the current hit and can be safely discarded.
                    if ( !hasResult || t0 <= result.t ) {
                        if ( node.secondChildIndex > 0 ) {
                            frontier[ ++currentIndex ] = node.secondChildIndex;
                        }
                        frontier[ ++currentIndex ] = nodeIdx + 1;
                    }
                }
                break;
            }

            case RTAcceleratedNode::Type::GEOMETRY: {
                const auto B = Box {};
                Real t0, t1;
                if ( intersect( R, B, node.world, t0, t1 ) ) {
                    // TODO(hernan): when traversing optimized triangles, we need to keep
                    // track of the min/max times for the intersection above.
                    // TODO(hernan): Maybe by keeping track of those values I can also
                    // solve how CSG nodes will be evaluated, since they also need to
                    // save min/max times for intersections.
                    const auto primIdx = nodeIdx + 1;
                    if ( scene.nodes[ primIdx ].type == RTAcceleratedNode::Type::PRIMITIVE_TRIANGLES ) {
                        if ( intersectPrim( R, scene, primIdx, t0, t1, result ) ) {
                            hasResult = true;
                        }
                    } else {
                        frontier[ ++currentIndex ] = nodeIdx + 1;
                    }
                }
                break;
            }

            case RTAcceleratedNode::Type::PRIMITIVE_SPHERE: {
                const auto S = Sphere {};
                Real t0, t1;
                if ( intersect( R, S, node.world, t0, t1 ) ) {
                    if ( t0 >= numbers::EPSILON && !isEqual( t0, numbers::POSITIVE_INFINITY ) && ( !hasResult || t0 < result.t ) ) {
                        result.t = t0;
                        result.point = R( result.t );
                        result.setFaceNormal( R, normal( S, node.world, result.point ) );
                        result.materialId = node.materialIndex;
                        hasResult = true;
                    }

                    if ( t1 >= numbers::EPSILON && !isEqual( t1, numbers::POSITIVE_INFINITY ) && ( !hasResult || t1 < result.t ) ) {
                        result.t = t1;
                        result.point = R( result.t );
                        result.setFaceNormal( R, normal( S, node.world, result.point ) );
                        result.materialId = node.materialIndex;
                        hasResult = true;
                    }
                }
                break;
            }

            case RTAcceleratedNode::Type::PRIMITIVE_BOX: {
                const auto B = Box {};
                Real t0, t1;
                if ( intersect( R, B, node.world, t0, t1 ) ) {
                    if ( !computeVolume( node, t0, t1 ) ) {
                        if ( t0 >= numbers::EPSILON && !isEqual( t0, numbers::POSITIVE_INFINITY ) && ( !hasResult || t0 < result.t ) ) {
                            result.t = t0;
                            result.point = R( result.t );
                            result.setFaceNormal( R, normal( B, node.world, result.point ) );
                            result.materialId = node.materialIndex;
                            hasResult = true;
                        }

                        if ( t1 >= numbers::EPSILON && !isEqual( t1, numbers::POSITIVE_INFINITY ) && ( !hasResult || t1 < result.t ) ) {
                            result.t = t1;
                            result.point = R( result.t );
                            result.setFaceNormal( R, normal( B, node.world, result.point ) );
                            result.materialId = node.materialIndex;
                            hasResult = true;
                        }
                    }
                }
                break;
            }

            // case Primitive::Type::OPEN_CYLINDER:
            case RTAcceleratedNode::Type::PRIMITIVE_CYLINDER: {
                const auto C = Cylinder { .closed = true };
                Real t0, t1;
                if ( intersect( R, C, node.world, t0, t1 ) ) {
                    if ( t0 >= numbers::EPSILON && !isEqual( t0, numbers::POSITIVE_INFINITY ) && ( !hasResult || t0 < result.t ) ) {
                        result.t = t0;
                        result.point = R( result.t );
                        result.setFaceNormal( R, normal( C, node.world, result.point ) );
                        result.materialId = node.materialIndex;
                        hasResult = true;
                    }

                    if ( t1 >= numbers::EPSILON && !isEqual( t1, numbers::POSITIVE_INFINITY ) && ( !hasResult || t1 < result.t ) ) {
                        result.t = t1;
                        result.point = R( result.t );
                        result.setFaceNormal( R, normal( C, node.world, result.point ) );
                        result.materialId = node.materialIndex;
                        hasResult = true;
                    }
                }
                break;
            }

            default: {
                break;
            }
        }
    }

    return hasResult;
}

[[nodiscard]] static Real reflectance( Real cosTheta, Real refIdx ) noexcept
{
    // Use Schlick's approximation for reflectance
    const auto r0 = ( 1 - refIdx ) / ( 1 + refIdx );
    const auto rr = r0 * r0;
    return rr + ( 1 - rr ) * pow( ( 1 - cosTheta ), 5 );
}

bool SoftRTPass::scatter( const RTAcceleration::Result &scene, const Ray3 &R, const IntersectionResult &result, Ray3 &scattered, ColorRGB &attenuation, Bool &shouldBounce ) noexcept
{
    shouldBounce = true;

    const auto &material = scene.materials[ result.materialId ];

    if ( material.transmission > 0 ) {
        // Transmissive
        attenuation = ColorRGB::Constants::WHITE;

        const auto ior = material.indexOfRefraction;

        const auto refractionRatio = result.frontFace ? ( Real( 1 ) / ior ) : ior;
        const auto dir = normalize( direction( R ) );
        double cosTheta = min( dot( -dir, result.normal ), Real( 1 ) );
        double sinTheta = sqrt( Real( 1 ) - cosTheta * cosTheta );
        const auto cannotRefract = refractionRatio * sinTheta > 1;
        const auto scatteredDirection = [ & ] {
            if ( cannotRefract || reflectance( cosTheta, refractionRatio ) > random::next() ) {
                return reflect( dir, result.normal );
            } else {
                return refract( dir, result.normal, refractionRatio );
            }
        }();
        scattered = Ray3 { result.point, scatteredDirection };
        return true;
    }

    if ( material.metallic > 0 ) {
        auto reflected = reflect( direction( R ), result.normal );
        scattered = Ray3 {
            result.point,
            reflected + material.roughness * random::nextInUnitSphere(),
        };
        attenuation = material.albedo;
        return dot( direction( scattered ), result.normal ) > 0;
    }

    if ( !isZero( material.emissive ) ) {
        shouldBounce = false;
        attenuation = material.emissive;
        return true;
    }

    auto scatteredDirection = vector3( result.normal ) + normalize( random::nextInUnitSphere() );
    // Catch degenerate scatter direction
    if ( isZero( scatteredDirection ) ) {
        scatteredDirection = vector3( result.normal );
    }
    scattered = Ray3 { result.point, scatteredDirection };
    attenuation = material.albedo;
    return true;
}

void SoftRTPass::doSampleBounce( RayInfo &rayInfo, const RTAcceleration::Result &scene ) noexcept
{
    if ( rayInfo.bounces > 5 ) {
        // If ray bounces too many times, it migth indicate that
        // no light (or only a tiny bit of it) reaches that point.
        rayInfo.sampleColor = ColorRGB { 0, 0, 0 };
        onSampleCompleted( rayInfo );
        return;
    }

    IntersectionResult result;
    const auto R = getRay( rayInfo );
    if ( !intersectNR( R, scene, result ) ) {
        // no intersection. Use background color
        rayInfo.sampleColor = rayInfo.sampleColor * m_backgroundColor;
        onSampleCompleted( rayInfo );
        return;
    }

    Ray3 scattered;
    ColorRGB attenuation;
    Bool shouldBounce;
    if ( scatter( scene, R, result, scattered, attenuation, shouldBounce ) ) {
        rayInfo.sampleColor = rayInfo.sampleColor * attenuation;
        if ( shouldBounce ) {
            rayInfo.ray = scattered;
            rayInfo.bounces++;
        } else {
            onSampleCompleted( rayInfo );
        }
    } else {
        rayInfo.sampleColor = ColorRGB { 0, 0, 0 };
        onSampleCompleted( rayInfo );
    }
}

void SoftRTPass::renderScene( Node *scene, Camera *camera ) noexcept
{
    if ( m_scene == nullptr ) {
        setup( scene, camera );
    }

    if ( m_scene == nullptr ) {
        return;
    }

    for ( auto &ray : m_rays ) {
        do {
            doSampleBounce( ray, m_acceleratedScene );
        } while ( ray.bounces != 0 );
    }
}

void SoftRTPass::updateImage( size_t frameIndex ) noexcept
{
    auto renderDevice = getRenderDevice();

    renderDevice->copyToBuffer(
        m_imageResources[ frameIndex ].stagingBufferMemory,
        m_imageData.data(),
        m_imageData.size()
    );

    renderDevice->transitionImageLayout(
        m_imageResources[ frameIndex ].image,
        m_imageFormat,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        1
    );

    renderDevice->copyBufferToImage(
        m_imageResources[ frameIndex ].stagingBuffer,
        m_imageResources[ frameIndex ].image,
        m_renderArea.extent.width,
        m_renderArea.extent.height,
        1
    );

    renderDevice->transitionImageLayout(
        m_imageResources[ frameIndex ].image,
        m_imageFormat,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        1,
        1
    );
}
