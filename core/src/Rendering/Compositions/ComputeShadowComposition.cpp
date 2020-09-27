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

#include "Rendering/Compositions/ComputeShadowComposition.hpp"

#include "Rendering/CommandBuffer.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/ShadowMap.hpp"
#include "Rendering/Uniforms/CallbackUniformBuffer.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Geometry.hpp"
#include "Visitors/ApplyToGeometries.hpp"
#include "Visitors/FetchLights.hpp"

using namespace crimild;
using namespace crimild::compositions;

static Pipeline *createPipeline( Composition &cmp, Light::Type lightType ) noexcept
{
    auto pipeline = cmp.create< Pipeline >();
    pipeline->program = [ & ] {
        auto program = crimild::alloc< ShaderProgram >(
            Array< SharedPointer< Shader > > {
                crimild::alloc< Shader >(
                    Shader::Stage::VERTEX,
                    R"(
                        layout ( location = 0 ) in vec3 inPosition;
                        layout ( location = 1 ) in vec3 inNormal;
                        layout ( location = 2 ) in vec2 inTextureCoord;

                        layout ( set = 0, binding = 0 ) uniform LightUniforms {
                            mat4 lightSpaceMatrix;
                        };

                        layout ( set = 1, binding = 0 ) uniform GeometryUniforms {
                            mat4 model;
                        };

                        layout ( location = 0 ) out vec3 outPosition;
                        layout ( location = 1 ) out vec3 outLightPos;

                        void main()
                        {
                            gl_Position = lightSpaceMatrix * model * vec4( inPosition, 1.0 );

                            outPosition = ( model * vec4( inPosition, 1.0 ) ).xyz;

                            mat4 invView = inverse( lightSpaceMatrix );
                            outLightPos = vec3( invView[ 3 ].x, invView[ 3 ].y, invView[ 3 ].z );
                        }
                    )" ),
                crimild::alloc< Shader >(
                    Shader::Stage::FRAGMENT,
                    lightType == Light::Type::POINT
                        ? R"(
                            layout ( location = 0 ) in vec3 inPosition;
                            layout ( location = 1 ) in vec3 inLightPos;

                            layout ( location = 0 ) out vec4 fragColor;

                            void main()
                            {
                                float d = length( inPosition - inLightPos );
                                d /= 200.0;
                                fragColor = vec4( vec3( d ), 1.0 );
                                gl_FragDepth = d;
                            }
                        )"
                        : R"(
                            layout ( location = 0 ) out vec4 fragColor;

                            float linearizeDepth(float depth, float nearPlane, float farPlane)
                            {
                                float z = depth * 2.0 - 1.0; // Back to NDC
                                return ( 2.0 * nearPlane * farPlane ) / ( farPlane + nearPlane - z * ( farPlane - nearPlane ) ) / farPlane;
                            }

                            void main()
                            {
                                float d = gl_FragCoord.z;
                                d = linearizeDepth( d, 1.0, 200.0 );
                                fragColor = vec4( vec3( d ), 1.0 );
                            }
                        )" ),
            } );
        program->vertexLayouts = { VertexLayout::P3_N3_TC2 };
        program->descriptorSetLayouts = {
            [] {
                auto layout = crimild::alloc< DescriptorSetLayout >();
                layout->bindings = {
                    {
                        .descriptorType = DescriptorType::UNIFORM_BUFFER,
                        .stage = Shader::Stage::VERTEX,
                    },
                };
                return layout;
            }(),
            [] {
                auto layout = crimild::alloc< DescriptorSetLayout >();
                layout->bindings = {
                    {
                        .descriptorType = DescriptorType::UNIFORM_BUFFER,
                        .stage = Shader::Stage::VERTEX,
                    },
                };
                return layout;
            }(),
        };
        return program;
    }();
    pipeline->viewport = { .scalingMode = ScalingMode::DYNAMIC };
    pipeline->scissor = { .scalingMode = ScalingMode::DYNAMIC };
    pipeline->rasterizationState = RasterizationState {
        .cullMode = CullMode::FRONT,

        // It would be ideal to support this feature for directional lights
        // but it seems it doesn't work...
        //.depthClampEnable = true,
    };

    return pipeline;
}

size_t recordPointLightCommands( Composition &cmp, CommandBuffer *commandBuffer, Pipeline *pipeline, Array< ViewportDimensions > &layout, size_t offset, Array< Light * > lights, Node *scene ) noexcept
{
    lights.each(
        [ & ]( auto light ) {
            if ( offset >= layout.size() ) {
                CRIMILD_LOG_WARNING( "No available viewport in layout in shadow atlas for spot light" );
                return;
            }

            auto layoutViewport = layout[ offset++ ];

            light->getShadowMap()->setViewport(
                [ & ] {
                    auto rect = layoutViewport.dimensions;
                    return Vector4f(
                        rect.getX(),
                        rect.getY(),
                        rect.getWidth(),
                        rect.getHeight() );
                }() );

            auto transformViewport = []( auto layout, auto viewport ) {
                auto ld = layout.dimensions;
                auto vd = viewport.dimensions;
                return ViewportDimensions {
                    .scalingMode = ScalingMode::RELATIVE,
                    .dimensions = Rectf(
                        ld.getX() + vd.getX() * ld.getWidth(),
                        ld.getY() + vd.getY() * ld.getHeight(),
                        ld.getWidth() * vd.getWidth(),
                        ld.getHeight() * vd.getHeight() ),
                };
            };

            ViewportDimensions viewports[ 6 ] = {
                transformViewport(
                    layoutViewport,
                    ViewportDimensions {
                        .scalingMode = ScalingMode::RELATIVE,
                        .dimensions = Rectf( 0.0f, 0.5f, 0.25f, 0.25f ),
                    } ),
                transformViewport(
                    layoutViewport,
                    ViewportDimensions {
                        .scalingMode = ScalingMode::RELATIVE,
                        .dimensions = Rectf( 0.5f, 0.5f, 0.25f, 0.25f ),
                    } ),
                transformViewport(
                    layoutViewport,
                    ViewportDimensions {
                        .scalingMode = ScalingMode::RELATIVE,
                        .dimensions = Rectf( 0.5f, 0.25f, 0.25f, 0.25f ),
                    } ),
                transformViewport(
                    layoutViewport,
                    ViewportDimensions {
                        .scalingMode = ScalingMode::RELATIVE,
                        .dimensions = Rectf( 0.5f, 0.75f, 0.25f, 0.25f ),
                    } ),
                transformViewport(
                    layoutViewport,
                    ViewportDimensions {
                        .scalingMode = ScalingMode::RELATIVE,
                        .dimensions = Rectf( 0.25f, 0.5f, 0.25f, 0.25f ),
                    } ),
                transformViewport(
                    layoutViewport,
                    ViewportDimensions {
                        .scalingMode = ScalingMode::RELATIVE,
                        .dimensions = Rectf( 0.75f, 0.5f, 0.25f, 0.25f ),
                    } ),
            };

            for ( auto face = 0l; face < 6; ++face ) {
                // render the scene for each cubemap face
                auto viewport = viewports[ face ];
                commandBuffer->setViewport( viewport );
                commandBuffer->setScissor( viewport );
                scene->perform(
                    ApplyToGeometries(
                        [ & ]( Geometry *geometry ) {
                            // TODO: binding pipeline and render pass descriptors should be done earlier, right?
                            commandBuffer->bindGraphicsPipeline( pipeline );
                            commandBuffer->bindDescriptorSet(
                                [ & ] {
                                    auto ds = cmp.create< DescriptorSet >();
                                    ds->descriptors = {
                                        {
                                            .descriptorType = DescriptorType::UNIFORM_BUFFER,
                                            .obj = [ & ] {
                                                return crimild::alloc< CallbackUniformBuffer< Matrix4f > >(
                                                    [ light, face ] {
                                                        Transformation t;
                                                        switch ( face ) {
                                                            case 0: // positive x
                                                                t.rotate().fromAxisAngle( Vector3f::UNIT_Y, Numericf::HALF_PI );
                                                                break;

                                                            case 1: // negative x
                                                                t.rotate().fromAxisAngle( Vector3f::UNIT_Y, -Numericf::HALF_PI );
                                                                break;

                                                            case 2: // positive y
                                                                t.rotate().fromAxisAngle( Vector3f::UNIT_X, Numericf::HALF_PI );
                                                                break;

                                                            case 3: // negative y
                                                                t.rotate().fromAxisAngle( Vector3f::UNIT_X, -Numericf::HALF_PI );
                                                                break;

                                                            case 4: // positive z
                                                                t.rotate().fromAxisAngle( Vector3f::UNIT_Y, Numericf::PI );
                                                                break;

                                                            case 5: // negative z
                                                                t.rotate().fromAxisAngle( Vector3f::UNIT_Y, 0 );
                                                                break;
                                                        }

                                                        t.setTranslate( light->getWorld().getTranslate() );
                                                        auto vMatrix = t.computeModelMatrix().getInverse();
                                                        auto pMatrix = light->computeLightSpaceMatrix();
                                                        return vMatrix * pMatrix;
                                                    } );
                                            }(),
                                        },
                                    };
                                    return ds;
                                }() );
                            commandBuffer->bindDescriptorSet( geometry->getDescriptors() );
                            commandBuffer->drawPrimitive( geometry->anyPrimitive() );
                        } ) );
            }
        } );

    return offset;
}

size_t recordSpotLightCommands( Composition &cmp, CommandBuffer *commandBuffer, Pipeline *pipeline, Array< ViewportDimensions > &viewports, size_t offset, Array< Light * > lights, Node *scene ) noexcept
{
    lights.each(
        [ & ]( auto light ) {
            if ( offset >= viewports.size() ) {
                CRIMILD_LOG_WARNING( "No available viewport in layout in shadow atlas for spot light" );
                return;
            }

            auto viewport = viewports[ offset++ ];

            light->getShadowMap()->setViewport(
                [ & ] {
                    auto rect = viewport.dimensions;
                    return Vector4f(
                        rect.getX(),
                        rect.getY(),
                        rect.getWidth(),
                        rect.getHeight() );
                }() );

            commandBuffer->setViewport( viewport );
            commandBuffer->setScissor( viewport );
            scene->perform(
                ApplyToGeometries(
                    [ & ]( Geometry *geometry ) {
                        commandBuffer->bindGraphicsPipeline( pipeline );
                        commandBuffer->bindDescriptorSet(
                            [ & ] {
                                auto descriptors = cmp.create< DescriptorSet >();
                                descriptors->descriptors = {
                                    {
                                        .descriptorType = DescriptorType::UNIFORM_BUFFER,
                                        .obj = [ & ] {
                                            return crimild::alloc< CallbackUniformBuffer< Matrix4f > >(
                                                [ light ] {
                                                    auto shadowMap = light->getShadowMap();
                                                    auto vMatrix = light->getWorld().computeModelMatrix().getInverse();
                                                    auto pMatrix = light->computeLightSpaceMatrix();
                                                    shadowMap->setLightProjectionMatrix( 0, vMatrix * pMatrix );
                                                    return shadowMap->getLightProjectionMatrix( 0 );
                                                } );
                                        }(),
                                    },
                                };
                                return descriptors;
                            }() );
                        commandBuffer->bindDescriptorSet( geometry->getDescriptors() );
                        commandBuffer->drawPrimitive( geometry->anyPrimitive() );
                    } ) );
        } );

    return offset;
}

size_t recordDirectionalLightCommands(
    Composition &cmp,
    CommandBuffer *commandBuffer,
    Pipeline *pipeline,
    Array< ViewportDimensions > &viewports,
    size_t offset,
    Array< Light * > lights,
    Node *scene ) noexcept
{
    static auto ortho = []( float left, float right, float bottom, float top, float near, float far ) {
        return Matrix4f(
            2.0f / ( right - left ),
            0.0f,
            0.0f,
            0.0f,
            0.0f,
            2.0f / ( bottom - top ),
            0.0f,
            0.0f,
            0.0f,
            0.0f,
            1.0f / ( near - far ),
            0.0f,

            -( right + left ) / ( right - left ),
            -( bottom + top ) / ( bottom - top ),
            near / ( near - far ),
            1.0f );
    };

    auto updateCascade = []( auto cascadeId, auto light ) {
        auto camera = Camera::getMainCamera();
        if ( camera == nullptr ) {
            CRIMILD_LOG_ERROR( "Cannot fetch camera from scene" );
            return;
        }
        auto frustum = camera->getFrustum();

        Vector4f cascadeSplits;
        auto nearClip = frustum.getDMin();
        auto farClip = frustum.getDMax();
        auto clipRange = farClip - nearClip;
        auto minZ = nearClip;
        auto maxZ = nearClip + clipRange;
        auto range = maxZ - minZ;
        auto ratio = maxZ / minZ;

        const auto CASCADE_SPLIT_LAMBDA = 0.95f;

        // Calculate cascade split depths based on view camera frustum
        // This is based on these presentations:
        // https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
        // https://johanmedestrom.wordpress.com/2016/03/18/opengl-cascaded-shadow-maps/
        // TODO (hernan): This might break if the camera frustum changes...
        for ( auto i = 0; i < 4; ++i ) {
            auto p = float( i + 1 ) / float( 4 );
            auto log = minZ * std::pow( ratio, p );
            auto uniform = minZ + range * p;
            auto d = CASCADE_SPLIT_LAMBDA * ( log - uniform ) + uniform;
            cascadeSplits[ i ] = ( d - nearClip ) / clipRange;
        }

        auto shadowMap = light->getShadowMap();
        auto pMatrix = camera->getProjectionMatrix();
        auto vMatrix = camera->getViewMatrix();
        auto invCamera = ( vMatrix * pMatrix ).getInverse();

        // Calculate orthographic projections matrices for each cascade
        auto lastSplitDistance = cascadeId > 0 ? cascadeSplits[ cascadeId - 1 ] : 0.0f;
        auto splitDistance = cascadeSplits[ cascadeId ];

        auto frustumCorners = Array< Vector3f > {
            Vector3f( -1.0f, +1.0f, -1.0f ),
            Vector3f( +1.0f, +1.0f, -1.0f ),
            Vector3f( +1.0f, -1.0f, -1.0f ),
            Vector3f( -1.0f, -1.0f, -1.0f ),
            Vector3f( -1.0f, +1.0f, +1.0f ),
            Vector3f( +1.0f, +1.0f, +1.0f ),
            Vector3f( +1.0f, -1.0f, +1.0f ),
            Vector3f( -1.0f, -1.0f, +1.0f ),
        };

        // project frustum corners into world space
        frustumCorners = frustumCorners.map(
            [ invCamera, camera ]( const auto &p ) {
                // TODO (hernan): this is why I need to fix matrix multiplications...
                auto invCorner = invCamera.getTranspose() * Vector4f( p.x(), p.y(), p.z(), 1.0f );
                return invCorner.xyz() / invCorner.w();
            } );

        for ( auto i = 0; i < 4; ++i ) {
            auto dist = frustumCorners[ i + 4 ] - frustumCorners[ i ];
            frustumCorners[ i + 4 ] = frustumCorners[ i ] + ( dist * splitDistance );
            frustumCorners[ i ] = frustumCorners[ i ] + ( dist * lastSplitDistance );
        }

        auto frustumCenter = Vector3f::ZERO;
        frustumCorners.each(
            [ &frustumCenter ]( const auto &p ) {
                frustumCenter += p;
            } );
        frustumCenter /= frustumCorners.size();

        auto radius = 0.0f;
        frustumCorners.each(
            [ &radius, frustumCenter ]( const auto &p ) {
                auto distance = ( p - frustumCenter ).getMagnitude();
                radius = Numericf::max( radius, distance );
            } );
        radius = std::ceil( radius * 16.0f ) / 16.0f;

        auto maxExtents = Vector3f( radius, radius, radius );
        auto minExtents = -maxExtents;

        auto lightDirection = light->getDirection().getNormalized();
        auto lightViewMatrix = Matrix4f::lookAt( frustumCenter - lightDirection * -minExtents.z(), frustumCenter, Vector3f::UNIT_Y );
        // Swap Y-coordinate min/max because of Vulkan's inverted coordinate system...
        auto lightProjectionMatrix = ortho( minExtents.x(), maxExtents.x(), maxExtents.y(), minExtents.y(), 0.0f, maxExtents.z() - minExtents.z() );

        // store split distances and matrices
        shadowMap->setCascadeSplit( cascadeId, -1.0f * ( nearClip + splitDistance * clipRange ) );
        shadowMap->setLightProjectionMatrix( cascadeId, lightViewMatrix * lightProjectionMatrix );
    };

    // TODO: move this to ViewportDimensions
    static auto transformViewport = []( auto layout, auto viewport ) {
        auto ld = layout.dimensions;
        auto vd = viewport.dimensions;
        return ViewportDimensions {
            .scalingMode = ScalingMode::RELATIVE,
            .dimensions = Rectf(
                ld.getX() + vd.getX() * ld.getWidth(),
                ld.getY() + vd.getY() * ld.getHeight(),
                ld.getWidth() * vd.getWidth(),
                ld.getHeight() * vd.getHeight() ),
        };
    };

    auto recordCascadeCommands = [ & ]( auto light, auto cascadeId, auto viewport ) {
        auto descriptors = [ & ] {
            auto descriptors = cmp.create< DescriptorSet >();
            descriptors->descriptors = {
                {
                    .descriptorType = DescriptorType::UNIFORM_BUFFER,
                    .obj = [ & ] {
                        return crimild::alloc< CallbackUniformBuffer< Matrix4f > >(
                            [ &, cascadeId, light ] {
                                updateCascade( cascadeId, light );
                                auto shadowMap = light->getShadowMap();
                                return shadowMap->getLightProjectionMatrix( cascadeId );
                            } );
                    }(),
                },
            };
            return descriptors;
        }();

        commandBuffer->setViewport( viewport );
        commandBuffer->setScissor( viewport );
        scene->perform(
            ApplyToGeometries(
                [ & ]( Geometry *geometry ) {
                    commandBuffer->bindGraphicsPipeline( pipeline );
                    commandBuffer->bindDescriptorSet( descriptors );
                    commandBuffer->bindDescriptorSet( geometry->getDescriptors() );
                    commandBuffer->drawPrimitive( geometry->anyPrimitive() );
                } ) );
    };

    lights.each(
        [ & ]( auto light ) {
            if ( offset >= viewports.size() ) {
                CRIMILD_LOG_WARNING( "No available viewport in layout in shadow atlas for spot light" );
                return;
            }

            auto layoutViewport = viewports[ offset++ ];

            // Assign a single viewport to the shadow map. Since each cascade viewport is hard-coded to
            // take only a quarter of the available region, we can use that information to compute the
            // actual viewport in the shader without having to pass this information explicitly.
            light->getShadowMap()->setViewport(
                [ & ] {
                    auto rect = layoutViewport.dimensions;
                    return Vector4f(
                        rect.getX(),
                        rect.getY(),
                        rect.getWidth(),
                        rect.getHeight() );
                }() );

            ViewportDimensions cascadeViewports[ 4 ] = {
                transformViewport(
                    layoutViewport,
                    ViewportDimensions {
                        .scalingMode = ScalingMode::RELATIVE,
                        .dimensions = Rectf( 0.0f, 0.0f, 0.5f, 0.5f ),
                    } ),
                transformViewport(
                    layoutViewport,
                    ViewportDimensions {
                        .scalingMode = ScalingMode::RELATIVE,
                        .dimensions = Rectf( 0.5f, 0.0f, 0.5f, 0.5f ),
                    } ),
                transformViewport(
                    layoutViewport,
                    ViewportDimensions {
                        .scalingMode = ScalingMode::RELATIVE,
                        .dimensions = Rectf( 0.0f, 0.5f, 0.5f, 0.5f ),
                    } ),
                transformViewport(
                    layoutViewport,
                    ViewportDimensions {
                        .scalingMode = ScalingMode::RELATIVE,
                        .dimensions = Rectf( 0.5f, 0.5f, 0.5f, 0.5f ),
                    } ),
            };

            for ( auto i = 0l; i < 4; ++i ) {
                recordCascadeCommands(
                    light,
                    i,
                    cascadeViewports[ i ] );
            }
        } );

    return offset;
}

Composition crimild::compositions::computeShadow( SharedPointer< Node > const &scene ) noexcept
{
    return computeShadow( Composition {}, crimild::get_ptr( scene ) );
}

Composition crimild::compositions::computeShadow( Composition cmp, Node *scene ) noexcept
{
    FetchLights fetch;
    scene->perform( fetch );
    Map< Light::Type, Array< Light * > > lights;
    Size lightCount = 0;
    fetch.forEachLight(
        [ & ]( auto l ) {
            if ( l->castShadows() ) {
                lights[ l->getType() ].add( l );
                ++lightCount;
            }
        } );

    if ( lights.size() == 0 ) {
        CRIMILD_LOG_WARNING( "ComputeShadow composition called with no lights" );
        return cmp;
    }

    auto pipelines = Map< Light::Type, Pipeline * > {
        { Light::Type::DIRECTIONAL, createPipeline( cmp, Light::Type::DIRECTIONAL ) },
        { Light::Type::SPOT, createPipeline( cmp, Light::Type::SPOT ) },
        { Light::Type::POINT, createPipeline( cmp, Light::Type::POINT ) },
    };

    auto viewportLayout =
        lightCount == 1
            ? Array< ViewportDimensions > {
                  {
                      .scalingMode = ScalingMode::RELATIVE,
                      .dimensions = Rectf( 0.0f, 0.0f, 1.0f, 1.0f ),
                  },
              }
            : Array< ViewportDimensions > {
                  {
                      .scalingMode = ScalingMode::RELATIVE,
                      .dimensions = Rectf( 0.0f, 0.0f, 0.5f, 0.5f ),
                  },
                  {
                      .scalingMode = ScalingMode::RELATIVE,
                      .dimensions = Rectf( 0.0f, 0.5f, 0.5f, 0.5f ),
                  },
                  {
                      .scalingMode = ScalingMode::RELATIVE,
                      .dimensions = Rectf( 0.5f, 0.0f, 0.5f, 0.5f ),
                  },
                  {
                      .scalingMode = ScalingMode::RELATIVE,
                      .dimensions = Rectf( 0.5f, 0.5f, 0.5f, 0.5f ),
                  },
              };

    auto renderPass = cmp.create< RenderPass >();
    renderPass->attachments = {
        [ & ] {
            auto att = cmp.createAttachment( "shadowDepth" );
            att->format = Format::DEPTH_STENCIL_DEVICE_OPTIMAL;
            att->imageView = crimild::alloc< ImageView >();
            att->imageView->image = crimild::alloc< Image >();
            return crimild::retain( att );
        }()
    };

    renderPass->extent = {
        .scalingMode = ScalingMode::FIXED,
        .width = 4096.0f,
        .height = 4096.0f,
    };

    renderPass->clearValue = {
        .color = RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ),
    };

    renderPass->commands = [ & ] {
        auto commandBuffer = crimild::alloc< CommandBuffer >();

        auto offset = 0l;

        offset = recordDirectionalLightCommands(
            cmp,
            crimild::get_ptr( commandBuffer ),
            pipelines[ Light::Type::DIRECTIONAL ],
            viewportLayout,
            offset,
            lights[ Light::Type::DIRECTIONAL ],
            scene );

        offset = recordSpotLightCommands(
            cmp,
            crimild::get_ptr( commandBuffer ),
            pipelines[ Light::Type::SPOT ],
            viewportLayout,
            offset,
            lights[ Light::Type::SPOT ],
            scene );

        offset = recordPointLightCommands(
            cmp,
            crimild::get_ptr( commandBuffer ),
            pipelines[ Light::Type::POINT ],
            viewportLayout,
            offset,
            lights[ Light::Type::POINT ],
            scene );

        return commandBuffer;
    }();

    cmp.setOutput( crimild::get_ptr( renderPass->attachments[ 0 ] ) );

    return cmp;
}
