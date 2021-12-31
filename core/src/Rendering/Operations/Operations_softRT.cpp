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

#include "Rendering/Operations/Operations_softRT.hpp"

#include "Components/MaterialComponent.hpp"
#include "Mathematics/Box_normal.hpp"
#include "Mathematics/ColorRGBOps.hpp"
#include "Mathematics/ColorRGB_isZero.hpp"
#include "Mathematics/Cylinder_normal.hpp"
#include "Mathematics/Matrix4_operators.hpp"
#include "Mathematics/Normal3Ops.hpp"
#include "Mathematics/Random.hpp"
#include "Mathematics/Ray_apply.hpp"
#include "Mathematics/Sphere_normal.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Transformation_inverse.hpp"
#include "Mathematics/Vector2Ops.hpp"
#include "Mathematics/Vector3_isZero.hpp"
#include "Mathematics/easing.hpp"
#include "Mathematics/intersect.hpp"
#include "Mathematics/io.hpp"
#include "Mathematics/isNaN.hpp"
#include "Mathematics/min.hpp"
#include "Mathematics/reflect.hpp"
#include "Mathematics/refract.hpp"
#include "Mathematics/swizzle.hpp"
#include "Rendering/Image.hpp"
#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"
#include "Rendering/Materials/PrincipledVolumeMaterial.hpp"
#include "Rendering/ScenePass.hpp"
#include "SceneGraph/CSGNode.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/BinTreeScene.hpp"
#include "Visitors/FetchCameras.hpp"
#include "Visitors/IntersectWorld.hpp"
#include "Visitors/RTAcceleration.hpp"
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild;

#define CRIMILD_RT_SAMPLES_PER_FRAME 1

[[nodiscard]] Real reflectance( Real cosTheta, Real refIdx ) noexcept
{
    // Use Schlick's approximation for reflectance
    const auto r0 = ( 1 - refIdx ) / ( 1 + refIdx );
    const auto rr = r0 * r0;
    return rr + ( 1 - rr ) * pow( ( 1 - cosTheta ), 5 );
}

struct IntersectionResult {
    Int32 materialId = -1;
    Real t = numbers::POSITIVE_INFINITY;
    Point3 point;
    Normal3 normal;
    Bool frontFace;

    inline void setFaceNormal( const Ray3 &R, const Normal3 &N ) noexcept
    {
        frontFace = dot( direction( R ), N ) < 0;
        normal = frontFace ? N : -N;
    }
};

[[nodiscard]] Bool intersectPrim( const Ray3 &R, const RTAcceleration::Result &scene, Int32 nodeId, IntersectionResult &result ) noexcept
{
    Bool hasResult = false;

    const auto &node = scene.nodes[ nodeId ];
    const auto primIdx = node.primitiveIndex;
    if ( primIdx < 0 ) {
        return false;
    }

    Int32 frontier[ 64 ] = { primIdx, 0 };
    Int32 currentIdx = 0;

    while ( currentIdx >= 0 ) {
        const auto primIdx = frontier[ currentIdx-- ];
        const auto &prim = scene.primitives.primTree[ primIdx ];
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
            if ( prim.aboveChild > ( primIdx + 1 ) ) {
                frontier[ ++currentIdx ] = prim.getAboveChild();
            }
            frontier[ ++currentIdx ] = primIdx + 1;
        }
    }

    return hasResult;
}

[[nodiscard]] Bool intersectNR( const Ray3 &R, const RTAcceleration::Result &scene, IntersectionResult &result ) noexcept
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
                    frontier[ ++currentIndex ] = nodeIdx + 1;
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

            case RTAcceleratedNode::Type::PRIMITIVE_TRIANGLES: {
                if ( intersectPrim( R, scene, nodeIdx, result ) ) {
                    hasResult = true;
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

// [[nodiscard]] ColorRGB rayColor( const Ray3 &R, const RTAcceleration::Result &scene, const ColorRGB &backgroundColor, Int32 depth ) noexcept
// {
//     auto ret = backgroundColor;

//     if ( depth <= 0 ) {
//         return ColorRGB::Constants::BLACK;
//     }

//     IntersectionResult result;
//     if ( intersectR( R, scene, result ) ) {
//         if ( result.materialId >= 0 ) {
//             return scene.materials[ result.materialId ].albedo;
//         }
//     }

//     return ret;
// }

[[nodiscard]] ColorRGB rayColor( const Ray3 &R, SharedPointer< Node > &scene, const ColorRGB &backgroundColor, Int32 depth ) noexcept
{
    auto ret = backgroundColor;

    if ( depth <= 0 ) {
        return ColorRGB::Constants::BLACK;
    }

#if 1
    auto results = IntersectWorld::Results {};
    scene->perform( IntersectWorld( R, results ) );
    if ( !results.empty() ) {
        auto res = results.first();

        const auto firstMaterial = res.geometry->getComponent< MaterialComponent >()->first();
        if ( firstMaterial->getClassName() == materials::PrincipledBSDF::__CLASS_NAME ) {
            const auto material = static_cast< materials::PrincipledBSDF * >( firstMaterial );
            return material->getAlbedo();
        } else {
            return ColorRGB { 1, 0, 1 };
        }
    }

#else

    auto results = IntersectWorld::Results {};
    scene->perform( IntersectWorld( R, results ) );
    if ( !results.empty() ) {
        auto res = results.first();
        Ray3 scattered;
        ColorRGB attenuation;

        const auto firstMaterial = res.geometry->getComponent< MaterialComponent >()->first();
        if ( firstMaterial->getClassName() == materials::PrincipledVolume::__CLASS_NAME ) {
            const auto material = static_cast< materials::PrincipledVolume * >( firstMaterial );
            const auto &albedo = material->getAlbedo();
            scattered = Ray3 { res.point, randomInUnitSphere() };
            attenuation = albedo;
        } else {
            const auto material = static_cast< materials::PrincipledBSDF * >( firstMaterial );
            const auto albedo = material->getAlbedo();
            const auto metallic = material->getMetallic();
            const auto roughness = material->getRoughness();
            const auto transmission = material->getTransmission();
            const auto ior = material->getIndexOfRefraction();
            const auto emissive = material->getEmissive();

            if ( !isZero( transmission ) ) {
                // Transmissive
                attenuation = ColorRGB::Constants::WHITE;
                const auto refractionRatio = res.frontFace ? ( Real( 1 ) / ior ) : ior;
                const auto dir = normalize( direction( R ) );
                double cosTheta = min( dot( -dir, res.normal ), Real( 1 ) );
                double sinTheta = sqrt( Real( 1 ) - cosTheta * cosTheta );
                const auto cannotRefract = refractionRatio * sinTheta > 1;
                const auto scatteredDirection = [ & ] {
                    if ( cannotRefract || reflectance( cosTheta, refractionRatio ) > Random::generate< Real >() ) {
                        return reflect( dir, res.normal );
                    } else {
                        return refract( dir, res.normal, refractionRatio );
                    }
                }();
                scattered = Ray3 { res.point, scatteredDirection };
            } else if ( !isZero( metallic ) ) {
                // Metallic model
                const auto reflected = reflect( normalize( direction( R ) ), res.normal );
                scattered = Ray3 { res.point, reflected + roughness * randomInUnitSphere() };
                attenuation = albedo;
                if ( dot( direction( scattered ), res.normal ) <= 0 ) {
                    return ColorRGB::Constants::BLACK;
                }
            } else if ( !isEqual( emissive, ColorRGB::Constants::BLACK ) ) {
                return emissive;
            } else {
                // Lambertian model
                // TODO(hernan): use randomInHemisphere instead?
                auto scatterDirection = vector3( res.normal ) + randomUnitVector();
                if ( isZero( scatterDirection ) ) {
                    scatterDirection = vector3( res.normal );
                }
                scattered = Ray3 { res.point, scatterDirection };
                attenuation = albedo;
            }
        }

        return attenuation * rayColor( scattered, scene, backgroundColor, depth - 1 );
    }

#endif

    return ret;
}

namespace crimild {

    /**
     * \brief Renders a scene using ray tracing in CPU
     * 
     * How does it works:
     * - Convert the scene to a binary tree
     * - Accelerate the binary tree and transform it into a linear tree
     * - Traverse the tree non-recursively
     */
    class SoftRT : public ScenePass {
    private:
        using Job = std::function< void( void ) >;

    public:
        SoftRT( void ) noexcept
        {
            setName( "softRT" );

            this->apply = [ self = this ]( auto, auto ) {
                return self->execute();
            };

            auto settings = Simulation::getInstance()->getSettings();
            auto quality = settings->get( "rt.quality", "240p" );
            if ( quality == "240p" ) {
                m_width = 320;
                m_height = 240;
                m_workerCount = std::max( 1, settings->get< Int32 >( "rt.workers", 2 ) );
            } else {
                m_width = settings->get< Int32 >( "video.width", 320 );
                m_height = settings->get< Int32 >( "video.height", 240 );
                m_workerCount = std::max( 1, settings->get< Int32 >( "rt.workers", std::thread::hardware_concurrency() ) );
            }

            m_aspectRatio = Real( m_width ) / Real( m_height );
            m_bpp = 4;

            m_tileSize = settings->get< Int32 >( "rt.tile_size", 64 );

            m_image = crimild::alloc< Image >();
            m_image->extent = Extent3D {
                .width = Real32( m_width ),
                .height = Real32( m_height ),
            };
            m_image->format = crimild::utils::getFormat< ColorRGBA >();
            m_image->setBufferView(
                [ & ] {
                    auto buffer = crimild::alloc< Buffer >( Array< ColorRGBA >( m_width * m_height ) );
                    auto bufferView = crimild::alloc< BufferView >( BufferView::Target::IMAGE, buffer, 0, sizeof( ColorRGBA ) );
                    bufferView->setUsage( BufferView::Usage::DYNAMIC );
                    return bufferView;
                }() );

            m_imageAaccessor = crimild::alloc< BufferAccessor >( m_image->getBufferView(), 0, sizeof( ColorRGBA ) );

            m_progress = m_width * m_height;

            settings->set( "rt.samples.count", 0 );

            writes( { m_image } );
            produces( { m_image } );
        }

        virtual ~SoftRT( void ) noexcept
        {
            m_running = false;

            // clear all pending jobs
            // m_jobs.clear();

            // wait for workers
            // reset();

            for ( auto &w : m_workers ) {
                w.join();
            }

            m_workers.clear();
        }

    private:
        // Maybe rename to SampleInfo?
        struct RayInfo {
            Ray3 ray;
            ColorRGB sampleColor;
            ColorRGB accumColor;
            Vector2 uv;
            Int32 bounces;
            Int32 samples;
        };

        std::vector< RayInfo > m_rays;

        void initializeRays( void ) noexcept
        {
            // Assumes images are 2D
            for ( Real y = 0; y < m_height; ++y ) {
                for ( Real x = 0; x < m_width; ++x ) {
                    m_rays.push_back(
                        RayInfo {
                            .ray = Ray3 {},
                            .sampleColor = ColorRGB { 1, 1, 1 },
                            .accumColor = ColorRGB { 0, 0, 0 },
                            .uv = Vector2 { x, y },
                            .bounces = 0,
                            .samples = 0,
                        } );
                }
            }

            // TODO(hernan): Add a setting for suffling rays.
            if ( getSettings()->get< Bool >( "rt.shuffle", true ) ) {
                std::shuffle( m_rays.begin(), m_rays.end(), random::defaultGenerator() );
            }
        }

        [[nodiscard]] inline Settings *getSettings( void ) noexcept
        {
            return Simulation::getInstance()->getSettings();
        }

        void onSampleCompleted( RayInfo &rayInfo ) noexcept
        {
            // Check for NaN values before accumulating color. That way
            // we avoid "dead" pixels (either black or white) due to
            // invalid values.
            rayInfo.accumColor = rayInfo.accumColor + ( isNaN( rayInfo.sampleColor ) ? ColorRGB { 0, 0, 0 } : rayInfo.sampleColor );
            rayInfo.samples++;

            ColorRGB color = rayInfo.accumColor / Real( rayInfo.samples );
            m_imageAaccessor->set( rayInfo.uv.y * m_width + rayInfo.uv.x, rgba( color ) );

            rayInfo.sampleColor = ColorRGB { 1, 1, 1 };
            rayInfo.bounces = 0;
        }

        [[nodiscard]] Ray3 getRay( RayInfo &rayInfo ) noexcept
        {
            if ( rayInfo.bounces > 0 ) {
                // Not the first bounce, so info already contains a valid ray
                return rayInfo.ray;
            }

            // First bounce. Generate a new ray from the camera

            // Basic antialliasing by offseting the UV coordinates
            auto uv = rayInfo.uv;
            uv = uv + random::nextVector2( -1, 1 );
            uv = uv / Vector2 { m_width - 1.0f, m_height - 1.0f };

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

        bool scatter( const RTAcceleration::Result &scene, const Ray3 &R, const IntersectionResult &result, Ray3 &scattered, ColorRGB &attenuation, Bool &shouldBounce ) noexcept
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
            if ( isZero( scatteredDirection ) ) {
                scatteredDirection = vector3( result.normal );
            }
            scattered = Ray3 { result.point, scatteredDirection };
            attenuation = material.albedo;
            return true;
        }

        void doSampleBounce( RayInfo &rayInfo, const RTAcceleration::Result &scene ) noexcept
        {
            if ( rayInfo.bounces > 50 ) {
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
                // TODO(hernan): Avoid getting settings every time
                auto settings = getSettings();
                auto backgroundColor = ColorRGB {
                    settings->get< Real >( "rt.background_color.r", 0.5f ),
                    settings->get< Real >( "rt.background_color.g", 0.7f ),
                    settings->get< Real >( "rt.background_color.b", 1.0f ),
                };
                rayInfo.sampleColor = rayInfo.sampleColor * backgroundColor;
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

        Bool execute( void ) noexcept
        {
            if ( !m_workers.empty() ) {
                return true;
            }

            // if ( !isDone() ) {
            //     return true;
            // }

            auto settings = Simulation::getInstance()->getSettings();

            auto scene = [ & ]() -> SharedPointer< Node > {
                if ( m_scene != nullptr ) {
                    return m_scene;
                }

                // We want to avoid blocking the main thread so we can get a progressive result,
                // but scenes can be updated in between samples. During an update, a scene could
                // reset its transform during the world update process, which might also affect
                // the camera. So, the easiest way to avoid artifacts during rendering is to
                // clone the entire scene. This is very much like collecting renderables and
                // forwarding them to the GPU, just much slower.
                // TODO: should I build an acceleration structor (BVH?) as part of this process?
                // TODO: maybe just collect geometries...
                auto scene = Simulation::getInstance()->getScene();
                if ( !scene ) {
                    return nullptr;
                }

                CRIMILD_LOG_DEBUG( "Cloning scene before rendering" );

                scene->perform( UpdateWorldState() );

                ShallowCopy copy;
                scene->perform( copy );
                m_scene = copy.getResult< Node >();
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

                return m_scene;
            }();

            if ( !scene ) {
                std::cout << "no scene" << std::endl;
                exit( -1 );
                return true;
            }

            // Reset all jobs and wait for workers to finish
            // reset();

            auto camera = [ & ]() -> SharedPointer< Camera > {
                if ( m_camera != nullptr ) {
                    return m_camera;
                }

                CRIMILD_LOG_DEBUG( "Fetching cameras" );

                FetchCameras fetchCameras;
                scene->perform( fetchCameras );
                if ( !fetchCameras.hasCameras() ) {
                    return nullptr;
                }

                m_camera = crimild::retain( fetchCameras.anyCamera() );
                return m_camera;
            }();

            if ( m_camera == nullptr ) {
                std::cout << "no cameras" << std::endl;
                exit( -1 );
                return true;
            }

#if 1
            initializeRays();

            m_workers.clear();
            for ( auto i = 0; i < m_workerCount; ++i ) {
                m_workers.push_back(
                    std::thread(
                        [ self = this, i ] {
                            Size next = i;
                            while ( self->m_running ) {
                                // It is possible that id clashing happens for two or more
                                // workers, but I guess that's ok for now since only one of
                                // them will actually write the final color in the image
                                Size rayID = next;
                                next += self->m_workerCount;
                                if ( next >= self->m_rays.size() ) {
                                    next = i;
                                }
                                if ( rayID < self->m_rays.size() ) {
                                    auto &ray = self->m_rays[ rayID ];
                                    self->doSampleBounce( ray, self->m_acceleratedScene );
                                }
                            }
                        } ) );
            }

#else

            auto maxSamples = settings->get< UInt32 >( "rt.samples.max", 5000 );
            auto sampleCount = settings->get< UInt32 >( "rt.samples.count", 1 );
            auto bounces = settings->get< UInt32 >( "rt.bounces", 10 );
            auto focusDist = settings->get< Real >( "rt.focusDist", Real( 10 ) ); // move to camera
            auto aperture = settings->get< Real >( "rt.aperture", Real( 0 ) );    // move to camera
            auto backgroundColor = ColorRGB {
                settings->get< Real >( "rt.background_color.r", 0.5f ),
                settings->get< Real >( "rt.background_color.g", 0.7f ),
                settings->get< Real >( "rt.background_color.b", 1.0f ),
            };
            auto useScanline = settings->get< Bool >( "rt.use_scanline", true );

            const auto cameraFocusDistance = focusDist; //camera->getFocusDistance();
            const auto cameraAperture = aperture;       //camera->getAperture();
            const auto cameraLensRadius = 0.5f * cameraAperture;
            const auto cameraRight = right( camera->getWorld() );
            const auto cameraUp = up( camera->getWorld() );

            auto job = [ & ]( UInt32 x, UInt32 y ) {
                return [ self = this,
                         scene,
                         camera,
                         cameraFocusDistance,
                         cameraLensRadius,
                         cameraUp,
                         cameraRight,
                         sampleCount,
                         bounces,
                         backgroundColor,
                         x,
                         y ]() {
                    for ( auto dy = 0; dy < self->m_tileSize; ++dy ) {
                        if ( y + dy >= self->m_height ) {
                            break;
                        }
                        for ( auto dx = 0; dx < self->m_tileSize; ++dx ) {
                            if ( x + dx >= self->m_width ) {
                                break;
                            }

                            auto color = ColorRGB::Constants::BLACK;
                            for ( auto s = 0l; s < CRIMILD_RT_SAMPLES_PER_FRAME; ++s ) {
                                const auto u = ( ( x + dx ) + Random::generate< Real >() ) / Real( self->m_width - 1 );
                                const auto v = ( ( y + dy ) + Random::generate< Real >() ) / Real( self->m_height - 1 );

                                Ray3 ray;
                                if ( camera->getPickRay( u, v, ray ) ) {
                                    // Calculate depth-of-field based on camera properties
                                    const auto rd = cameraLensRadius * randomInUnitDisk();
                                    const auto offset = cameraRight * rd.x + cameraUp * rd.y;
                                    ray = Ray3 {
                                        origin( ray ) + offset,
                                        cameraFocusDistance * direction( ray ) - offset,
                                    };

    #if 0
                                    color = color + rayColor( ray, self->m_acceleratedScene, backgroundColor, bounces );
    #else
                                    color = color + rayColor( ray, self->m_scene, backgroundColor, bounces );
    #endif
                                }
                            }

                            const Index idx = ( y + dy ) * self->m_width + ( x + dx );
                            const auto prevColor = self->m_imageAaccessor->get< ColorRGBA >( idx ) * sampleCount;
                            self->m_imageAaccessor->set( idx, ( prevColor + rgba( color ) ) / ( sampleCount + CRIMILD_RT_SAMPLES_PER_FRAME ) );

                            std::this_thread::yield();
                        }
                    }
                };
            };

            if ( useScanline ) {
                for ( auto y = 0; y < m_height; y += m_tileSize ) {
                    for ( auto x = 0; x < m_width; x += m_tileSize ) {
                        m_jobs.push_back( job( x, y ) );
                    }
                }
            } else {
                // Render tiles in a spiral, so center ones show up first
                const Int32 MAX_X = ( m_width / m_tileSize ) / 2;
                const Int32 MAX_Y = ( m_height / m_tileSize ) / 2;
                Int32 x = 0;
                Int32 y = 0;
                Int32 dx = 0;
                Int32 dy = -1;
                Int32 t = max( 2 * MAX_X, 2 * MAX_Y );
                const Int32 N = t * t;
                for ( auto i = 0l; i < N; i++ ) {
                    if ( -MAX_X <= x && x <= MAX_X && -MAX_Y <= y && y <= MAX_Y ) {
                        const auto jx = x * m_tileSize + m_width / 2 - m_tileSize;
                        const auto jy = y * m_tileSize + m_height / 2;
                        m_jobs.push_back( job( jx, jy ) );
                    }
                    if ( ( x == y ) || ( ( x < 0 ) && ( x == -y ) ) || ( ( x > 0 ) && ( x == 1 - y ) ) ) {
                        t = dx;
                        dx = -dy;
                        dy = t;
                    }
                    x += dx;
                    y += dy;
                }
            }

            // launch all threads
            for ( auto i = 0; i < m_workerCount; ++i ) {
                m_workers.push_back(
                    std::move(
                        std::thread(
                            [ & ] {
                                Job job;
                                while ( getNextJob( job ) ) {
                                    job();
                                }
                                reportProgress();
                            } ) ) );
            }

            CRIMILD_LOG_INFO( "RT Samples: ", sampleCount );

#endif

            return true;
        }

        void reset( void ) noexcept
        {
            // wait for all threads to finish
            for ( auto &worker : m_workers ) {
                worker.join();
            }

            m_nextJobIdx = 0;
            m_jobs.clear();
            m_workers.clear();

            m_progress = 0;

            auto settings = Simulation::getInstance()->getSettings();
            auto sampleCount = settings->get< UInt32 >( "rt.samples.count", 0 );

            if ( sampleCount == 0 ) {
                // Reset colors
                for ( auto i = 0l; i < m_width * m_height; ++i ) {
                    m_imageAaccessor->set( i, ColorRGBA { 0, 0, 0, 1 } );
                }
            }

            // settings->set( "rt.samples.count", sampleCount + CRIMILD_RT_SAMPLES_PER_FRAME );

            if ( Simulation::getInstance()->getSettings()->get< Bool >( "rt.dynamic_scene", false ) ) {
                // Scene is dynamic, so reset cached scene and camera
                m_scene = nullptr;
                m_camera = nullptr;
            }
        }

        Bool getNextJob( Job &nextJob ) noexcept
        {
            std::lock_guard< std::mutex > lock( m_mutex );

            if ( m_nextJobIdx >= m_jobs.size() ) {
                return false;
            }

            nextJob = m_jobs[ m_nextJobIdx++ ];
            return true;
        }

        void reportProgress( void ) noexcept
        {
            std::lock_guard< std::mutex > lock( m_progressMutex );
            m_progress++;
        }

        Bool isDone( void ) noexcept
        {
            std::lock_guard< std::mutex > lock( m_progressMutex );
            return m_progress >= m_workerCount;
        }

    private:
        Int32 m_width;
        Int32 m_height;
        Int32 m_bpp;
        Real m_aspectRatio;
        Int32 m_tileSize;
        Int32 m_workerCount;
        Bool m_running = true;

        RTAcceleration::Result m_acceleratedScene;

        SharedPointer< Node > m_scene;
        SharedPointer< Camera > m_camera;

        SharedPointer< Image > m_image;
        SharedPointer< BufferAccessor > m_imageAaccessor;

        std::vector< Job > m_jobs;
        Index m_nextJobIdx = 0;

        std::vector< std::thread > m_workers;
        std::mutex m_mutex;

        Size m_progress = 0l;
        std::mutex m_progressMutex;
    };
}

SharedPointer< FrameGraphOperation > crimild::framegraph::softRT( void ) noexcept
{
    return crimild::alloc< SoftRT >();
}
