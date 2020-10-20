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

#include "Rendering/Compositions/RenderScene.hpp"

#include "Components/MaterialComponent.hpp"
#include "Rendering/CommandBuffer.hpp"
#include "Rendering/Compositions/ComputeBRDFLUTComposition.hpp"
#include "Rendering/Compositions/ComputeIrradianceMapComposition.hpp"
#include "Rendering/Compositions/ComputePrefilterMapComposition.hpp"
#include "Rendering/Compositions/ComputeReflectionMapComposition.hpp"
#include "Rendering/Compositions/ComputeShadowComposition.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/Uniforms/CameraViewProjectionUniformBuffer.hpp"
#include "Rendering/Uniforms/LightingUniform.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Node.hpp"
#include "Visitors/ApplyToGeometries.hpp"
#include "Visitors/FetchCameras.hpp"
#include "Visitors/FetchLights.hpp"

using namespace crimild;
using namespace crimild::compositions;

struct Renderables {
    Array< Geometry * > environment;
    Array< Geometry * > unlit;
    Array< Geometry * > unlitTransparent;
    Array< Geometry * > litBasic;
    Array< Geometry * > litBasicTransparent;
    Array< Geometry * > lit;
    Array< Geometry * > litTransparent;
};

static Renderables sortRenderables( Node *scene ) noexcept
{
    Renderables ret;

    scene->perform(
        ApplyToGeometries(
            [ &ret ]( Geometry *geometry ) {
                if ( geometry->getLayer() == Node::Layer::SKYBOX ) {
                    ret.environment.add( geometry );
                } 
                else if ( auto material = geometry->getComponent< MaterialComponent >()->first() ) {
                    // TODO: What if there are multiple materials?
                    // Can we add the same geometry to multiple lists? That will result
                    // in multiple render passes for a single geometry, but I don't know
                    // if that's ok. 
                    if ( auto pipeline = material->getPipeline() ) {
                        if ( auto program = crimild::get_ptr( pipeline->program ) ) {
                            program->descriptorSetLayouts.each(
                                [ &ret, geometry ]( auto layout ) {
                                    if ( layout->bindings.filter( []( auto &binding ) { return binding.descriptorType == DescriptorType::ALBEDO_MAP; } ).size() > 0 ) {
                                        // assume PBR
                                        ret.lit.add( geometry );
                                    }
                                    else {
                                        // assume phong lit
                                        ret.litBasic.add( geometry );
                                    }
                                }
                            );
                        }
                    }
                }
            }
        )
    );

    return ret;
}

Composition crimild::compositions::renderScene( SharedPointer< Node > const &scene, crimild::Bool useHDR ) noexcept
{
    return renderScene( crimild::get_ptr( scene ), useHDR );
}

Composition crimild::compositions::renderScene( Node *scene, crimild::Bool useHDR ) noexcept
{
    auto renderables = sortRenderables( scene );

    Composition cmp;
    cmp.enableHDR( useHDR );

    cmp = computeShadow( cmp, scene );
    auto shadowAtlas = [ & ] {
        auto texture = cmp.create< Texture >();
        texture->imageView = [ & ] {
            auto att = cmp.getOutput();
            if ( att == nullptr || att->imageView == nullptr ) {
                auto imageView = crimild::alloc< ImageView >();
                imageView->image = Image::ONE;
                return imageView;
            }
            return att->imageView;
        }();
        texture->sampler = [] {
            auto sampler = crimild::alloc< Sampler >();
            // In order to avoid darking objects outside of the view frustum, we set
            // the wrap mode to either CLAMP_TO_BORDER, with a WHITE border color
            // (all ones). That way, any object outside the light's frustum will have
            // a depth of 1.0. Then, when comparing that depth with the current
            // fragment's one, the later one will never be in shadow.
            // For some types of lights, we might want to use a different approach
            // though. For example, point lights or spot lights should make objects
            // outside of the view frustum to be in shadow.
            // TODO (hernan): Maybe it will be best to split the shadow atlas into two
            // after all
            sampler->setWrapMode( Sampler::WrapMode::CLAMP_TO_BORDER );
            sampler->setBorderColor( Sampler::BorderColor::INT_OPAQUE_WHITE );
            return sampler;
        }();
        return texture;
    }();

    auto reflectionAtlas = [ & ] {
        auto texture = cmp.create< Texture >();
        texture->imageView = [ & ] {
            auto att = [ & ]() -> Attachment * {
                if ( renderables.environment.empty() ) {
                    return nullptr;
                }
                cmp = computeReflectionMap( cmp, scene );
                return cmp.getOutput();
            }();
            if ( att == nullptr || att->imageView == nullptr ) {
                auto imageView = crimild::alloc< ImageView >();
                imageView->image = Image::ZERO;
                return imageView;
            }
            return att->imageView;
        }();
        texture->sampler = [] {
            auto sampler = crimild::alloc< Sampler >();
            sampler->setMinFilter( Sampler::Filter::LINEAR );
            sampler->setMagFilter( Sampler::Filter::LINEAR );
            sampler->setWrapMode( Sampler::WrapMode::CLAMP_TO_BORDER );
            sampler->setBorderColor( Sampler::BorderColor::INT_OPAQUE_WHITE );
            return sampler;
        }();
        return texture;
    }();

    auto irradianceAtlas = [ & ] {
        auto texture = cmp.create< Texture >();
        texture->imageView = [ & ] {
            auto att = [ & ]() -> Attachment * {
                if ( renderables.environment.empty() ) {
                    return nullptr;
                }
                cmp = computeIrradianceMap( cmp );
                return cmp.getOutput();
            }();
            if ( att == nullptr || att->imageView == nullptr ) {
                auto imageView = crimild::alloc< ImageView >();
                imageView->image = Image::ZERO;
                return imageView;
            }
            return att->imageView;
        }();
        texture->sampler = [] {
            auto sampler = crimild::alloc< Sampler >();
            sampler->setMinFilter( Sampler::Filter::LINEAR );
            sampler->setMagFilter( Sampler::Filter::LINEAR );
            sampler->setWrapMode( Sampler::WrapMode::CLAMP_TO_BORDER );
            sampler->setBorderColor( Sampler::BorderColor::INT_OPAQUE_WHITE );
            return sampler;
        }();
        return texture;
    }();

    auto prefilterAtlas = [ & ] {
        auto texture = cmp.create< Texture >();
        texture->imageView = [ & ] {
            auto att = [ & ]() -> Attachment * {
                if ( renderables.environment.empty() ) {
                    return nullptr;
                }
                cmp = computePrefilterMap( cmp );
                return cmp.getOutput();
            }();
            if ( att == nullptr || att->imageView == nullptr ) {
                auto imageView = crimild::alloc< ImageView >();
                imageView->image = Image::ZERO;
                return imageView;
            }
            return att->imageView;
        }();
        texture->sampler = [] {
            auto sampler = crimild::alloc< Sampler >();
            sampler->setMinFilter( Sampler::Filter::LINEAR );
            sampler->setMagFilter( Sampler::Filter::LINEAR );
            sampler->setWrapMode( Sampler::WrapMode::CLAMP_TO_BORDER );
            sampler->setBorderColor( Sampler::BorderColor::INT_OPAQUE_WHITE );
            return sampler;
        }();
        return texture;
    }();

    auto BRDFLUT = [ & ] {
        auto texture = cmp.create< Texture >();
        texture->imageView = [ & ] {
            auto att = [ & ]() -> Attachment * {
                if ( renderables.environment.empty() ) {
                    return nullptr;
                }
                cmp = computeBRDFLUT( cmp );
                return cmp.getOutput();
            }();
            if ( att == nullptr || att->imageView == nullptr ) {
                auto imageView = crimild::alloc< ImageView >();
                imageView->image = Image::ZERO;
                return imageView;
            }
            return att->imageView;
        }();
        texture->sampler = [] {
            auto sampler = crimild::alloc< Sampler >();
            sampler->setMinFilter( Sampler::Filter::LINEAR );
            sampler->setMagFilter( Sampler::Filter::LINEAR );
            sampler->setWrapMode( Sampler::WrapMode::CLAMP_TO_BORDER );
            sampler->setBorderColor( Sampler::BorderColor::INT_OPAQUE_WHITE );
            return sampler;
        }();
        return texture;
    }();

    auto renderPass = cmp.create< RenderPass >();
    renderPass->attachments = {
        [ & ] {
            auto att = cmp.createAttachment( "gBufferColor" );
            att->usage = Attachment::Usage::COLOR_ATTACHMENT;
            if ( useHDR ) {
                att->format = Format::R32G32B32A32_SFLOAT;
            } else {
                att->format = Format::R8G8B8A8_UNORM;
            }
            att->imageView = crimild::alloc< ImageView >();
            att->imageView->image = crimild::alloc< Image >();
            return crimild::retain( att );
        }(),
        [ & ] {
            auto att = cmp.createAttachment( "gBufferDepth" );
            att->format = Format::DEPTH_STENCIL_DEVICE_OPTIMAL;
            att->imageView = crimild::alloc< ImageView >();
            att->imageView->image = crimild::alloc< Image >();
            return crimild::retain( att );
        }()
    };

    auto environmentDescriptors = [ & ] {
        auto descriptorSet = cmp.create< DescriptorSet >();
        descriptorSet->descriptors = {
            Descriptor {
                .descriptorType = DescriptorType::UNIFORM_BUFFER,
                .obj = [ & ] {
                    FetchCameras fetch;
                    scene->perform( fetch );
                    auto camera = fetch.anyCamera();
                    return crimild::alloc< CameraViewProjectionUniform >( camera );
                }(),
            },
        };
        return descriptorSet;
    }();

    auto litBasicDescriptors = [ & ] {
        auto descriptorSet = cmp.create< DescriptorSet >();
        descriptorSet->descriptors = {
            Descriptor {
                .descriptorType = DescriptorType::UNIFORM_BUFFER,
                .obj = [ & ] {
                    FetchCameras fetch;
                    scene->perform( fetch );
                    auto camera = fetch.anyCamera();
                    return crimild::alloc< CameraViewProjectionUniform >( camera );
                }(),
            },
            Descriptor {
                .descriptorType = DescriptorType::UNIFORM_BUFFER,
                .obj = crimild::alloc< LightingUniform >( [ & ] {
                    FetchLights fetch;
                    Array< Light * > lights;
                    scene->perform( fetch );
                    fetch.forEachLight( [ & ]( auto light ) {
                        lights.add( light );
                    } );
                    return lights;
                }() ),
            },
            Descriptor {
                .descriptorType = DescriptorType::SHADOW_ATLAS,
                .obj = crimild::retain( shadowAtlas ),
            },
        };
        return descriptorSet;
    }();

    auto litDescriptors = [ & ] {
        auto descriptorSet = cmp.create< DescriptorSet >();
        descriptorSet->descriptors = {
            Descriptor {
                .descriptorType = DescriptorType::UNIFORM_BUFFER,
                .obj = [ & ] {
                    FetchCameras fetch;
                    scene->perform( fetch );
                    auto camera = fetch.anyCamera();
                    return crimild::alloc< CameraViewProjectionUniform >( camera );
                }(),
            },
            Descriptor {
                .descriptorType = DescriptorType::UNIFORM_BUFFER,
                .obj = crimild::alloc< LightingUniform >( [ & ] {
                    FetchLights fetch;
                    Array< Light * > lights;
                    scene->perform( fetch );
                    fetch.forEachLight( [ & ]( auto light ) {
                        lights.add( light );
                    } );
                    return lights;
                }() ),
            },
            Descriptor {
                .descriptorType = DescriptorType::SHADOW_ATLAS,
                .obj = crimild::retain( shadowAtlas ),
            },
            Descriptor {
                .descriptorType = DescriptorType::REFLECTION_ATLAS,
                .obj = crimild::retain( reflectionAtlas ),
            },
            Descriptor {
                .descriptorType = DescriptorType::IRRADIANCE_ATLAS,
                .obj = crimild::retain( irradianceAtlas ),
            },
            Descriptor {
                .descriptorType = DescriptorType::PREFILTER_ATLAS,
                .obj = crimild::retain( prefilterAtlas ),
            },
            Descriptor {
                .descriptorType = DescriptorType::BRDF_LUT,
                .obj = crimild::retain( BRDFLUT ),
            },
        };
        return descriptorSet;
    }();

    auto viewport = ViewportDimensions {
        .scalingMode = ScalingMode::SWAPCHAIN_RELATIVE,
        .dimensions = Rectf( 0, 0, 1, 1 ),
    };

    renderPass->commands = [ & ] {
        auto commandBuffer = crimild::alloc< CommandBuffer >();
        commandBuffer->setViewport( viewport );
        commandBuffer->setScissor( viewport );
        renderables.litBasic.each(
            [ & ]( Geometry *g ) {
                if ( auto ms = g->getComponent< MaterialComponent >() ) {
                    if ( auto material = ms->first() ) {
                        commandBuffer->bindGraphicsPipeline( material->getPipeline() );
                        commandBuffer->bindDescriptorSet( litBasicDescriptors );
                        commandBuffer->bindDescriptorSet( material->getDescriptors() );
                        commandBuffer->bindDescriptorSet( g->getDescriptors() );
                        commandBuffer->drawPrimitive( g->anyPrimitive() );
                    }
                }
            }
        );
        renderables.lit.each(
            [ & ]( Geometry *g ) {
                if ( auto ms = g->getComponent< MaterialComponent >() ) {
                    if ( auto material = ms->first() ) {
                        commandBuffer->bindGraphicsPipeline( material->getPipeline() );
                        commandBuffer->bindDescriptorSet( litDescriptors );
                        commandBuffer->bindDescriptorSet( material->getDescriptors() );
                        commandBuffer->bindDescriptorSet( g->getDescriptors() );
                        commandBuffer->drawPrimitive( g->anyPrimitive() );
                    }
                }
            }
        );
        renderables.environment.each(
            [ & ]( Geometry *g ) {
                if ( auto ms = g->getComponent< MaterialComponent >() ) {
                    if ( auto material = ms->first() ) {
                        commandBuffer->bindGraphicsPipeline( material->getPipeline() );
                        commandBuffer->bindDescriptorSet( environmentDescriptors );
                        commandBuffer->bindDescriptorSet( material->getDescriptors() );
                        commandBuffer->bindDescriptorSet( g->getDescriptors() );
                        commandBuffer->drawPrimitive( g->anyPrimitive() );
                    }
                }
            }
        );
        return commandBuffer;
    }();

    cmp.setOutput( crimild::get_ptr( renderPass->attachments[ 0 ] ) );

    return cmp;
}
