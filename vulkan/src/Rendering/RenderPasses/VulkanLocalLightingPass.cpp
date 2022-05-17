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

LocalLightingPass::LocalLightingPass(
    RenderDevice *renderDevice,
    const FramebufferAttachment *albedoInput,
    const FramebufferAttachment *positionInput,
    const FramebufferAttachment *normalInput,
    const FramebufferAttachment *materialInput,
    const FramebufferAttachment *shadowInput ) noexcept
    : RenderPassBase( renderDevice ),
      m_albedoInput( albedoInput ),
      m_positionInput( positionInput ),
      m_normalInput( normalInput ),
      m_materialInput( materialInput ),
      m_shadowInput( shadowInput )
{
    m_renderArea = VkRect2D {
        .offset = {
            0,
            0,
        },
        .extent = {
            .width = 1024,
            .height = 1024,
        },
    };

    m_lightVolume = std::make_unique< SpherePrimitive >(
        SpherePrimitive::Params {
            .type = Primitive::Type::TRIANGLES,
            .radius = 1.0,
            .layout = VertexP3::getLayout(),
        } );

    init();
}

LocalLightingPass::~LocalLightingPass( void ) noexcept
{
    clear();
}

Event LocalLightingPass::handle( const Event &e ) noexcept
{
    switch ( e.type ) {
        case Event::Type::WINDOW_RESIZE: {
            m_renderArea.extent = {
                .width = uint32_t( e.extent.width ),
                .height = uint32_t( e.extent.height ),
            };
            clear();
            init();
            break;
        }

        default:
            break;
    }

    return e;
}

void LocalLightingPass::render( void ) noexcept
{
    auto scene = Simulation::getInstance()->getScene();
    if ( scene == nullptr ) {
        return;
    }

    FetchLights fetchLights;
    scene->perform( fetchLights );

    auto camera = Camera::getMainCamera();
    if ( m_renderPassObjects.uniforms != nullptr ) {
        m_renderPassObjects.uniforms->setValue(
            RenderPassObjects::Uniforms {
                .view = camera->getViewMatrix(),
                .proj = camera->getProjectionMatrix(),
                .viewport = Vector2 {
                    Real( m_renderArea.extent.width ),
                    Real( m_renderArea.extent.height ),
                } } );
        getRenderDevice()->update( m_renderPassObjects.uniforms.get() );
    }

    const auto currentFrameIndex = getRenderDevice()->getCurrentFrameIndex();
    auto commandBuffer = getRenderDevice()->getCurrentCommandBuffer();

    const auto clearValues = std::array< VkClearValue, 1 > {
        VkClearValue {
            .color = {
                .float32 = {
                    0.0f,
                    0.0f,
                    0.0f,
                    // Set alpha to 1 since the resulting image should not be transparent.
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

    fetchLights.forEachLight(
        [ & ]( auto light ) {
            if ( light->getType() == Light::Type::DIRECTIONAL ) {
                vkCmdBindPipeline(
                    commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    m_directionalLightPipeline->getHandle() );
                vkCmdBindDescriptorSets( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_directionalLightPipeline->getPipelineLayout(), 0, 1, &m_renderPassObjects.descriptorSets[ currentFrameIndex ], 0, nullptr );
                bindLightDescriptors( commandBuffer, m_directionalLightPipeline->getPipelineLayout(), currentFrameIndex, light );
                vkCmdDraw( commandBuffer, 6, 1, 0, 0 );
            } else {
                // TODO: Use instancing to render all lights in one call
                vkCmdBindPipeline(
                    commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    m_pointLightPipeline->getHandle() );
                vkCmdBindDescriptorSets( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pointLightPipeline->getPipelineLayout(), 0, 1, &m_renderPassObjects.descriptorSets[ currentFrameIndex ], 0, nullptr );
                bindLightDescriptors( commandBuffer, m_directionalLightPipeline->getPipelineLayout(), currentFrameIndex, light );
                drawPrimitive( commandBuffer, currentFrameIndex, m_lightVolume.get() );
            }
        } );

    vkCmdEndRenderPass( commandBuffer );
}

void LocalLightingPass::init( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    const auto extent = m_renderArea.extent;

    createFramebufferAttachment( "Scene/Lighting", extent, VK_FORMAT_R32G32B32A32_SFLOAT, m_colorAttachment );

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
            &m_renderPass ) );

    m_framebuffers.resize( getRenderDevice()->getSwapchainImageViews().size() );
    for ( uint8_t i = 0; i < m_framebuffers.size(); ++i ) {
        auto attachments = std::array< VkImageView, 1 > {
            m_colorAttachment.imageView,
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
                &m_framebuffers[ i ] ) );
    }

    createRenderPassObjects();
    createLightObjects();

    auto createPipeline = [ & ]( auto lightType ) {
        auto program = crimild::alloc< ShaderProgram >();
        program->setShaders(
            Array< SharedPointer< Shader > > {
                crimild::alloc< Shader >(
                    Shader::Stage::VERTEX,
                    lightType == Light::Type::DIRECTIONAL
                        ? R"(
                                vec2 positions[6] = vec2[](
                                    vec2( -1.0, 1.0 ),
                                    vec2( -1.0, -1.0 ),
                                    vec2( 1.0, -1.0 ),

                                    vec2( -1.0, 1.0 ),
                                    vec2( 1.0, -1.0 ),
                                    vec2( 1.0, 1.0 )
                                );

                                vec2 texCoords[6] = vec2[](
                                    vec2( 0.0, 0.0 ),
                                    vec2( 0.0, 1.0 ),
                                    vec2( 1.0, 1.0 ),

                                    vec2( 0.0, 0.0 ),
                                    vec2( 1.0, 1.0 ),
                                    vec2( 1.0, 0.0 )
                                );

                                void main()
                                {
                                    gl_Position = vec4( positions[ gl_VertexIndex ], 0.0, 1.0 );
                                }
                                  )"
                        : R"(
                                layout ( location = 0 ) in vec3 inPosition;

                                layout( set = 0, binding = 0 ) uniform RenderPassUniforms {
                                    mat4 view;
                                    mat4 proj;
                                    vec2 viewport;
                                };

                                layout ( set = 1, binding = 0 ) uniform LightProps {
                                    uint type;
                                    vec4 position;
                                    vec4 direction;
                                    vec4 ambient;
                                    vec4 color;
                                    vec4 attenuation;
                                    vec4 cutoff;
                                    bool castShadows;
                                    float shadowBias;
                                    vec4 cascadeSplitsd;
                                    mat4 lightSpaceMatrix[ 4 ];
                                    vec4 viewport;
                                    float energy;
                                    float radius;
                                } uLight;

                                vec2 positions[6] = vec2[](
                                    vec2( -1.0, 1.0 ),
                                    vec2( -1.0, -1.0 ),
                                    vec2( 1.0, -1.0 ),

                                    vec2( -1.0, 1.0 ),
                                    vec2( 1.0, -1.0 ),
                                    vec2( 1.0, 1.0 )
                                );

                                vec2 texCoords[6] = vec2[](
                                    vec2( 0.0, 0.0 ),
                                    vec2( 0.0, 1.0 ),
                                    vec2( 1.0, 1.0 ),

                                    vec2( 0.0, 0.0 ),
                                    vec2( 1.0, 1.0 ),
                                    vec2( 1.0, 0.0 )
                                );

                                void main()
                                {
                                    gl_Position = proj * view * vec4( ( uLight.position.xyz + uLight.radius * inPosition ), 1.0 );
                                }
                            )" ),
                crimild::alloc< Shader >(
                    Shader::Stage::FRAGMENT,
                    R"(
                                #include <textureCube>

                                layout( set = 0, binding = 0 ) uniform RenderPassUniforms {
                                    mat4 view;
                                    mat4 proj;
                                    vec2 viewport;
                                };

                                layout ( set = 0, binding = 1 ) uniform sampler2D uAlbedo;
                                layout ( set = 0, binding = 2 ) uniform sampler2D uPositions;
                                layout ( set = 0, binding = 3 ) uniform sampler2D uNormals;
                                layout ( set = 0, binding = 4 ) uniform sampler2D uMaterials;
                                layout ( set = 0, binding = 5 ) uniform sampler2D uShadowAtlas;

                                layout ( set = 1, binding = 0 ) uniform LightProps {
                                    uint type;
                                    vec4 position;
                                    vec4 direction;
                                    vec4 ambient;
                                    vec4 color;
                                    vec4 attenuation;
                                    vec4 cutoff;
                                    uint castShadows;
                                    float shadowBias;
                                    vec4 cascadeSplits;
                                    mat4 lightSpaceMatrix[ 4 ];
                                    vec4 viewport;
                                    float energy;
                                    float radius;
                                } uLight;

                                layout ( location = 0 ) out vec4 outColor;

                                const float PI = 3.14159265359;

                                const mat4 bias = mat4(
                                    0.5, 0.0, 0.0, 0.0,
                                    0.0, 0.5, 0.0, 0.0,
                                    0.0, 0.0, 1.0, 0.0,
                                    0.5, 0.5, 0.0, 1.0
                                );

                                float distributionGGX( vec3 N, vec3 H, float roughness )
                                {
                                    float a = roughness * roughness;
                                    float a2 = a * a;
                                    float NdotH = max( dot( N, H ), 0.0 );
                                    float NdotH2 = NdotH * NdotH;

                                    float num = a2;
                                    float denom = ( NdotH2 * ( a2 - 1.0 ) + 1.0 );
                                    denom = PI * denom * denom;

                                    return num / denom;
                                }

                                float geometrySchlickGGX( float NdotV, float roughness )
                                {
                                    float r = ( roughness + 1.0 );
                                    float k = ( r * r ) / 8.0;
                                    float num = NdotV;
                                    float denom = NdotV * ( 1.0 - k ) + k;
                                    return num / denom;
                                }

                                float geometrySmith( vec3 N, vec3 V, vec3 L, float roughness )
                                {
                                    float NdotV = max( dot( N, V ), 0.0 );
                                    float NdotL = max( dot( N, L ), 0.0 );
                                    float ggx1 = geometrySchlickGGX( NdotL, roughness );
                                    float ggx2 = geometrySchlickGGX( NdotV, roughness );
                                    return ggx1 * ggx2;
                                }

                                vec3 fresnelSchlick( float cosTheta, vec3 F0 )
                                {
                                    return F0 + ( 1.0 - F0 ) * pow( 1.0 - cosTheta, 5.0 );
                                }

                                vec3 fresnelSchlickRoughness( float cosTheta, vec3 F0, float roughness )
                                {
                                    return F0 + ( max( vec3( 1.0 - roughness ), F0 ) - F0 ) * pow( 1.0 - cosTheta, 5.0 );
                                }

                                vec3 computeF0( vec3 albedo, float metallic )
                                {
                                    vec3 F0 = vec3( 0.04 );
                                    F0 = mix( F0, albedo, metallic );
                                    return F0;
                                }

                                vec3 brdf( vec3 N, vec3 H, vec3 V, vec3 L, vec3 radiance, vec3 albedo, float metallic, float roughness )
                                {
                                    vec3 F0 = computeF0( albedo, metallic );

                                    float NDF = distributionGGX( N, H, roughness );
                                    float G = geometrySmith( N, V, L, roughness );
                                    vec3 F = fresnelSchlick( max( dot( H, V ), 0.0 ), F0 );

                                    vec3 kS = F;
                                    vec3 kD = vec3( 1.0 ) - kS;
                                    kD *= 1.0 - metallic;

                                    vec3 numerator = NDF * G * F;
                                    float denominator = 4.0 * max( dot( N, V ), 0.0 ) * max( dot( N, L ), 0.0 );
                                    vec3 specular = numerator / max( denominator, 0.001 );

                                    // add outgoing radiance
                                    float NdotL = max( dot( N, L ), 0.0 );
                                    return ( kD * albedo / PI + specular ) * radiance * NdotL;
                                }

                                float calculateShadow( vec4 ligthSpacePosition, vec4 viewport, vec3 N, vec3 L, float bias )
                                {
                                    // perform perspective divide
                                    vec3 projCoords = ligthSpacePosition.xyz / ligthSpacePosition.w;
                                    projCoords.t = 1.0 - projCoords.t;

                                    // Compute shadow PCF
                                    float depth = projCoords.z;
                                    float shadow = 0.0f;
                                    vec2 texelSize = 1.0 / textureSize( uShadowAtlas, 0 );
                                    for ( int y = -1; y <= 1; ++y ) {
                                        for ( int x = -1; x <= 1; ++x ) {
                                            vec2 uv = projCoords.st + vec2( x, y ) * texelSize;
                                            uv.x = viewport.x + uv.x * viewport.z;
                                            uv.y = viewport.y + uv.y * viewport.w;
                                            float pcfDepth = texture( uShadowAtlas, uv ).x;
                                            shadow += depth - bias > pcfDepth ? 1.0 : 0.0;
                                        }
                                    }
                                    shadow /= 9.0;

                                    return shadow;
                                }

                                float calculateDirectionalShadow( vec4 lightSpacePosition, vec3 N, vec3 L, float bias )
                                {
                                    vec4 P = lightSpacePosition / lightSpacePosition.w;
                                    // P.t = 1.0 - P.t;
                                    // P.st = 1.0 - P.st;

                                    float shadow = 1.0;
                                    if ( P.z > -1.0 && P.z < 1.0 ) {
                                        float dist = texture( uShadowAtlas, P.st ).r;
                                        if ( P.w > 0.0 && dist < P.z ) {
                                            // Not in shadow
                                            shadow = 0.0;
                                        }
                                    }
                                    return shadow;
                                }

                                float calculatePointShadow( sampler2D shadowAtlas, float dist, vec3 D, vec4 viewport, float bias )
                                {
                                    float depth = dist;
                                    float shadow = textureCubeUV( shadowAtlas, D, viewport ).r;
                                    return depth <= ( shadow + bias ) ? 0.0 : 0.85;
                                }

                                void main()
                                {
                                    vec2 uv = gl_FragCoord.st / viewport;

                                    vec4 baseColor = texture( uAlbedo, uv );
                                    if ( baseColor.a == 0 ) {
                                        // nothing to render. discard and avoid complex calculations
                                        discard;
                                    }
                                    vec3 albedo = baseColor.rgb;

                                    vec4 positionData = texture( uPositions, uv );

                                    vec3 P = positionData.xyz;
                                    float viewSpacePositionDepth = positionData.w;
                                    vec3 N = texture( uNormals, uv ).xyz;
                                    vec4 material = texture( uMaterials, uv );
                                    float metallic = material.x;
                                    float roughness = material.y;
                                    float ambientOcclusion = material.z;

                                    vec3 E = inverse( view )[ 3 ].xyz;
                                    vec3 V = normalize( E - P );

                                    vec3 F0 = vec3( 0.04 );
                                    F0 = mix( F0, albedo, metallic );

                                    vec3 L = vec3( 0 );
                                    vec3 radiance = vec3( 0 );
                                    float shadow = 0.0;

                                    vec3 debugColor = vec3( 1 );

                                    if ( uLight.type == 2 ) {
                                        // directional
                                        L = normalize( -uLight.direction.xyz );
                                        radiance = uLight.energy * uLight.color.rgb;

                                        if ( uLight.castShadows == 1 ) {
                                            // vec4 cascadeSplits = uLight.cascadeSplits;

                                            // these are all negative values. Lower means farther away from the eye
                                            float depth = viewSpacePositionDepth;
                                            int cascadeId = 0;

                                            // if ( depth < cascadeSplits[ 0 ] ) {
                                            //     cascadeId = 1;
                                            // }

                                            // if ( depth < cascadeSplits[ 1 ] ) {
                                            //     cascadeId = 2;
                                            // }

                                            // if ( depth < cascadeSplits[ 2 ] ) {
                                            //     cascadeId = 3;
                                            // }

                                            mat4 lightSpaceMatrix = uLight.lightSpaceMatrix[ cascadeId ];
                                            // vec4 viewport = uLight.viewport;
                                            // vec2 viewportSize = viewport.zw;
                                            // viewport.z = 0.5 * viewportSize.x;
                                            // viewport.w = 0.5 * viewportSize.y;
                                            // if ( cascadeId == 1 ) {
                                            //     // top-right cascade
                                            //     viewport.x += 0.5 * viewportSize.x;
                                            // } else if ( cascadeId == 2 ) {
                                            //     // bottom-left cascade
                                            //     viewport.y += 0.5 * viewportSize.y;
                                            // } else if ( cascadeId == 3 ) {
                                            //     // bottom-rigth cascade
                                            //     viewport.x += 0.5 * viewportSize.x;
                                            //     viewport.y += 0.5 * viewportSize.y;
                                            // }

                                            {
                                                vec4 lightSpacePos = ( bias * lightSpaceMatrix * vec4( P, 1.0 ) );
                                                lightSpacePos = lightSpacePos / lightSpacePos.w;
                                                // lightSpacePos.t = 1.0 - lightSpacePos.t;

                                                shadow = 0.0;
                                                if ( lightSpacePos.z > -1.0 && lightSpacePos.z < 1.0 ) {
                                                    float dist = texture( uShadowAtlas, lightSpacePos.st ).r;
                                                    if ( lightSpacePos.w > 0.0 && dist < lightSpacePos.z ) {
                                                        // In shadow
                                                        shadow = 1.0;
                                                    }
                                                }
                                            }


                                            // if ( true || lightSpacePos.z >= -1.0 && lightSpacePos.z <= 1.0 ) {
                                            //     // vec4 viewport = vec4(0, 0, 1, 1 );
                                            //     // shadow = calculateShadow( lightSpacePos, viewport, N, L, 0.005 );
                                            //     //shadow = calculateDirectionalShadow( lightSpacePos, N, L, 0.005 );
                                            //     shadow = 1.0;
                                            // } else {
                                            //     // the projected position in light space is outside the light's view frustum.
                                            //     // For directional lights, this means the object will be never in shadows
                                            //     // This might not be true for other types of lights. For example, a spot light
                                            //     // will produce shadows on objects outside its cone of influence
                                            //     shadow = 0.5;
                                            // }
                                        }
                                    } else if ( uLight.type == 3 ) {
                                        // spot
                                        L = uLight.position.xyz - P;
                                        float dist = length( L );
                                        L = normalize( L );

                                        float theta = dot( L, normalize( -uLight.direction.xyz ) );
                                        float epsilon = uLight.cutoff.x - uLight.cutoff.y;
                                        float intensity = clamp( ( theta - uLight.cutoff.y ) / epsilon, 0.0, 1.0 );
                                        float attenuation = 1.0 / ( dist * dist );
                                        radiance = attenuation * intensity * uLight.energy * uLight.color.rgb;

                                        if ( uLight.castShadows == 1 ) {
                                            vec4 lightSpacePos = ( bias * uLight.lightSpaceMatrix[ 0 ] * vec4( P, 1.0 ) );
                                            shadow = calculateShadow( lightSpacePos, uLight.viewport, N, L, 0.005 );
                                        }
                                    } else {
                                        L = uLight.position.xyz - P;
                                        float dist = length( L );
                                        L = normalize( L );
                                        float attenuation = 1.0 / ( dist * dist );
                                        radiance = uLight.color.rgb * uLight.energy * attenuation;

                                        debugColor = vec3(dist / 100.0);

                                        if ( uLight.castShadows == 1 ) {
                                            shadow = calculatePointShadow(
                                                uShadowAtlas,
                                                dist,
                                                -L,
                                                uLight.viewport,
                                                uLight.shadowBias
                                            );

                                            //shadow = textureCubeUV( uShadowAtlas, -L, uLight.viewport ).r;
                                            //debugColor = vec3(shadow / 100.0);
                                        }
                                    }

                                    vec3 H = normalize( V + L );
                                    vec3 Lo = brdf( N, H, V, L, radiance, albedo, metallic, roughness );

                                    Lo *= 1.0 - shadow;
                                    //Lo = debugColor;

                                    outColor = vec4( Lo, 1.0 );
                                }
                            )" ),
            } );

        const auto viewport = ViewportDimensions::fromExtent( m_renderArea.extent.width, m_renderArea.extent.height );

        const auto vertexLayouts = lightType != Light::Type::DIRECTIONAL
                                       ? std::vector< VertexLayout > { VertexP3::getLayout() }
                                       : std::vector< VertexLayout > {};
        return std::make_unique< GraphicsPipeline >(
            getRenderDevice(),
            m_renderPass,
            GraphicsPipeline::Descriptor {
                .descriptorSetLayouts = std::vector< VkDescriptorSetLayout > {
                    m_renderPassObjects.layout,
                    m_lightObjects.descriptorSetLayout,
                },
                .program = program.get(),
                .vertexLayouts = vertexLayouts,
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
                .colorAttachmentCount = colorReferences.size(),
                .viewport = viewport,
                .scissor = viewport,
            } );
    };

    m_pointLightPipeline = createPipeline( Light::Type::POINT );
    m_directionalLightPipeline = createPipeline( Light::Type::DIRECTIONAL );
}

void LocalLightingPass::clear( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDeviceWaitIdle( getRenderDevice()->getHandle() );

    m_pointLightPipeline = nullptr;
    m_directionalLightPipeline = nullptr;

    destroyLightObjects();
    destroyRenderPassObjects();

    for ( auto &fb : m_framebuffers ) {
        vkDestroyFramebuffer( getRenderDevice()->getHandle(), fb, nullptr );
    }
    m_framebuffers.clear();

    destroyFramebufferAttachment( m_colorAttachment );

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

    const auto poolSizes = std::array< VkDescriptorPoolSize, 6 > {
        VkDescriptorPoolSize {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
        },
        VkDescriptorPoolSize {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
        },
        VkDescriptorPoolSize {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
        },
        VkDescriptorPoolSize {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
        },
        VkDescriptorPoolSize {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
        },
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

    const auto bindings = std::array< VkDescriptorSetLayoutBinding, 6 > {
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
        VkDescriptorSetLayoutBinding {
            .binding = 2,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
        },
        VkDescriptorSetLayoutBinding {
            .binding = 3,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
        },
        VkDescriptorSetLayoutBinding {
            .binding = 4,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
        },
        VkDescriptorSetLayoutBinding {
            .binding = 5,
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
        const auto bufferInfo = VkDescriptorBufferInfo {
            .buffer = getRenderDevice()->getHandle( m_renderPassObjects.uniforms.get(), i ),
            .offset = 0,
            .range = m_renderPassObjects.uniforms->getBufferView()->getLength(),
        };

        const auto albedoImageInfo = VkDescriptorImageInfo {
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .imageView = m_albedoInput->imageView,
            .sampler = m_albedoInput->sampler,
        };

        const auto positionImageInfo = VkDescriptorImageInfo {
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .imageView = m_positionInput->imageView,
            .sampler = m_positionInput->sampler,
        };

        const auto normalImageInfo = VkDescriptorImageInfo {
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .imageView = m_normalInput->imageView,
            .sampler = m_normalInput->sampler,
        };

        const auto materialImageInfo = VkDescriptorImageInfo {
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .imageView = m_materialInput->imageView,
            .sampler = m_materialInput->sampler,
        };

        const auto shadowImageInfo = VkDescriptorImageInfo {
            .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
            .imageView = m_shadowInput->imageView,
            .sampler = m_shadowInput->sampler,
        };

        const auto writes = std::array< VkWriteDescriptorSet, 6 > {
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
            VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = m_renderPassObjects.descriptorSets[ i ],
                .dstBinding = 1,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .pBufferInfo = nullptr,
                .pImageInfo = &albedoImageInfo,
                .pTexelBufferView = nullptr,
            },
            VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = m_renderPassObjects.descriptorSets[ i ],
                .dstBinding = 2,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .pBufferInfo = nullptr,
                .pImageInfo = &positionImageInfo,
                .pTexelBufferView = nullptr,
            },
            VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = m_renderPassObjects.descriptorSets[ i ],
                .dstBinding = 3,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .pBufferInfo = nullptr,
                .pImageInfo = &normalImageInfo,
                .pTexelBufferView = nullptr,
            },
            VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = m_renderPassObjects.descriptorSets[ i ],
                .dstBinding = 4,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .pBufferInfo = nullptr,
                .pImageInfo = &materialImageInfo,
                .pTexelBufferView = nullptr,
            },
            VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = m_renderPassObjects.descriptorSets[ i ],
                .dstBinding = 5,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .pBufferInfo = nullptr,
                .pImageInfo = &shadowImageInfo,
                .pTexelBufferView = nullptr,
            },
        };
        vkUpdateDescriptorSets( getRenderDevice()->getHandle(), writes.size(), writes.data(), 0, nullptr );
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

void LocalLightingPass::createLightObjects( void ) noexcept
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

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorSetLayout( getRenderDevice()->getHandle(), &layoutCreateInfo, nullptr, &m_lightObjects.descriptorSetLayout ) );
}

void LocalLightingPass::bindLightDescriptors( VkCommandBuffer cmds, VkPipelineLayout pipelineLayout, Index currentFrameIndex, Light *light ) noexcept
{
    struct LightProps {
        alignas( 4 ) UInt32 type;
        alignas( 16 ) Vector4f position;
        alignas( 16 ) Vector4f direction;
        alignas( 16 ) ColorRGBA ambient;
        alignas( 16 ) ColorRGBA color;
        alignas( 16 ) Vector4f attenuation;
        alignas( 16 ) Vector4f cutoff;
        alignas( 4 ) UInt32 castShadows;
        alignas( 4 ) Real32 shadowBias;
        alignas( 16 ) Vector4f cascadeSplits;
        alignas( 16 ) Matrix4f lightSpaceMatrix[ 4 ];
        alignas( 16 ) Vector4f viewport;
        alignas( 4 ) Real32 energy;
        alignas( 4 ) Real32 radius;
    };

    if ( !m_lightObjects.descriptorSets.contains( light ) ) {
        VkDescriptorPoolSize poolSize {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
        };

        auto poolCreateInfo = VkDescriptorPoolCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .poolSizeCount = 1,
            .pPoolSizes = &poolSize,
            .maxSets = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
        };

        CRIMILD_VULKAN_CHECK( vkCreateDescriptorPool( getRenderDevice()->getHandle(), &poolCreateInfo, nullptr, &m_lightObjects.descriptorPools[ light ] ) );

        m_lightObjects.uniforms[ light ] = std::make_unique< UniformBuffer >( LightProps {} );
        getRenderDevice()->bind( m_lightObjects.uniforms[ light ].get() );

        std::vector< VkDescriptorSetLayout > layouts( getRenderDevice()->getSwapchainImageCount(), m_lightObjects.descriptorSetLayout );

        const auto allocInfo = VkDescriptorSetAllocateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = m_lightObjects.descriptorPools[ light ],
            .descriptorSetCount = uint32_t( layouts.size() ),
            .pSetLayouts = layouts.data(),
        };

        m_lightObjects.descriptorSets[ light ].resize( getRenderDevice()->getSwapchainImageCount() );
        CRIMILD_VULKAN_CHECK( vkAllocateDescriptorSets( getRenderDevice()->getHandle(), &allocInfo, m_lightObjects.descriptorSets[ light ].data() ) );

        for ( size_t i = 0; i < m_lightObjects.descriptorSets[ light ].size(); ++i ) {
            const auto bufferInfo = VkDescriptorBufferInfo {
                .buffer = getRenderDevice()->getHandle( m_lightObjects.uniforms[ light ].get(), i ),
                .offset = 0,
                .range = m_lightObjects.uniforms[ light ]->getBufferView()->getLength(),
            };

            const auto descriptorWrite = VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = m_lightObjects.descriptorSets[ light ][ i ],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .pBufferInfo = &bufferInfo,
                .pImageInfo = nullptr,
                .pTexelBufferView = nullptr,
            };

            vkUpdateDescriptorSets( getRenderDevice()->getHandle(), 1, &descriptorWrite, 0, nullptr );
        }
    }

    auto props = LightProps {};
    props.type = static_cast< UInt32 >( light->getType() );
    props.position = vector4( light->getPosition(), Real( 1 ) );
    props.direction = vector4( light->getDirection(), Real( 0 ) );
    props.color = light->getColor();
    props.attenuation = vector4( light->getAttenuation(), Real( 0 ) );
    props.ambient = light->getAmbient();
    props.cutoff = Vector4f {
        Numericf::cos( light->getInnerCutoff() ),
        Numericf::cos( light->getOuterCutoff() ),
        0.0f,
        0.0f,
    };
    props.castShadows = light->castShadows() ? 1 : 0;
    if ( light->castShadows() ) {
        props.shadowBias = light->getShadowMap()->getBias();
        props.cascadeSplits = light->getShadowMap()->getCascadeSplits();
        for ( auto split = 0; split < 4; ++split ) {
            props.lightSpaceMatrix[ split ] = light->getShadowMap()->getLightProjectionMatrix( split );
        }
        props.viewport = light->getShadowMap()->getViewport();
    }
    props.energy = light->getEnergy();
    props.radius = light->getRadius();

    m_lightObjects.uniforms[ light ]->setValue( props );
    getRenderDevice()->update( m_lightObjects.uniforms[ light ].get() );

    vkCmdBindDescriptorSets(
        cmds,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout,
        1,
        1,
        &m_lightObjects.descriptorSets[ light ][ currentFrameIndex ],
        0,
        nullptr );
}

void LocalLightingPass::destroyLightObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    // no need to destroy sets
    m_lightObjects.descriptorSets.clear();

    vkDestroyDescriptorSetLayout( getRenderDevice()->getHandle(), m_lightObjects.descriptorSetLayout, nullptr );
    m_lightObjects.descriptorSetLayout = VK_NULL_HANDLE;

    for ( auto &it : m_lightObjects.descriptorPools ) {
        vkDestroyDescriptorPool( getRenderDevice()->getHandle(), it.second, nullptr );
    }
    m_lightObjects.descriptorPools.clear();

    for ( auto &it : m_lightObjects.uniforms ) {
        getRenderDevice()->unbind( it.second.get() );
    }
    m_lightObjects.uniforms.clear();
}

void LocalLightingPass::drawPrimitive( VkCommandBuffer cmds, Index currentFrameIndex, Primitive *primitive ) noexcept
{
    primitive->getVertexData().each(
        [ &, i = 0 ]( auto &vertices ) mutable {
            if ( vertices != nullptr ) {
                VkBuffer buffers[] = { getRenderDevice()->bind( vertices.get() ) };
                VkDeviceSize offsets[] = { 0 };
                vkCmdBindVertexBuffers( cmds, i, 1, buffers, offsets );
            }
        } );

    auto indices = primitive->getIndices();

    vkCmdBindIndexBuffer(
        cmds,
        getRenderDevice()->bind( indices ),
        0,
        utils::getIndexType( crimild::get_ptr( indices ) ) );

    vkCmdDrawIndexed( cmds, indices->getIndexCount(), 1, 0, 0, 0 );
}
