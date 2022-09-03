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

#include "Rendering/RenderPasses/VulkanLocalLightingPass.hpp"

#include "Components/MaterialComponent.hpp"
#include "Mathematics/swizzle.hpp"
#include "Primitives/Primitive.hpp"
#include "Primitives/SpherePrimitive.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"
#include "Rendering/RenderableSet.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/ShadowMap.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/Vertex.hpp"
#include "Rendering/VulkanGraphicsPipeline.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Geometry.hpp"
#include "Simulation/Settings.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/FetchLights.hpp"

#include <array>

using namespace crimild;
using namespace crimild::vulkan;

struct DirectionalLightProps {
    alignas( 16 ) Vector3f direction;
    alignas( 16 ) ColorRGBf color;
};

static const auto DIRECTIONAL_LIGHT_VERT_SHADER = R"(
    layout( set = 0, binding = 0 ) uniform RenderPassUniforms {
        mat4 view;
        mat4 proj;
        vec2 viewportSize;
    };

    vec2 positions[6] = vec2[](
        vec2( -1.0, 1.0 ),
        vec2( -1.0, -1.0 ),
        vec2( 1.0, -1.0 ),
        vec2( -1.0, 1.0 ),
        vec2( 1.0, -1.0 ),
        vec2( 1.0, 1.0 )
    );

    layout ( location = 0 ) out vec2 outViewportSize;

    void main()
    {
        gl_Position = vec4( positions[ gl_VertexIndex ], 0.0, 1.0 );
        outViewportSize = viewportSize;
    }
)";

static const auto DIRECTIONAL_LIGHT_FRAG_SHADER = R"(
    layout ( location = 0 ) in vec2 inViewportSize;

    layout ( set = 0, binding = 1 ) uniform sampler2D uAlbedo;
    layout ( set = 0, binding = 2 ) uniform sampler2D uPosition;
    layout ( set = 0, binding = 3 ) uniform sampler2D uNormal;
    layout ( set = 0, binding = 4 ) uniform sampler2D uMaterial;

    layout ( set = 1, binding = 0 ) uniform LightProps {
        vec3 direction;
        vec3 color;
    } uLight;

    layout( location = 0 ) out vec4 outColor;

    void main()
    {
        vec2 uv = gl_FragCoord.st / inViewportSize;

        vec3 albedo = texture( uAlbedo, uv ).rgb;
        vec3 N = normalize( texture( uNormal, uv ).rgb );

        vec3 L = normalize( -uLight.direction );

        // Simple diffuse lighting
        vec3 diffuse = uLight.color * max( dot( N, L ), 0.0 );

        outColor = vec4( albedo * diffuse, 1.0 );
    }
)";

struct PointLightProps {
    alignas( 16 ) Point3f position;
    alignas( 4 ) Real32 radius;
    alignas( 16 ) ColorRGBf color;
};

static const auto POINT_LIGHT_VERT_SHADER = R"(
    layout ( location = 0 ) in vec3 inPosition;

    layout( set = 0, binding = 0 ) uniform RenderPassUniforms {
        mat4 view;
        mat4 proj;
        vec2 viewportSize;
    };

    layout ( set = 1, binding = 0 ) uniform LightProps {
        vec3 position;
        float radius;
        vec3 color;
    } uLight;

    layout ( location = 0 ) out vec2 outViewportSize;

    void main()
    {
        gl_Position = proj * view * vec4( ( uLight.position.xyz + uLight.radius * inPosition ), 1.0 );

        outViewportSize = viewportSize;
    }
)";

static const auto POINT_LIGHT_FRAG_SHADER = R"(
    layout ( location = 0 ) in vec2 inViewportSize;

    layout ( set = 0, binding = 1 ) uniform sampler2D uAlbedo;
    layout ( set = 0, binding = 2 ) uniform sampler2D uPosition;
    layout ( set = 0, binding = 3 ) uniform sampler2D uNormal;
    layout ( set = 0, binding = 4 ) uniform sampler2D uMaterial;

    layout ( set = 1, binding = 0 ) uniform LightProps {
        vec3 position;
        float radius;
        vec3 color;
    } uLight;

    layout( location = 0 ) out vec4 outColor;

    // Calculate light's attenuation based on distance and radius
    float attenuation( float d, float r )
    {
        // See: https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/
        float Kc = 1.0;
        float Kl = 2.0 / r;
        float Kq = 1 / ( r * r );
        return 1 / ( Kc + Kl * d + Kq * d * d );
    }

    void main()
    {
        vec2 uv = gl_FragCoord.st / inViewportSize;

        vec3 albedo = texture( uAlbedo, uv ).rgb;
        vec3 P = texture( uPosition, uv ).xyz;
        vec3 N = normalize( texture( uNormal, uv ).xyz );

        vec3 L = uLight.position - P;
        float distance = length( L );
        L = normalize( L );

        // Simple diffuse lighting
        vec3 diffuse = uLight.color * max( dot( N, L ), 0.0 );

        // Simple attenuation
        diffuse *= attenuation( distance, uLight.radius );

        outColor = vec4( albedo * diffuse, 1.0 );
    }
)";

struct SpotLightProps {
    alignas( 16 ) Point3f position;
    alignas( 16 ) Vector3f direction;
    alignas( 4 ) Real32 radius;
    alignas( 16 ) ColorRGBf color;
    alignas( 4 ) Real32 innerCutoff;
    alignas( 4 ) Real32 outerCutoff;
};

static const auto SPOT_LIGHT_VERT_SHADER = R"(
    layout ( location = 0 ) in vec3 inPosition;

    layout( set = 0, binding = 0 ) uniform RenderPassUniforms {
        mat4 view;
        mat4 proj;
        vec2 viewportSize;
    };

    layout ( set = 1, binding = 0 ) uniform LightProps {
        vec3 position;
        vec3 direction;
        float radius;
        vec3 color;
        float innerCutoff;
        float outerCutoff;
    } uLight;

    layout ( location = 0 ) out vec2 outViewportSize;

    void main()
    {
        gl_Position = proj * view * vec4( ( uLight.position.xyz + uLight.radius * inPosition ), 1.0 );

        outViewportSize = viewportSize;
    }
)";

static const auto SPOT_LIGHT_FRAG_SHADER = R"(
    layout ( location = 0 ) in vec2 inViewportSize;

    layout ( set = 0, binding = 1 ) uniform sampler2D uAlbedo;
    layout ( set = 0, binding = 2 ) uniform sampler2D uPosition;
    layout ( set = 0, binding = 3 ) uniform sampler2D uNormal;
    layout ( set = 0, binding = 4 ) uniform sampler2D uMaterial;

    layout ( set = 1, binding = 0 ) uniform LightProps {
        vec3 position;
        vec3 direction;
        float radius;
        vec3 color;
        float innerCutoff;
        float outerCutoff;
    } uLight;

    layout( location = 0 ) out vec4 outColor;

    // Calculate light's attenuation based on distance and radius
    float attenuation( float d, float r )
    {
        // See: https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/
        float Kc = 1.0;
        float Kl = 2.0 / r;
        float Kq = 1 / ( r * r );
        return 1 / ( Kc + Kl * d + Kq * d * d );
    }

    void main()
    {
        vec2 uv = gl_FragCoord.st / inViewportSize;

        vec3 albedo = texture( uAlbedo, uv ).rgb;
        vec3 P = texture( uPosition, uv ).xyz;
        vec3 N = normalize( texture( uNormal, uv ).xyz );

        vec3 L = uLight.position - P;
        float distance = length( L );
        L = normalize( L );

        // Simple diffuse lighting with attenuation
        vec3 diffuse = uLight.color * max( dot( N, L ), 0.0 );
        diffuse *= attenuation( distance, uLight.radius );

        // Spot
        float theta = dot( L, normalize( -uLight.direction ) );
        float epsilon = uLight.innerCutoff - uLight.outerCutoff;
        float intensity = clamp( ( theta - uLight.outerCutoff ) / epsilon, 0.0, 1.0 );
        vec3 I = diffuse * intensity;

        outColor = vec4( albedo * I, 1.0 );
    }
)";

LocalLightingPass::LocalLightingPass(
    RenderDevice *renderDevice,
    const std::vector< const FramebufferAttachment * > inputs,
    const FramebufferAttachment *output
) noexcept
    : RenderPassBase( renderDevice ),
      m_inputAttachments( inputs ),
      m_outputAttachment( output )
{
    m_lightVolume = crimild::alloc< SpherePrimitive >(
        SpherePrimitive::Params {
            .type = Primitive::Type::TRIANGLES,
            .radius = 1.0,
            .layout = VertexP3::getLayout(),
        }
    );

    init();
}

LocalLightingPass::~LocalLightingPass( void ) noexcept
{
    deinit();
}

Event LocalLightingPass::handle( const Event &e ) noexcept
{
    switch ( e.type ) {
        case Event::Type::WINDOW_RESIZE: {
            deinit();
            init();
            break;
        }

        default:
            break;
    }

    return e;
}

void LocalLightingPass::render( Node *scene, Camera *camera ) noexcept
{
    fetchLights( scene );
    updateCameraUniforms( camera );

    const auto currentFrameIndex = getRenderDevice()->getCurrentFrameIndex();
    auto commandBuffer = getRenderDevice()->getCurrentCommandBuffer();

    auto renderPassInfo = VkRenderPassBeginInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = m_renderPass,
        .framebuffer = m_framebuffers[ currentFrameIndex ],
        .renderArea = m_renderArea,
        .clearValueCount = 0,
        .pClearValues = nullptr,
    };

    vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );

    for ( const auto light : m_directionalLights.lights ) {
        vkCmdBindPipeline(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_directionalLights.pipeline->getHandle()
        );
        vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_directionalLights.pipeline->getPipelineLayout(),
            0,
            1,
            &m_renderPassObjects.descriptorSets[ currentFrameIndex ],
            0,
            nullptr
        );
        bindDirectionalLightDescriptors( commandBuffer, currentFrameIndex, light );
        vkCmdDraw( commandBuffer, 6, 1, 0, 0 );
    }

    for ( const auto light : m_pointLights.lights ) {
        // TODO: Use instancing to render all lights in one call
        vkCmdBindPipeline(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pointLights.pipeline->getHandle()
        );
        vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pointLights.pipeline->getPipelineLayout(),
            0,
            1,
            &m_renderPassObjects.descriptorSets[ currentFrameIndex ],
            0,
            nullptr
        );
        bindPointLightDescriptors( commandBuffer, currentFrameIndex, light );
        drawPrimitive( commandBuffer, m_lightVolume.get() );
    }

    for ( const auto light : m_spotLights.lights ) {
        // TODO: Use instancing to render all lights in one call
        vkCmdBindPipeline(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_spotLights.pipeline->getHandle()
        );
        vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_spotLights.pipeline->getPipelineLayout(),
            0,
            1,
            &m_renderPassObjects.descriptorSets[ currentFrameIndex ],
            0,
            nullptr
        );
        bindSpotLightDescriptors( commandBuffer, currentFrameIndex, light );
        drawPrimitive( commandBuffer, m_lightVolume.get() );
    }

    vkCmdEndRenderPass( commandBuffer );
}

void LocalLightingPass::init( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    m_renderArea = VkRect2D {
        .extent = m_outputAttachment->extent,
        .offset = { 0, 0 },
    };

    const auto extent = m_renderArea.extent;

    auto attachments = std::array< VkAttachmentDescription, 1 > {
        VkAttachmentDescription {
            .format = m_outputAttachment->format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        },
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
            m_outputAttachment->imageViews[ i ],
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

    createRenderPassObjects();

    createLightObjects( m_directionalLights, Light::Type::DIRECTIONAL );
    createLightObjects( m_pointLights, Light::Type::POINT );
    createLightObjects( m_spotLights, Light::Type::SPOT );
}

void LocalLightingPass::deinit( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDeviceWaitIdle( getRenderDevice()->getHandle() );

    destroyLightObjects( m_directionalLights );
    destroyLightObjects( m_pointLights );
    destroyLightObjects( m_spotLights );
    destroyRenderPassObjects();

    for ( auto &fb : m_framebuffers ) {
        vkDestroyFramebuffer( getRenderDevice()->getHandle(), fb, nullptr );
    }
    m_framebuffers.clear();

    vkDestroyRenderPass( getRenderDevice()->getHandle(), m_renderPass, nullptr );
    m_renderPass = VK_NULL_HANDLE;
}

void LocalLightingPass::createRenderPassObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    m_renderPassObjects.uniforms = [ & ] {
        auto ubo = std::make_unique< UniformBuffer >( RenderPassObjects::Uniforms {} );
        ubo->getBufferView()->setUsage( BufferView::Usage::DYNAMIC );
        getRenderDevice()->bind( ubo.get() );
        return ubo;
    }();

    auto poolSizes = std::vector< VkDescriptorPoolSize > {
        VkDescriptorPoolSize {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
        },
    };
    for ( const auto &att : m_inputAttachments ) {
        poolSizes.push_back(
            VkDescriptorPoolSize {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
            }
        );
    };

    auto poolCreateInfo = VkDescriptorPoolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = uint32_t( poolSizes.size() ),
        .pPoolSizes = poolSizes.data(),
        .maxSets = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
    };

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorPool( getRenderDevice()->getHandle(), &poolCreateInfo, nullptr, &m_renderPassObjects.pool ) );

    auto bindings = std::vector< VkDescriptorSetLayoutBinding > {
        VkDescriptorSetLayoutBinding {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
        },
    };
    uint32_t bindingIndex = bindings.size();
    for ( const auto &att : m_inputAttachments ) {
        bindings.push_back(
            VkDescriptorSetLayoutBinding {
                .binding = bindingIndex++,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr,
            }
        );
    }

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
        const auto bufferInfo = VkDescriptorBufferInfo {
            .buffer = getRenderDevice()->getHandle( m_renderPassObjects.uniforms.get(), i ),
            .offset = 0,
            .range = m_renderPassObjects.uniforms->getBufferView()->getLength(),
        };

        auto writes = std::vector< VkWriteDescriptorSet > {
            VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = m_renderPassObjects.descriptorSets[ i ],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .pBufferInfo = &bufferInfo,
                .pImageInfo = nullptr,
                .pTexelBufferView = nullptr,
            },
        };

        std::vector< VkDescriptorImageInfo > imageInfos;
        for ( const auto &att : m_inputAttachments ) {
            imageInfos.push_back(
                VkDescriptorImageInfo {
                    .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    .imageView = att->imageViews[ i ],
                    .sampler = att->sampler,
                }
            );
        }

        uint32_t dstBinding = writes.size();
        for ( uint32_t j = 0; j < m_inputAttachments.size(); ++j ) {
            writes.push_back(
                VkWriteDescriptorSet {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = m_renderPassObjects.descriptorSets[ i ],
                    .dstBinding = dstBinding + j,
                    .dstArrayElement = 0,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .descriptorCount = 1,
                    .pBufferInfo = nullptr,
                    // Use the recently added image view
                    .pImageInfo = &imageInfos[ j ],
                    .pTexelBufferView = nullptr,
                }
            );
        }

        vkUpdateDescriptorSets(
            getRenderDevice()->getHandle(),
            writes.size(),
            writes.data(),
            0,
            nullptr
        );
    }
}

void LocalLightingPass::destroyRenderPassObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDestroyDescriptorSetLayout( getRenderDevice()->getHandle(), m_renderPassObjects.layout, nullptr );
    m_renderPassObjects.layout = VK_NULL_HANDLE;

    vkDestroyDescriptorPool( getRenderDevice()->getHandle(), m_renderPassObjects.pool, nullptr );
    m_renderPassObjects.pool = VK_NULL_HANDLE;

    getRenderDevice()->unbind( m_renderPassObjects.uniforms.get() );
    m_renderPassObjects.uniforms = nullptr;
}

void LocalLightingPass::createLightObjects( LightObjects &objects, Light::Type lightType ) noexcept
{
    CRIMILD_LOG_TRACE();

    const auto layoutBinding = VkDescriptorSetLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        .pImmutableSamplers = nullptr,
    };

    auto layoutCreateInfo = VkDescriptorSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &layoutBinding,
    };

    CRIMILD_VULKAN_CHECK(
        vkCreateDescriptorSetLayout(
            getRenderDevice()->getHandle(),
            &layoutCreateInfo,
            nullptr,
            &objects.descriptorSetLayout
        )
    );

    auto program = crimild::alloc< ShaderProgram >();
    program->setShaders(
        Array< SharedPointer< Shader > > {
            crimild::alloc< Shader >(
                Shader::Stage::VERTEX,
                [ & ] {
                    switch ( lightType ) {
                        case Light::Type::DIRECTIONAL:
                            return DIRECTIONAL_LIGHT_VERT_SHADER;
                        case Light::Type::POINT:
                            return POINT_LIGHT_VERT_SHADER;
                        case Light::Type::SPOT:
                            return SPOT_LIGHT_VERT_SHADER;
                        default:
                            return "";
                    }
                }()
            ),
            crimild::alloc< Shader >(
                Shader::Stage::FRAGMENT,
                [ & ] {
                    switch ( lightType ) {
                        case Light::Type::DIRECTIONAL:
                            return DIRECTIONAL_LIGHT_FRAG_SHADER;
                        case Light::Type::POINT:
                            return POINT_LIGHT_FRAG_SHADER;
                        case Light::Type::SPOT:
                            return SPOT_LIGHT_FRAG_SHADER;
                        default:
                            return "";
                    }
                }()
            ),
        }
    );

    const auto viewport = ViewportDimensions::fromExtent( m_renderArea.extent.width, m_renderArea.extent.height );

    objects.pipeline = crimild::alloc< GraphicsPipeline >(
        getRenderDevice(),
        m_renderPass,
        GraphicsPipeline::Descriptor {
            .descriptorSetLayouts = std::vector< VkDescriptorSetLayout > {
                m_renderPassObjects.layout,
                objects.descriptorSetLayout,
            },
            .program = program.get(),
            .vertexLayouts = [ & ] {
                std::vector< VertexLayout > ret;
                if ( lightType != Light::Type::DIRECTIONAL ) {
                    ret.push_back( VertexP3::getLayout() );
                }
                return ret;
            }(),
            .depthStencilState = DepthStencilState {
                .depthTestEnable = false,
            },
            .rasterizationState = RasterizationState {
                .cullMode = lightType == Light::Type::DIRECTIONAL ? CullMode::BACK : CullMode::FRONT, // Render back faces only
            },
            .colorBlendState = ColorBlendState {
                .enable = true,
                .srcColorBlendFactor = BlendFactor::ONE,
                .dstColorBlendFactor = BlendFactor::ONE,
            },
            .viewport = viewport,
            .scissor = viewport,
        }
    );
}

void LocalLightingPass::destroyLightObjects( LightObjects &objects ) noexcept
{
    CRIMILD_LOG_TRACE();

    objects.pipeline = nullptr;

    // no need to destroy sets
    objects.descriptorSets.clear();

    vkDestroyDescriptorSetLayout(
        getRenderDevice()->getHandle(),
        objects.descriptorSetLayout,
        nullptr
    );
    objects.descriptorSetLayout = VK_NULL_HANDLE;

    for ( auto &it : objects.descriptorPools ) {
        vkDestroyDescriptorPool( getRenderDevice()->getHandle(), it.second, nullptr );
    }
    objects.descriptorPools.clear();

    for ( auto &it : objects.uniforms ) {
        getRenderDevice()->unbind( it.second.get() );
    }
    objects.uniforms.clear();
}

template< typename LightObjectsType, typename LightPropsType >
static void createLightDescriptors(
    RenderDevice *renderDevice,
    LightObjectsType &lightObjects,
    const Light *light,
    const LightPropsType &lightProps
) noexcept
{
    VkDescriptorPoolSize poolSize {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = uint32_t( renderDevice->getSwapchainImageCount() ),
    };

    auto poolCreateInfo = VkDescriptorPoolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize,
        .maxSets = uint32_t( renderDevice->getSwapchainImageCount() ),
    };

    CRIMILD_VULKAN_CHECK(
        vkCreateDescriptorPool(
            renderDevice->getHandle(),
            &poolCreateInfo,
            nullptr,
            &lightObjects.descriptorPools[ light ]
        )
    );

    lightObjects.uniforms[ light ] = crimild::alloc< UniformBuffer >( lightProps );
    renderDevice->bind( lightObjects.uniforms[ light ].get() );

    std::vector< VkDescriptorSetLayout > layouts(
        renderDevice->getSwapchainImageCount(),
        lightObjects.descriptorSetLayout
    );

    const auto allocInfo = VkDescriptorSetAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = lightObjects.descriptorPools[ light ],
        .descriptorSetCount = uint32_t( layouts.size() ),
        .pSetLayouts = layouts.data(),
    };

    lightObjects.descriptorSets[ light ].resize( renderDevice->getSwapchainImageCount() );
    CRIMILD_VULKAN_CHECK(
        vkAllocateDescriptorSets(
            renderDevice->getHandle(),
            &allocInfo,
            lightObjects.descriptorSets[ light ].data()
        )
    );

    for ( size_t i = 0; i < lightObjects.descriptorSets[ light ].size(); ++i ) {
        const auto bufferInfo = VkDescriptorBufferInfo {
            .buffer = renderDevice->getHandle( lightObjects.uniforms[ light ].get(), i ),
            .offset = 0,
            .range = lightObjects.uniforms[ light ]->getBufferView()->getLength(),
        };

        const auto descriptorWrite = VkWriteDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = lightObjects.descriptorSets[ light ][ i ],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .pBufferInfo = &bufferInfo,
            .pImageInfo = nullptr,
            .pTexelBufferView = nullptr,
        };

        vkUpdateDescriptorSets(
            renderDevice->getHandle(),
            1,
            &descriptorWrite,
            0,
            nullptr
        );
    }
}

void LocalLightingPass::bindDirectionalLightDescriptors(
    VkCommandBuffer cmds,
    Index currentFrameIndex,
    const Light *light
) noexcept
{
    DirectionalLightProps props;
    props.direction = light->getDirection();
    props.color = light->getColor();

    if ( !m_directionalLights.descriptorSets.contains( light ) ) {
        createLightDescriptors( getRenderDevice(), m_directionalLights, light, props );
    } else {
        m_directionalLights.uniforms[ light ]->setValue( props );
        getRenderDevice()->update( m_directionalLights.uniforms[ light ].get() );
    }

    vkCmdBindDescriptorSets(
        cmds,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_directionalLights.pipeline->getPipelineLayout(),
        1,
        1,
        &m_directionalLights.descriptorSets[ light ][ currentFrameIndex ],
        0,
        nullptr
    );
}

void LocalLightingPass::bindPointLightDescriptors(
    VkCommandBuffer cmds,
    Index currentFrameIndex,
    const Light *light
) noexcept
{
    PointLightProps props;
    props.position = light->getPosition();
    props.radius = light->getRadius();
    props.color = light->getColor();

    if ( !m_pointLights.descriptorSets.contains( light ) ) {
        createLightDescriptors( getRenderDevice(), m_pointLights, light, props );
    } else {
        m_pointLights.uniforms[ light ]->setValue( props );
        getRenderDevice()->update( m_pointLights.uniforms[ light ].get() );
    }

    vkCmdBindDescriptorSets(
        cmds,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pointLights.pipeline->getPipelineLayout(),
        1,
        1,
        &m_pointLights.descriptorSets[ light ][ currentFrameIndex ],
        0,
        nullptr
    );
}

void LocalLightingPass::bindSpotLightDescriptors(
    VkCommandBuffer cmds,
    Index currentFrameIndex,
    const Light *light
) noexcept
{
    SpotLightProps props;
    props.position = light->getPosition();
    props.direction = light->getDirection();
    props.radius = light->getRadius();
    props.color = light->getColor();
    props.innerCutoff = Numericf::cos( light->getInnerCutoff() );
    props.outerCutoff = Numericf::cos( light->getOuterCutoff() );

    if ( !m_spotLights.descriptorSets.contains( light ) ) {
        createLightDescriptors( getRenderDevice(), m_spotLights, light, props );
    } else {
        m_spotLights.uniforms[ light ]->setValue( props );
        getRenderDevice()->update( m_spotLights.uniforms[ light ].get() );
    }

    vkCmdBindDescriptorSets(
        cmds,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_spotLights.pipeline->getPipelineLayout(),
        1,
        1,
        &m_spotLights.descriptorSets[ light ][ currentFrameIndex ],
        0,
        nullptr
    );
}

void LocalLightingPass::drawPrimitive( VkCommandBuffer cmds, Primitive *primitive ) noexcept
{
    primitive->getVertexData().each(
        [ &, i = 0 ]( auto &vertices ) mutable {
            if ( vertices != nullptr ) {
                VkBuffer buffers[] = { getRenderDevice()->bind( vertices.get() ) };
                VkDeviceSize offsets[] = { 0 };
                vkCmdBindVertexBuffers( cmds, i, 1, buffers, offsets );
            }
        }
    );

    auto indices = primitive->getIndices();

    vkCmdBindIndexBuffer(
        cmds,
        getRenderDevice()->bind( indices ),
        0,
        utils::getIndexType( crimild::get_ptr( indices ) )
    );

    vkCmdDrawIndexed( cmds, indices->getIndexCount(), 1, 0, 0, 0 );
}

void LocalLightingPass::fetchLights( Node *scene ) noexcept
{
    // TODO: find a way to cache these collections
    m_directionalLights.lights.clear();
    m_pointLights.lights.clear();
    m_spotLights.lights.clear();

    if ( scene == nullptr ) {
        return;
    }

    FetchLights allLights;
    scene->perform( allLights );
    allLights.forEachLight(
        [ & ]( auto light ) {
            switch ( light->getType() ) {
                case Light::Type::DIRECTIONAL:
                    m_directionalLights.lights.insert( light );
                    break;
                case Light::Type::POINT:
                    m_pointLights.lights.insert( light );
                    break;
                case Light::Type::SPOT:
                    m_spotLights.lights.insert( light );
                    break;
                default:
                    break;
            }
        }
    );
}

void LocalLightingPass::updateCameraUniforms( const Camera *camera ) noexcept
{
    if ( camera == nullptr ) {
        return;
    }

    if ( m_renderPassObjects.uniforms == nullptr ) {
        return;
    }

    m_renderPassObjects.uniforms->setValue(
        RenderPassObjects::Uniforms {
            .view = camera->getViewMatrix(),
            .proj = camera->getProjectionMatrix(),
            .viewport = Vector2 {
                Real( m_renderArea.extent.width ),
                Real( m_renderArea.extent.height ),
            },
        }
    );
    getRenderDevice()->update( m_renderPassObjects.uniforms.get() );
}
