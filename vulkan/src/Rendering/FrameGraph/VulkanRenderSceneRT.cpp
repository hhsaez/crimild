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

#include "Rendering/FrameGraph/VulkanRenderSceneRT.hpp"

#include "Mathematics/Box_normal.hpp"
#include "Mathematics/ColorRGBOps.hpp"
#include "Mathematics/ColorRGB_isZero.hpp"
#include "Mathematics/Cylinder_normal.hpp"
#include "Mathematics/Point3Ops.hpp"
#include "Mathematics/Random.hpp"
#include "Mathematics/Ray_apply.hpp"
#include "Mathematics/Sphere_normal.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Vector2Ops.hpp"
#include "Mathematics/Vector3Ops.hpp"
#include "Mathematics/Vector3_isZero.hpp"
#include "Mathematics/dot.hpp"
#include "Mathematics/intersect.hpp"
#include "Mathematics/isNaN.hpp"
#include "Mathematics/min.hpp"
#include "Mathematics/pow.hpp"
#include "Mathematics/reflect.hpp"
#include "Mathematics/refract.hpp"
#include "Mathematics/swizzle.hpp"
#include "Mathematics/whichSide.hpp"
#include "Rendering/Buffer.hpp"
#include "Rendering/BufferAccessor.hpp"
#include "Rendering/BufferView.hpp"
#include "Rendering/VulkanBuffer.hpp"
#include "Rendering/VulkanCommandBuffer.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanRenderTarget.hpp"
#include "SceneGraph/Camera.hpp"
#include "Simulation/Settings.hpp"
#include "Visitors/BinTreeScene.hpp"
#include "Visitors/ShallowCopy.hpp"
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild::vulkan;
using namespace crimild::vulkan::framegraph;

RenderSceneRT::RenderSceneRT( RenderDevice *device, std::string name, const VkExtent2D &extent ) noexcept
    : RenderBase( device, name, extent ),
      WithCommandBuffer( crimild::alloc< CommandBuffer >( device, getName() + "/CommandBuffer" ) ),
      m_outputTarget(
          crimild::alloc< RenderTarget >(
              device,
              getName() + "/Targets/Color",
              VK_FORMAT_R32G32B32A32_SFLOAT,
              extent
          )
      ),
      m_imageData(
          crimild::alloc< BufferView >(
              BufferView::Target::STAGING,
              crimild::alloc< crimild::Buffer >(
                  Array< ColorRGBA >( getExtent().width * getExtent().height )
              ),
              0,
              sizeof( ColorRGBA )
          )
      ),
      m_imageAccessor( crimild::alloc< BufferAccessor >( m_imageData ) ),
      m_stagingBuffer(
          crimild::alloc< vulkan::Buffer >( device, getName() + "/Staging", m_imageData.get() )
      )
{
    m_imageData->setUsage( BufferView::Usage::DYNAMIC );
}

RenderSceneRT::~RenderSceneRT( void ) noexcept
{
    m_state = State::TERMINATED;
    if ( m_done.valid() ) {
        m_done.wait();
    }

    getRenderDevice()->flush();
}

void RenderSceneRT::onResize( void ) noexcept
{
    // TODO
}

void RenderSceneRT::reset( void ) noexcept
{
    if ( m_done.valid() ) {
        m_done.wait();
    }

    m_scene = nullptr;
    m_camera = nullptr;
    m_sampleCount = 0;
    m_state = State::RUNNING;
}

void RenderSceneRT::setup( crimild::Node *scene, Camera *camera ) noexcept
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

        m_camera = crimild::alloc< Camera >( 45, getExtent().width / getExtent().height, 0.1f, 1000.0f );
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

    // Initialize image to black
    const auto N = m_imageAccessor->getBufferView()->getCount();
    for ( auto i = 0; i < N; ++i ) {
        m_imageAccessor->set( i, ColorRGBA { 0, 0, 0, 1 } );
    }
}

bool RenderSceneRT::intersectPrim( const Ray3 &R, Int32 nodeId, Real minT, Real maxT, IntersectionResult &result ) noexcept
{
    Bool hasResult = false;

    const auto &node = m_acceleratedScene.nodes[ nodeId ];
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

        const auto &prim = m_acceleratedScene.primitives.primTree[ expanded.id ];
        if ( prim.isLeaf() ) {
            const auto idxOffset = prim.primitiveIndicesOffset;
            const auto primCount = prim.getPrimCount();
            for ( auto i = 0; i < primCount; ++i ) {
                const auto baseIdx = m_acceleratedScene.primitives.indexOffsets[ idxOffset + i ];

                const auto &v0 = m_acceleratedScene.primitives.triangles[ m_acceleratedScene.primitives.indices[ baseIdx + 0 ] ];
                const auto &v1 = m_acceleratedScene.primitives.triangles[ m_acceleratedScene.primitives.indices[ baseIdx + 1 ] ];
                const auto &v2 = m_acceleratedScene.primitives.triangles[ m_acceleratedScene.primitives.indices[ baseIdx + 2 ] ];

                const auto T = Triangle {
                    v0.position,
                    v1.position,
                    v2.position,
                };

                Real t;
                // TODO(hernan): Pre-compute inverse world ray to avoid doing it every loop
                if ( crimild::intersect( R, T, node.world, t ) ) {
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
            if ( crimild::intersect( R1, P, t ) ) {
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

bool RenderSceneRT::intersect( const Ray3 &R, IntersectionResult &result ) noexcept
{
    if ( m_acceleratedScene.nodes.empty() ) {
        return false;
    }

    Bool hasResult = false;

    Index frontier[ 64 ] = { 0 };
    Int32 currentIndex = 0;

    auto computeVolume = [ & ]( const auto &node, auto t0, auto t1 ) {
        const auto &material = m_acceleratedScene.materials[ node.materialIndex ];
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

        const auto &node = m_acceleratedScene.nodes[ nodeIdx ];

        switch ( node.type ) {
            case RTAcceleratedNode::Type::GROUP: {
                const auto B = Box {};
                Real t0, t1;
                if ( crimild::intersect( R, B, node.world, t0, t1 ) ) {
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
                if ( crimild::intersect( R, B, node.world, t0, t1 ) ) {
                    // TODO(hernan): when traversing optimized triangles, we need to keep
                    // track of the min/max times for the intersection above.
                    // TODO(hernan): Maybe by keeping track of those values I can also
                    // solve how CSG nodes will be evaluated, since they also need to
                    // save min/max times for intersections.
                    const auto primIdx = nodeIdx + 1;
                    if ( m_acceleratedScene.nodes[ primIdx ].type == RTAcceleratedNode::Type::PRIMITIVE_TRIANGLES ) {
                        if ( intersectPrim( R, primIdx, t0, t1, result ) ) {
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
                if ( crimild::intersect( R, S, node.world, t0, t1 ) ) {
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
                if ( crimild::intersect( R, B, node.world, t0, t1 ) ) {
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
                if ( crimild::intersect( R, C, node.world, t0, t1 ) ) {
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

crimild::Ray3 RenderSceneRT::getRay( Vector2f uv ) noexcept
{
    // Basic antialliasing by offseting the UV coordinates
    uv = uv + random::nextVector2( -1, 1 );
    uv = uv / Vector2 { getExtent().width - 1.0f, getExtent().height - 1.0f };

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

    return crimild::Ray3 {
        origin( ray ) + offset,
        cameraFocusDistance * direction( ray ) - offset,
    };
}

// Move to Mathematics
[[nodiscard]] static float reflectance( float cosTheta, float refIdx ) noexcept
{
    // Use Schlick's approximation for reflectance
    const auto r0 = ( 1 - refIdx ) / ( 1 + refIdx );
    const auto rr = r0 * r0;
    return rr + ( 1 - rr ) * pow( ( 1 - cosTheta ), 5 );
}

bool RenderSceneRT::scatter( const Ray3 &R, const IntersectionResult &result, Ray3 &scattered, ColorRGB &attenuation, Bool &shouldBounce ) noexcept
{
    shouldBounce = true;

    const auto &material = m_acceleratedScene.materials[ result.materialId ];

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

crimild::ColorRGB RenderSceneRT::getRayColor( const Ray3 &R, const ColorRGB &backgroundColor, int depth ) noexcept
{
    if ( depth == 0 ) {
        // If ray bounces too many times, it migth indicate that
        // no light (or only a tiny bit of it) reaches that point.
        return ColorRGB { 0, 0, 0 };
    }

    IntersectionResult result;
    if ( !intersect( R, result ) ) {
        // no intersection. Use background color
        return backgroundColor;
    }

    Ray3 scattered;
    ColorRGB attenuation;
    bool shouldBounce;
    if ( scatter( R, result, scattered, attenuation, shouldBounce ) ) {
        if ( shouldBounce ) {
            return attenuation * getRayColor( scattered, backgroundColor, depth - 1 );
        } else {
            return attenuation;
        }
    } else {
        return ColorRGB { 0, 0, 0 };
    }
}

void RenderSceneRT::updateImage( void ) noexcept
{
    const ColorRGB backgroundColor = getBackgroundColor();
    const int MAX_DEPTH = 10;
    const float H = getExtent().height;
    const float W = getExtent().width;
    const auto total = H * W;
    m_progress = 0.0f;
    for ( float y = 0; y < H; ++y ) {
        for ( float x = 0; x < W; ++x ) {
            if ( m_state != State::RUNNING ) {
                // Check termination every loop
                return;
            }
            const auto R = getRay( { x, y } );
            const auto color = getRayColor( R, backgroundColor, MAX_DEPTH );
            const size_t index = y * W + x;
            auto accum = m_sampleCount * rgb( m_imageAccessor->get< ColorRGBA >( index ) );
            accum = accum + color;
            accum = accum / float( m_sampleCount + 1 );
            m_imageAccessor->set( index, rgba( accum ) );
            m_progress = 100.0f * float( index ) / float( total );
        }
    }
    ++m_sampleCount;
}

void RenderSceneRT::render( crimild::Node *scene, Camera *camera ) noexcept
{
    if ( m_scene == nullptr ) {
        setup( scene, camera );
    }

    if ( m_done.valid() && ( m_done.wait_for( std::chrono::milliseconds( 0 ) ) != std::future_status::ready ) ) {
        return;
    }

    // Update device image
    auto &cmds = getCommandBuffer();
    cmds->reset();
    cmds->begin(
        {
            .pre = {
                // Transition target image to general layout so it can be written
                .imageMemoryBarriers = {
                    vulkan::ImageMemoryBarrier {
                        .srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                        .dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT,
                        .srcAccessMask = 0,
                        .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                        .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        .imageView = m_outputTarget->getImageView(),
                    },
                },
            },
        }
    );
    m_stagingBuffer->update();
    cmds->copy( m_stagingBuffer, m_outputTarget->getImageView() );
    cmds->end(
        {
            .post = {
                // Transition target image to shader read
                .imageMemoryBarriers = {
                    vulkan::ImageMemoryBarrier {
                        .srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT,
                        .dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                        .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                        .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
                        .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        .imageView = m_outputTarget->getImageView(),
                    },
                },
            },
        }
    );

    // Send to graphics queue even if no render pass is used.
    getRenderDevice()->submitGraphicsCommands( cmds );

    if ( m_state != State::RUNNING ) {
        return;
    }

    m_done = std::async(
        [ this ] {
            updateImage();
        }
    );
}
