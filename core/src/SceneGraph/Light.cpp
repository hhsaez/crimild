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
#include "Foundation/Log.hpp"
#include "Mathematics/Frustum.hpp"
#include "Mathematics/Matrix4_inverse.hpp"
#include "Mathematics/Numbers.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Transformation_lookAt.hpp"
#include "Mathematics/Transformation_rotation.hpp"
#include "Mathematics/Transformation_translation.hpp"
#include "Mathematics/Vector3Ops.hpp"
#include "Mathematics/length.hpp"
#include "Mathematics/ortho.hpp"
#include "Mathematics/perspective.hpp"
#include "Mathematics/swizzle.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/ShadowMap.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/Uniforms/CallbackUniformBuffer.hpp"
#include "SceneGraph/Camera.hpp"

namespace crimild {

    class LightUniform : public UniformBuffer {
    public:
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

    public:
        explicit LightUniform( Light *light ) noexcept
            : UniformBuffer( LightProps {} ),
              m_light( light )
        {
            // no-op
        }

        virtual ~LightUniform( void ) = default;

        void onPreRender( void ) noexcept override
        {
            auto &props = getValue< LightProps >();

            props.type = static_cast< UInt32 >( m_light->getType() );
            props.position = vector4( m_light->getPosition(), Real( 1 ) );
            props.direction = vector4( m_light->getDirection(), Real( 0 ) );
            props.color = m_light->getColor();
            props.attenuation = vector4( m_light->getAttenuation(), Real( 0 ) );
            props.ambient = m_light->getAmbient();
            props.cutoff = Vector4f {
                Numericf::cos( m_light->getInnerCutoff() ),
                Numericf::cos( m_light->getOuterCutoff() ),
                0.0f,
                0.0f,
            };
            props.castShadows = m_light->castShadows() ? 1 : 0;
            if ( m_light->castShadows() ) {
                props.shadowBias = m_light->getShadowMap()->getBias();
                props.cascadeSplits = m_light->getShadowMap()->getCascadeSplits();
                for ( auto split = 0; split < 4; ++split ) {
                    props.lightSpaceMatrix[ split ] = m_light->getShadowMap()->getLightProjectionMatrix( split );
                }
                props.viewport = m_light->getShadowMap()->getViewport();
            }
            props.energy = m_light->getEnergy();
            props.radius = m_light->getRadius();
        }

    private:
        Light *m_light = nullptr;
    };
}

using namespace crimild;

Light::Light( Type type )
    : _type( type ),
      _attenuation { 1.0f, 0.0f, 0.0f },
      _color { 1.0f, 1.0f, 1.0f, 1.0f },
      _outerCutoff( 0.0f ),
      _innerCutoff( 0.0f ),
      _exponent( 0.0f ),
      _ambient { 0.0f, 0.0f, 0.0f, 0.0f }
{
}

Light::~Light( void )
{
    unload();
}

[[nodiscard]] Point3 Light::getPosition( void ) const noexcept
{
    return location( getWorld() );
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
        return ortho( -50.0f, 50.0f, -50.0f, 50.0f, 1.0f, 200.0f );
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
    auto camera = Camera::getMainCamera();
    if ( camera == nullptr ) {
        CRIMILD_LOG_ERROR( "Cannot fetch camera from scene" );
        return;
    }

    Array< Real > cascadeSplits( 4 );

    // TODO: get clipping values from camera
    auto nearClip = 0.1f;   //frustum.getDMin();
    auto farClip = 1024.0f; //frustum.getDMax();
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

    // TODO: this should be reversed now
    auto invCamera = inverse( ( vMatrix * pMatrix ) ); //.getInverse();

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

    // project frustum corners into world space
    frustumCorners = frustumCorners.map(
        [ invCamera, camera ]( const auto &p ) {
            // TODO (hernan): this is why I need to fix matrix multiplications...
            auto invCorner = transpose( invCamera ) * Vector4f { p.x, p.y, p.z, 1.0f };
            return xyz( invCorner ) / invCorner.w;
        } );

    for ( auto i = 0; i < 4; ++i ) {
        auto dist = frustumCorners[ i + 4 ] - frustumCorners[ i ];
        frustumCorners[ i + 4 ] = frustumCorners[ i ] + ( dist * splitDistance );
        frustumCorners[ i ] = frustumCorners[ i ] + ( dist * lastSplitDistance );
    }

    auto frustumCenter = Vector3f::Constants::ZERO;
    frustumCorners.each(
        [ &frustumCenter ]( const auto &p ) {
            frustumCenter = frustumCenter + p;
        } );
    frustumCenter = frustumCenter / frustumCorners.size();

    auto radius = 0.0f;
    frustumCorners.each(
        [ &radius, frustumCenter ]( const auto &p ) {
            auto distance = length( p - frustumCenter );
            radius = Numericf::max( radius, distance );
        } );
    radius = std::ceil( radius * 16.0f ) / 16.0f;

    auto maxExtents = Vector3f { radius, radius, radius };
    auto minExtents = -maxExtents;

    const auto lightDirection = normalize( light->getDirection() );
    const auto lightViewMatrix = lookAt( Point3 { frustumCenter - lightDirection * -minExtents.z }, point3( frustumCenter ), Vector3f::Constants::UNIT_Y );
    // Swap Y-coordinate min/max because of Vulkan's inverted coordinate system...
    auto lightProjectionMatrix = ortho( minExtents.x, maxExtents.x, maxExtents.y, minExtents.y, 0.0f, maxExtents.z - minExtents.z );

    // store split distances and matrices
    shadowMap->setCascadeSplit( cascadeId, -1.0f * ( nearClip + splitDistance * clipRange ) );
    shadowMap->setLightProjectionMatrix( cascadeId, lightViewMatrix.mat * lightProjectionMatrix );
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
                            const auto t0 = []( auto face ) {
                                switch ( face ) {
                                    case 0: // positive x
                                        return rotationY( numbers::PI_DIV_2 );

                                    case 1: // negative x
                                        return rotationY( -numbers::PI_DIV_2 );

                                    case 2: // positive y
                                        return rotationX( numbers::PI_DIV_2 );

                                    case 3: // negative y
                                        return rotationX( -numbers::PI_DIV_2 );

                                    case 4: // positive z
                                        return rotationY( numbers::PI );

                                    case 5: // negative z
                                    default:
                                        return rotationY( 0 );
                                }
                            }( face );

                            //t.setTranslate( Vector3f::ZERO ); // TODO (hernan): use probe's position
                            const auto t1 = translation( vector3( location( light->getWorld() ) ) );
                            const auto vMatrix = ( t1 * t0 ).invMat; //t.getInverseMatrix(); //t.computeModelMatrix().getInverse();

                            //t.setTranslate( light->getWorld().getTranslate() );
                            //auto vMatrix = t.computeModelMatrix().getInverse();
                            auto pMatrix = light->computeLightSpaceMatrix();
                            return ShadowAtlasLightUniform {
                                .proj = pMatrix,
                                .view = vMatrix,
                                .lightPos = light->getWorld()( Vector3::Constants::ZERO ),
                            };
                        } ),
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
                                auto vMatrix = light->getWorld().invMat;
                                auto pMatrix = light->computeLightSpaceMatrix();
                                shadowMap->setLightProjectionMatrix( 0, vMatrix * pMatrix );
                                return ShadowAtlasLightUniform {
                                    .proj = pMatrix,
                                    .view = vMatrix,
                                    .lightPos = light->getWorld()( Vector3::Constants::ZERO ),
                                };
                            } );
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
                                } );
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

    crimild::Bool shadows = false;
    decoder.decode( "shadows", shadows );
    setCastShadows( shadows );
}
