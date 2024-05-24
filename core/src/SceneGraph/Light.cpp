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

#include "SceneGraph/Light.hpp"

#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"
#include "Crimild_Foundation.hpp"
#include "Crimild_Mathematics.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/ShadowMap.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/Uniforms/CallbackUniformBuffer.hpp"
#include "Rendering/Uniforms/LightUniform.hpp"
#include "SceneGraph/Camera.hpp"

using namespace crimild;

Light::Light( Type type )
    : _type( type ),
      _attenuation { 1.0f, 0.0f, 0.0f },
      _color { 1.0f, 1.0f, 1.0f },
      _outerCutoff( 0.0f ),
      _innerCutoff( 0.0f ),
      _exponent( 0.0f ),
      _ambient { 0.0f, 0.0f, 0.0f }
{
}

Light::~Light( void )
{
    unload();
}

[[nodiscard]] Point3f Light::getPosition( void ) const noexcept
{
    return origin( getWorld() );
}

[[nodiscard]] Vector3 Light::getDirection( void ) const noexcept
{
    return ( _type == Type::POINT ? Vector3::Constants::ZERO : forward( getWorld() ) );
}

void Light::accept( NodeVisitor &visitor )
{
    visitor.visitLight( this );
}

void Light::setCastShadows( crimild::Bool enabled )
{
    if ( enabled ) {
        setShadowMap( crimild::alloc< ShadowMap >() );
    } else {
        setShadowMap( nullptr );
    }
}

Matrix4f Light::computeLightSpaceMatrix( void ) const noexcept
{
    if ( getType() == Type::DIRECTIONAL ) {
        // Use a hardcoded orthographic projection for light space matrix when using
        // directional lights. This will change later when using cascade shadow
        // mapping.
        return ortho( -200, 200, -200, 200, 0.0f, 400.0f ) * Matrix4f( inverse( getWorld() ) );
    } else if ( getType() == Type::POINT ) {
        return perspective( 90.0f, 1.0f, 0.01f, 200.0f );
    } else {
        return perspective( 45.0f, 1.0f, 1.0f, 200.0f );
    }
}

DescriptorSet *Light::getDescriptors( void ) noexcept
{
    if ( auto ds = crimild::get_ptr( m_descriptors ) ) {
        return ds;
    }

    m_descriptors = [ & ] {
        auto descriptorSet = crimild::alloc< DescriptorSet >();
        descriptorSet->descriptors = {
            Descriptor {
                .descriptorType = DescriptorType::UNIFORM_BUFFER,
                .obj = crimild::alloc< LightUniform >( this ),
            },
        };
        return descriptorSet;
    }();

    return crimild::get_ptr( m_descriptors );
}

void Light::setRadius( Real32 radius ) noexcept
{
    m_radius = radius;
}

Real32 Light::getRadius( void ) const noexcept
{
    if ( m_radius >= 0.0f ) {
        return m_radius;
    }

    const auto MIN_ATTENUATION = 5.0f / 256.0f;
    return Numericf::sqrt( m_energy / MIN_ATTENUATION );
}

struct ShadowAtlasLightUniform {
    alignas( 16 ) Matrix4f proj;
    alignas( 16 ) Matrix4f view;
    alignas( 16 ) Vector3f lightPos;
};

auto updateCascade = []( auto cascadeId, auto light ) {
    // auto camera = Camera::getMainCamera();
    Camera *camera = nullptr; // TODO
    if ( camera == nullptr ) {
        CRIMILD_LOG_ERROR( "Cannot fetch camera from scene" );
        return;
    }

    Array< Real > cascadeSplits( 4 );

    // TODO: get clipping values from camera
    auto nearClip = 0.1f;   // frustum.getDMin();
    auto farClip = 1000.0f; // frustum.getDMax();
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
    const auto pMatrix = camera->getProjectionMatrix();
    const auto vMatrix = camera->getViewMatrix();
    const auto invViewProj = inverse( pMatrix * vMatrix );

    // Calculate orthographic projections matrices for each cascade
    auto lastSplitDistance = cascadeId > 0 ? cascadeSplits[ cascadeId - 1 ] : 0.0f;
    auto splitDistance = cascadeSplits[ cascadeId ];

    auto frustumCorners = Array< Vector3f > {
        Vector3f { -1.0f, +1.0f, -1.0f },
        Vector3f { +1.0f, +1.0f, -1.0f },
        Vector3f { +1.0f, -1.0f, -1.0f },
        Vector3f { -1.0f, -1.0f, -1.0f },
        Vector3f { -1.0f, +1.0f, +1.0f },
        Vector3f { +1.0f, +1.0f, +1.0f },
        Vector3f { +1.0f, -1.0f, +1.0f },
        Vector3f { -1.0f, -1.0f, +1.0f },
    };

    for ( auto i = 0l; i < 8; ++i ) {
        const auto inversePoint = invViewProj * Vector4( frustumCorners[ i ] );
        frustumCorners[ i ] = Vector3f( inversePoint / inversePoint.w );
    }

    for ( auto i = 0l; i < 4; ++i ) {
        const auto cornerRay = frustumCorners[ i + 4 ] - frustumCorners[ i ];
        const auto nearCornerRay = cornerRay * lastSplitDistance;
        const auto farCornerRay = cornerRay * splitDistance;
        frustumCorners[ i + 4 ] = frustumCorners[ i ] + farCornerRay;
        frustumCorners[ i ] = frustumCorners[ i ] + nearCornerRay;
    }

    auto frustumCenter = Vector3::Constants::ZERO;
    for ( auto i = 0l; i < 8; ++i ) {
        frustumCenter = frustumCenter + frustumCorners[ i ];
    }
    frustumCenter = frustumCenter / 8.0f;

    auto far = numbers::NEGATIVE_INFINITY;
    auto near = numbers::POSITIVE_INFINITY;
    auto radius = Real( 0 );

    for ( auto i = 0l; i < 8; ++i ) {
        const auto distance = length( frustumCorners[ i ] - frustumCenter );
        radius = crimild::max( radius, distance );
    }
    radius = crimild::ceil( radius * 16.0f ) / 16.0f;

    const auto maxExtents = Vector3 { radius, radius, radius };
    const auto minExtents = -maxExtents;

    const auto lightDirection = light->getDirection();
    const auto lightViewMatrix = Matrix4f( inverse( lookAt(
        Point3f { frustumCenter + lightDirection * minExtents.z },
        Point3f( frustumCenter ),
        Vector3f::Constants::UP
    ) ) );

    // Swap Y-coordinate min/max because of Vulkan's inverted coordinate system...
    const auto lightProjectionMatrix = ortho( minExtents.x, maxExtents.x, maxExtents.y, minExtents.y, 0.0f, maxExtents.z - minExtents.z );

    // store split distances and matrices
    shadowMap->setCascadeSplit( cascadeId, -1.0f * ( nearClip + splitDistance * clipRange ) );
    shadowMap->setLightProjectionMatrix( cascadeId, lightProjectionMatrix * lightViewMatrix );
};

Array< SharedPointer< DescriptorSet > > &Light::getShadowAtlasDescriptors( void ) noexcept
{
    if ( !m_shadowAtlasDescriptors.empty() ) {
        return m_shadowAtlasDescriptors;
    }

    if ( getType() == Light::Type::POINT ) {
        m_shadowAtlasDescriptors.resize( 6 );
        for ( auto face = 0l; face < 6; ++face ) {
            auto descriptors = crimild::alloc< DescriptorSet >();
            descriptors->descriptors = {
                {
                    .descriptorType = DescriptorType::UNIFORM_BUFFER,
                    .obj = crimild::alloc< CallbackUniformBuffer< ShadowAtlasLightUniform > >(
                        [ light = this, face ] {
                            // TODO (hernan): use probe's position
                            const auto lightPos = origin( light->getWorld() );
                            const auto t = [ lightPos ]( auto face ) {
                                switch ( face ) {
                                    case 0: // negative x
                                        return lookAt(
                                            lightPos,
                                            lightPos - Vector3::Constants::UNIT_X,
                                            Vector3::Constants::UP
                                        );

                                    case 1: // positive x
                                        return lookAt(
                                            lightPos,
                                            lightPos + Vector3::Constants::UNIT_X,
                                            Vector3::Constants::UP
                                        );

                                    case 2: // positive y
                                        return lookAt(
                                            lightPos,
                                            lightPos + Vector3::Constants::UNIT_Y,
                                            Vector3::Constants::UNIT_Z
                                        );

                                    case 3: // negative y
                                        return lookAt(
                                            lightPos,
                                            lightPos - Vector3::Constants::UNIT_Y,
                                            -Vector3::Constants::UNIT_Z
                                        );

                                    case 4: // positive z
                                        return lookAt(
                                            lightPos,
                                            lightPos + Vector3::Constants::UNIT_Z,
                                            Vector3::Constants::UP
                                        );

                                    case 5: // negative z
                                    default:
                                        return lookAt(
                                            lightPos,
                                            lightPos - Vector3::Constants::UNIT_Z,
                                            Vector3::Constants::UP
                                        );
                                }
                            }( face );

                            const auto vMatrix = Matrix4f( inverse( t ) );

                            auto pMatrix = light->computeLightSpaceMatrix();
                            return ShadowAtlasLightUniform {
                                .proj = pMatrix,
                                .view = vMatrix,
                                .lightPos = light->getWorld()( Vector3::Constants::ZERO ),
                            };
                        }
                    ),
                },
            };
            m_shadowAtlasDescriptors[ face ] = descriptors;
        }
    } else if ( getType() == Light::Type::SPOT ) {
        m_shadowAtlasDescriptors.resize( 1 );
        m_shadowAtlasDescriptors[ 0 ] = [ & ] {
            auto descriptors = crimild::alloc< DescriptorSet >();
            descriptors->descriptors = {
                {
                    .descriptorType = DescriptorType::UNIFORM_BUFFER,
                    .obj = [ & ] {
                        return crimild::alloc< CallbackUniformBuffer< ShadowAtlasLightUniform > >(
                            [ light = this ] {
                                auto shadowMap = light->getShadowMap();
                                auto vMatrix = Matrix4f( inverse( light->getWorld() ) );
                                auto pMatrix = light->computeLightSpaceMatrix();
                                shadowMap->setLightProjectionMatrix( 0, pMatrix * vMatrix );
                                return ShadowAtlasLightUniform {
                                    .proj = pMatrix,
                                    .view = vMatrix,
                                    .lightPos = light->getWorld()( Vector3::Constants::ZERO ),
                                };
                            }
                        );
                    }(),
                },
            };
            return descriptors;
        }();
    } else if ( getType() == Light::Type::DIRECTIONAL ) {
        const UInt32 MAX_CASCADES = 4;
        m_shadowAtlasDescriptors.resize( MAX_CASCADES );
        for ( UInt32 cascadeId = 0; cascadeId < MAX_CASCADES; ++cascadeId ) {
            m_shadowAtlasDescriptors[ cascadeId ] = [ & ] {
                auto descriptors = crimild::alloc< DescriptorSet >();
                descriptors->descriptors = {
                    {
                        .descriptorType = DescriptorType::UNIFORM_BUFFER,
                        .obj = [ & ] {
                            return crimild::alloc< CallbackUniformBuffer< ShadowAtlasLightUniform > >(
                                [ &, cascadeId, light = this ] {
                                    updateCascade( cascadeId, light );
                                    auto shadowMap = light->getShadowMap();
                                    return ShadowAtlasLightUniform {
                                        .proj = shadowMap->getLightProjectionMatrix( cascadeId ),
                                        .view = Matrix4f::Constants::IDENTITY,
                                        .lightPos = light->getWorld()( Vector3::Constants::ZERO ),
                                    };
                                }
                            );
                        }(),
                    },
                };
                return descriptors;
            }();
        }
    }

    return m_shadowAtlasDescriptors;
}

void Light::encode( coding::Encoder &encoder )
{
    Node::encode( encoder );

    std::string lightType;
    switch ( _type ) {
        case Light::Type::POINT:
            lightType = "point";
            break;

        case Light::Type::DIRECTIONAL:
            lightType = "directional";
            break;

        case Light::Type::SPOT:
            lightType = "spot";
            break;

        case Light::Type::AMBIENT:
            lightType = "ambient";
            break;
    }
    encoder.encode( "lightType", lightType );

    encoder.encode( "attenuation", _attenuation );
    encoder.encode( "color", _color );
    encoder.encode( "ambient", _ambient );

    encoder.encode( "innerCutoff", _innerCutoff );
    encoder.encode( "outerCutoff", _outerCutoff );

    encoder.encode( "energy", m_energy );
    encoder.encode( "radius", m_radius );

    encoder.encode( "castShadows", castShadows() );
}

void Light::decode( coding::Decoder &decoder )
{
    Node::decode( decoder );

    std::string lightType;
    decoder.decode( "lightType", lightType );
    if ( lightType == "ambient" ) {
        _type = Light::Type::AMBIENT;
    } else if ( lightType == "directional" ) {
        _type = Light::Type::DIRECTIONAL;
    } else if ( lightType == "spot" ) {
        _type = Light::Type::SPOT;
    } else {
        _type = Light::Type::POINT;
    }

    decoder.decode( "attenuation", _attenuation );
    decoder.decode( "color", _color );
    decoder.decode( "ambient", _ambient );

    decoder.decode( "innerCutoff", _innerCutoff );
    decoder.decode( "outerCutoff", _outerCutoff );

    decoder.decode( "energy", m_energy );
    decoder.decode( "radius", m_radius );

    crimild::Bool shadows = false;
    decoder.decode( "castShadows", shadows );
    setCastShadows( shadows );
}
