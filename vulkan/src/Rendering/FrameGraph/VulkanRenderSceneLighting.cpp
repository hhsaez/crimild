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

#include "Rendering/FrameGraph/VulkanRenderSceneLighting.hpp"

#include "Primitives/SpherePrimitive.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/Vertex.hpp"
#include "Rendering/VulkanCommandBuffer.hpp"
#include "Rendering/VulkanDescriptorPool.hpp"
#include "Rendering/VulkanDescriptorSet.hpp"
#include "Rendering/VulkanDescriptorSetLayout.hpp"
#include "Rendering/VulkanFramebuffer.hpp"
#include "Rendering/VulkanGraphicsPipeline.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanRenderDeviceCache.hpp"
#include "Rendering/VulkanRenderPass.hpp"
#include "Rendering/VulkanRenderTarget.hpp"
#include "Rendering/VulkanShadowMap.hpp"
#include "SceneGraph/Camera.hpp"

using namespace crimild;
using namespace crimild::vulkan;
using namespace crimild::vulkan::framegraph;

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
    #define DIRECTIONAL_LIGHT_CASCADES 4

    layout ( location = 0 ) in vec2 inViewportSize;

    layout ( set = 0, binding = 1 ) uniform sampler2D uAlbedo;
    layout ( set = 0, binding = 2 ) uniform sampler2D uPosition;
    layout ( set = 0, binding = 3 ) uniform sampler2D uNormal;
    layout ( set = 0, binding = 4 ) uniform sampler2D uMaterial;

    layout ( set = 1, binding = 0 ) uniform LightProps {
        float castShadows;
        vec3 direction;
        vec3 color;
        mat4 lightSpaceMatrices[ DIRECTIONAL_LIGHT_CASCADES ];
        vec4 splits;
    } uLight;

    layout( set = 1, binding = 1 ) uniform sampler2DArray uShadowMap;

    layout( location = 0 ) out vec4 outColor;

    const mat4 biasMat = mat4(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.5, 0.5, 0.0, 1.0
    );

    // Returns 1 if position is in shadows. 0 otherwise.
    // If PCF is enabled, it might return a value in between [0, 1].
    float readShadowMap( vec4 shadowCoord, vec2 offset, uint cascadeIndex )
    {
        float shadow = 0.0;
        if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) {
            float dist = texture( uShadowMap, vec3( shadowCoord.st + offset, cascadeIndex ) ).r;
            if ( shadowCoord.w > 0 && dist < shadowCoord.z ) {
                shadow = 1;
            }
        }
        return shadow;
    }

    float calcShadow( vec3 P, float fragZ )
    {
        float shadow = 0.0;

        // Get cascade index for the fragment's view position
        // These are all negative values. Lower means farther away from the eye
        uint cascadeIndex = 0;
        for ( uint i = 0; i < DIRECTIONAL_LIGHT_CASCADES - 1; ++i ) {
            if ( fragZ < uLight.splits[ i ] ) {
                cascadeIndex = i + 1;
            }
        }

        vec4 shadowCoord = biasMat * uLight.lightSpaceMatrices[ cascadeIndex ] * vec4( P, 1.0 );
        shadowCoord /= shadowCoord.w;

        // TODO: Move this to a setting
        bool pcf = true;

        if ( pcf ) {
            ivec2 shadowMapSize = textureSize( uShadowMap, 0 ).xy;
            float scale = 1.0;
            float dx = scale * 1.0 / float( shadowMapSize.x );
            float dy = scale * 1.0 / float( shadowMapSize.y );
            int count = 0;
            int range = 1;
            for ( int x = -range; x <= range; ++x ) {
                for ( int y = -range; y <= range; ++y ) {
                    shadow += readShadowMap( shadowCoord, vec2( dx * x, dy * y ), cascadeIndex );
                    count++;
                }
            }
            shadow /= count;
        } else {
            shadow = readShadowMap( shadowCoord, vec2( 0 ), cascadeIndex );
        }
        
        return shadow;
    }

    void main()
    {
        vec2 uv = gl_FragCoord.st / inViewportSize;

        vec3 albedo = texture( uAlbedo, uv ).rgb;
        vec4 Pz = texture( uPosition, uv );
        vec3 P = Pz.xyz;
        float fragZ = Pz.w;
        vec3 N = normalize( texture( uNormal, uv ).xyz );

        vec3 L = normalize( -uLight.direction );

        // Simple diffuse lighting
        vec3 diffuse = uLight.color * max( dot( N, L ), 0.0 );

        float shadow = uLight.castShadows * calcShadow( P, fragZ );

        outColor = vec4( albedo * diffuse * ( 1.0 - shadow ), 1.0 );
    }
)";

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
        float castShadows;
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
        float castShadows;
    } uLight;

    layout( set = 1, binding = 1 ) uniform samplerCube uShadowMap;

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

    // Pre-defined sample directions
    vec3 sampleOffsetDirections[ 20 ] = vec3[] (
        vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
        vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
        vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
        vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
        vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );

    // Performs shadow mapping with PCF
    float inShadows( float distance, vec3 L )
    {
        float shadow = 0.0;
        float bias = 0.15;
        int samples = 20;
        // Computes PCF disk radius using the current fragment's distance, so the
        // further the fragment is from the light, the more softer the shadow.
        float diskRadius = distance / pow( 1.0 + uLight.radius, 2.0 );
        for ( int i = 0; i < samples; ++i ) {
            float sampledDistance = texture( uShadowMap, normalize( L + sampleOffsetDirections[ i ] * diskRadius ) ).r;
            if ( distance >= sampledDistance + bias ) {
                shadow += 1;
            }
        }
        shadow /= float( samples );
        return shadow;

        float sampledDistance = texture( uShadowMap, L ).r;
        float EPSILON = 0.15;
        return ( distance >= sampledDistance + EPSILON ? 1.0 : 0.0 );
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

        float shadow = uLight.castShadows * inShadows( distance, -L );

        outColor = vec4( albedo * diffuse * ( 1.0 - shadow ), 1.0 );
    }
)";

static const auto SPOT_LIGHT_VERT_SHADER = R"(
    layout ( location = 0 ) in vec3 inPosition;

    layout( set = 0, binding = 0 ) uniform RenderPassUniforms {
        mat4 view;
        mat4 proj;
        vec2 viewportSize;
    };

    layout ( set = 1, binding = 0 ) uniform LightProps {
        float castShadows;
        vec3 position;
        vec3 direction;
        float radius;
        vec3 color;
        float innerCutoff;
        float outerCutoff;
        mat4 lightSpaceProjection;
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
        float castShadows;
        vec3 position;
        vec3 direction;
        float radius;
        vec3 color;
        float innerCutoff;
        float outerCutoff;
        mat4 lightSpaceProjection;
    } uLight;

    layout( set = 1, binding = 1 ) uniform sampler2D uShadowMap;

    layout( location = 0 ) out vec4 outColor;

    const mat4 biasMat = mat4(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.5, 0.5, 0.0, 1.0
    );

    // Returns 1 if position is in shadows. 0 otherwise.
    // If PCF is enabled, it might return a value in between [0, 1].
    float readShadowMap( vec4 shadowCoord, vec2 offset )
    {
        float shadow = 0.0;
        if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) {
            float dist = texture( uShadowMap, shadowCoord.st + offset ).r;
            if ( shadowCoord.w > 0 && dist < shadowCoord.z ) {
                shadow = 1;
            }
        }
        return shadow;
    }

    float calcShadow( vec3 P )
    {
        float shadow = 0.0;

        vec4 shadowCoord = biasMat * uLight.lightSpaceProjection * vec4( P, 1.0 );
        shadowCoord /= shadowCoord.w;

        // TODO: Move this to a setting
        bool pcf = true;

        if ( pcf ) {
            ivec2 shadowMapSize = textureSize( uShadowMap, 0 );
            float scale = 1.0;
            float dx = scale * 1.0 / float( shadowMapSize.x );
            float dy = scale * 1.0 / float( shadowMapSize.y );
            int count = 0;
            int range = 1;
            for ( int x = -range; x <= range; ++x ) {
                for ( int y = -range; y <= range; ++y ) {
                    shadow += readShadowMap( shadowCoord, vec2( dx * x, dy * y ) );
                    count++;
                }
            }
            shadow /= count;
        } else {
            shadow = readShadowMap( shadowCoord, vec2( 0 ) );
        }
        return shadow;
    }

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

        float shadow = 1.0 - uLight.castShadows * calcShadow( P );

        outColor = vec4( albedo * I * shadow, 1.0 );
    }
)";

static std::shared_ptr< vulkan::GraphicsPipeline > createGraphicsPipeline(
    RenderDevice *device,
    std::string name,
    Light::Type lightType,
    const VkExtent2D &extent,
    const std::shared_ptr< vulkan::RenderPass > &renderPass,
    const std::vector< VkDescriptorSetLayout > &descriptorSetLayouts
) noexcept
{
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

    const auto viewport = ViewportDimensions::fromExtent( extent.width, extent.height );

    auto pipeline = crimild::alloc< vulkan::GraphicsPipeline >(
        device,
        renderPass->getHandle(),
        vulkan::GraphicsPipeline::Descriptor {
            .descriptorSetLayouts = descriptorSetLayouts,
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
    device->setObjectName( pipeline->getHandle(), name );

    return pipeline;
}

RenderSceneLighting::RenderSceneLighting(
    RenderDevice *device,
    const VkExtent2D &extent,
    const std::vector< std::shared_ptr< RenderTarget > > &inputs,
    std::shared_ptr< RenderTarget > &output
) noexcept
    : RenderSceneBase( device, "RenderSceneLighting", extent ),
      WithCommandBuffer(
          crimild::alloc< CommandBuffer >(
              getRenderDevice(),
              getName() + "/CommandBuffer",
              VK_COMMAND_BUFFER_LEVEL_PRIMARY
          )
      ),
      m_inputs( inputs ),
      m_output( output )
{
    std::vector< std::shared_ptr< RenderTarget > > renderTargets = { m_output };

    m_resources.common.renderPass = crimild::alloc< RenderPass >(
        getRenderDevice(),
        getName() + "/RenderPass",
        renderTargets,
        VK_ATTACHMENT_LOAD_OP_CLEAR
    );

    m_resources.common.framebuffer = crimild::alloc< Framebuffer >(
        getRenderDevice(),
        getName() + "/Framebuffer",
        getExtent(),
        m_resources.common.renderPass,
        renderTargets
    );

    m_resources.common.uniforms = [ & ] {
        auto uniforms = crimild::alloc< UniformBuffer >( Resources::Common::UniformData {} );
        uniforms->getBufferView()->setUsage( BufferView::Usage::DYNAMIC );
        device->getCache()->bind( uniforms );
        return uniforms;
    }();

    m_resources.common.descriptorSet = crimild::alloc< DescriptorSet >(
        getRenderDevice(),
        getName() + "/Common/DescriptorSet",
        [ & ] {
            auto descriptors = std::vector< Descriptor > {
                Descriptor {
                    .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .buffer = device->getCache()->bind( m_resources.common.uniforms ),
                    .stage = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                },
            };
            for ( auto &target : m_inputs ) {
                descriptors.push_back(
                    Descriptor {
                        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                        .imageView = target->getImageView(),
                        .sampler = target->getSampler(),
                        .stage = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    }
                );
            }
            return descriptors;
        }()
    );

    m_resources.lights.descriptorSetLayout = crimild::alloc< DescriptorSetLayout >(
        getRenderDevice(),
        getName() + "/Lights/DescriptorSetLayout",
        std::vector< VkDescriptorSetLayoutBinding > {
            VkDescriptorSetLayoutBinding {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr,
            },
            VkDescriptorSetLayoutBinding {
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr,
            },
        }
    );

    m_resources.lights.directional.pipeline = createGraphicsPipeline(
        device,
        getName() + "/Lights/Directional/Pipeline",
        Light::Type::DIRECTIONAL,
        getExtent(),
        m_resources.common.renderPass,
        std::vector< VkDescriptorSetLayout > {
            m_resources.common.descriptorSet->getDescriptorSetLayout()->getHandle(),
            m_resources.lights.descriptorSetLayout->getHandle(),
        }
    );

    m_resources.lights.point.pipeline = createGraphicsPipeline(
        device,
        getName() + "/Lights/Point/Pipeline",
        Light::Type::POINT,
        getExtent(),
        m_resources.common.renderPass,
        std::vector< VkDescriptorSetLayout > {
            m_resources.common.descriptorSet->getDescriptorSetLayout()->getHandle(),
            m_resources.lights.descriptorSetLayout->getHandle(),
        }
    );
    m_resources.lights.point.volume = crimild::alloc< SpherePrimitive >(
        SpherePrimitive::Params {
            .type = Primitive::Type::TRIANGLES,
            .radius = 1.0,
            .layout = VertexP3::getLayout(),
        }
    );

    m_resources.lights.spot.pipeline = createGraphicsPipeline(
        device,
        getName() + "/Lights/Spot/Pipeline",
        Light::Type::SPOT,
        getExtent(),
        m_resources.common.renderPass,
        std::vector< VkDescriptorSetLayout > {
            m_resources.common.descriptorSet->getDescriptorSetLayout()->getHandle(),
            m_resources.lights.descriptorSetLayout->getHandle(),
        }
    );
    m_resources.lights.spot.volume = crimild::alloc< SpherePrimitive >(
        SpherePrimitive::Params {
            .type = Primitive::Type::TRIANGLES,
            .radius = 1.0,
            .layout = VertexP3::getLayout(),
        }
    );
}

std::shared_ptr< vulkan::DescriptorSet > RenderSceneLighting::getDirectionalLightDescriptors( const std::shared_ptr< const Light > &light ) noexcept
{
    auto cache = getRenderDevice()->getCache();
    const auto shadowMap = cache->getShadowMap( light );

    Resources::Lights::Directional::UniformData props;
    props.castShadows = light->castShadows() ? 1.0f : 0.0f;
    props.direction = light->getDirection();
    props.color = light->getColor();
    for ( uint32_t layerId = 0; layerId < shadowMap->getLayerCount(); ++layerId ) {
        props.lightSpaceMatrices[ layerId ] = shadowMap->getLightSpaceMatrix( layerId );
        props.splits[ layerId ] = shadowMap->getSplit( layerId );
    }

    if ( !cache->hasUniforms( light ) ) {
        auto uniforms = crimild::alloc< UniformBuffer >( props );
        uniforms->getBufferView()->setUsage( BufferView::Usage::DYNAMIC );
        getRenderDevice()->getCache()->bind( uniforms );
        cache->setUniforms( light, uniforms );
    } else {
        cache->getUniforms( light )->setValue( props );
    }

    if ( !cache->hasDescriptorSet( light ) ) {
        auto descriptors = std::vector< Descriptor > {
            Descriptor {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .buffer = getRenderDevice()->getCache()->bind( cache->getUniforms( light ) ),
                .stage = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            },
            Descriptor {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .imageView = shadowMap->getImageView(),
                .sampler = shadowMap->getSampler(),
                .stage = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            },
        };
        cache->setDescriptorSet(
            light,
            crimild::alloc< DescriptorSet >(
                getRenderDevice(),
                getName() + "/Lights/" + light->getName() + "/DescriptorSet",
                crimild::alloc< vulkan::DescriptorPool >(
                    getRenderDevice(),
                    getName() + "/Lights/" + light->getName() + "/DescriptorPool",
                    descriptors
                ),
                m_resources.lights.descriptorSetLayout,
                descriptors
            )
        );
    }
    return cache->getDescriptorSet( light );
}

std::shared_ptr< vulkan::DescriptorSet > RenderSceneLighting::getPointLightDescriptors( const std::shared_ptr< const Light > &light ) noexcept
{
    auto cache = getRenderDevice()->getCache();
    const auto shadowMap = cache->getShadowMap( light );

    Resources::Lights::Point::UniformData props;
    props.position = light->getPosition();
    props.radius = light->getRadius();
    props.color = light->getColor();
    props.castShadows = light->castShadows() ? 1.0f : 0.0f;

    if ( !cache->hasUniforms( light ) ) {
        auto uniforms = crimild::alloc< UniformBuffer >( props );
        uniforms->getBufferView()->setUsage( BufferView::Usage::DYNAMIC );
        getRenderDevice()->getCache()->bind( uniforms );
        cache->setUniforms( light, uniforms );
    } else {
        cache->getUniforms( light )->setValue( props );
    }

    if ( !cache->hasDescriptorSet( light ) ) {
        auto descriptors = std::vector< Descriptor > {
            Descriptor {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .buffer = getRenderDevice()->getCache()->bind( cache->getUniforms( light ) ),
                .stage = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            },
            Descriptor {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .imageView = shadowMap->getImageView(),
                .sampler = shadowMap->getSampler(),
                .stage = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            },
        };
        cache->setDescriptorSet(
            light,
            crimild::alloc< DescriptorSet >(
                getRenderDevice(),
                getName() + "/Lights/" + light->getName() + "/DescriptorSet",
                crimild::alloc< vulkan::DescriptorPool >(
                    getRenderDevice(),
                    getName() + "/Lights/" + light->getName() + "/DescriptorPool",
                    descriptors
                ),
                m_resources.lights.descriptorSetLayout,
                descriptors
            )
        );
    }
    return cache->getDescriptorSet( light );
}

std::shared_ptr< vulkan::DescriptorSet > RenderSceneLighting::getSpotLightDescriptors( const std::shared_ptr< const Light > &light ) noexcept
{
    auto cache = getRenderDevice()->getCache();
    const auto shadowMap = cache->getShadowMap( light );

    Resources::Lights::Spot::UniformData props;
    props.castShadows = light->castShadows() ? 1.0f : 0.0f;
    props.position = light->getPosition();
    props.direction = light->getDirection();
    props.radius = light->getRadius();
    props.color = light->getColor();
    props.innerCutoff = Numericf::cos( light->getInnerCutoff() );
    props.outerCutoff = Numericf::cos( light->getOuterCutoff() );
    props.lightSpaceProjection = shadowMap->getLightSpaceMatrix( 0 );

    if ( !cache->hasUniforms( light ) ) {
        auto uniforms = crimild::alloc< UniformBuffer >( props );
        uniforms->getBufferView()->setUsage( BufferView::Usage::DYNAMIC );
        getRenderDevice()->getCache()->bind( uniforms );
        cache->setUniforms( light, uniforms );
    } else {
        cache->getUniforms( light )->setValue( props );
    }

    if ( !cache->hasDescriptorSet( light ) ) {
        auto descriptors = std::vector< Descriptor > {
            Descriptor {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .buffer = getRenderDevice()->getCache()->bind( cache->getUniforms( light ) ),
                .stage = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            },
            Descriptor {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .imageView = shadowMap->getImageView(),
                .sampler = shadowMap->getSampler(),
                .stage = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            },
        };
        cache->setDescriptorSet(
            light,
            crimild::alloc< DescriptorSet >(
                getRenderDevice(),
                getName() + "/Lights/" + light->getName() + "/DescriptorSet",
                crimild::alloc< vulkan::DescriptorPool >(
                    getRenderDevice(),
                    getName() + "/Lights/" + light->getName() + "/DescriptorPool",
                    descriptors
                ),
                m_resources.lights.descriptorSetLayout,
                descriptors
            )
        );
    }
    return cache->getDescriptorSet( light );
}

void RenderSceneLighting::render(
    const SceneRenderState &renderState,
    const Camera *camera,
    SyncOptions const &options
) noexcept
{
    // Update camera uniforms
    if ( camera != nullptr ) {
        m_resources.common.uniforms->setValue(
            Resources::Common::UniformData {
                .view = camera->getViewMatrix(),
                .proj = camera->getProjectionMatrix(),
                .viewport = {
                    float( getExtent().width ),
                    float( getExtent().height ),
                },
            }
        );
    }

    auto &cmds = getCommandBuffer();
    cmds->reset();

    cmds->begin( options );
    cmds->beginRenderPass( m_resources.common.renderPass, m_resources.common.framebuffer );

    // Directional lighting
    cmds->bindPipeline( m_resources.lights.directional.pipeline );
    cmds->bindDescriptorSet( 0, m_resources.common.descriptorSet );
    for ( const auto &light : renderState.lights.at( Light::Type::DIRECTIONAL ) ) {
        auto lightDescriptors = getDirectionalLightDescriptors( light );
        cmds->bindDescriptorSet( 1, lightDescriptors );
        cmds->draw( 6 );
    }

    // Point lighting
    // TODO: Use instancing to render all lights in one call
    cmds->bindPipeline( m_resources.lights.point.pipeline );
    cmds->bindDescriptorSet( 0, m_resources.common.descriptorSet );
    for ( const auto &light : renderState.lights.at( Light::Type::POINT ) ) {
        auto lightDescriptors = getPointLightDescriptors( light );
        cmds->bindDescriptorSet( 1, lightDescriptors );
        cmds->drawPrimitive( m_resources.lights.point.volume );
    }

    // Spot lighting
    // TODO: Use instancing to render all lights in one call
    cmds->bindPipeline( m_resources.lights.spot.pipeline );
    cmds->bindDescriptorSet( 0, m_resources.common.descriptorSet );
    for ( const auto &light : renderState.lights.at( Light::Type::SPOT ) ) {
        auto lightDescriptors = getSpotLightDescriptors( light );
        cmds->bindDescriptorSet( 1, lightDescriptors );
        cmds->drawPrimitive( m_resources.lights.spot.volume );
    }

    cmds->endRenderPass();
    cmds->end( options );

    getRenderDevice()->submitGraphicsCommands( cmds );
}
