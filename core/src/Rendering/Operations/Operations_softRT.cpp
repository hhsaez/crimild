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
#include "Mathematics/ColorRGBOps.hpp"
#include "Mathematics/Matrix4_operators.hpp"
#include "Mathematics/Normal3Ops.hpp"
#include "Mathematics/Random.hpp"
#include "Mathematics/Ray_apply.hpp"
#include "Mathematics/Sphere_normal.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Transformation_inverse.hpp"
#include "Mathematics/Vector3_isZero.hpp"
#include "Mathematics/easing.hpp"
#include "Mathematics/intersect.hpp"
#include "Mathematics/io.hpp"
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
#include "Visitors/FetchCameras.hpp"
#include "Visitors/IntersectWorld.hpp"
#include "Visitors/RTAcceleration.hpp"
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild;

#define CRIMILD_RT_SAMPLES_PER_FRAME 1

Bool boundCompare( SharedPointer< Node > &a, SharedPointer< Node > &b, Index axis )
{
    return a->getWorldBound()->getMin()[ axis ] < b->getWorldBound()->getMin()[ axis ];
}

Bool boundCompareX( SharedPointer< Node > &a, SharedPointer< Node > &b )
{
    return boundCompare( a, b, 0 );
}

Bool boundCompareY( SharedPointer< Node > &a, SharedPointer< Node > &b )
{
    return boundCompare( a, b, 1 );
}

Bool boundCompareZ( SharedPointer< Node > &a, SharedPointer< Node > &b )
{
    return boundCompare( a, b, 2 );
}

crimild::SharedPointer< Node > optimize( Array< SharedPointer< Node > > &nodes, Index start, Index end ) noexcept
{
    const auto span = end - start;
    if ( span == 1 ) {
        return nodes[ start ];
    }

    int axis = Random::generate< Int >( 0, 3 );
    auto comparator = axis == 0 ? boundCompareX : ( axis == 1 ? boundCompareY : boundCompareZ );

    auto group = crimild::alloc< Group >();

    if ( span == 2 ) {
        if ( comparator( nodes[ start ], nodes[ start + 1 ] ) ) {
            group->attachNode( nodes[ start ] );
            group->attachNode( nodes[ start + 1 ] );
        } else {
            group->attachNode( nodes[ start + 1 ] );
            group->attachNode( nodes[ start ] );
        }
    } else {
        const auto mid = start + span / 2;
        nodes.sort( comparator );
        Array< SharedPointer< Node > > left;
        for ( auto i = start; i < mid; ++i ) {
            left.add( nodes[ i ] );
        }

        Array< SharedPointer< Node > > right;
        for ( auto i = mid; i < end; ++i ) {
            right.add( nodes[ i ] );
        }

        left.sort( comparator );
        right.sort( comparator );

        group->attachNode( optimize( left, 0, left.size() ) );   //, start, mid ) );
        group->attachNode( optimize( right, 0, right.size() ) ); //nodes, mid, end ) );
    }

    return group;
}

crimild::SharedPointer< Node > crimild::framegraph::utils::optimize( Array< SharedPointer< Node > > &nodes ) noexcept
{
    nodes.each(
        []( auto n ) {
            n->perform( UpdateWorldState() );
        } );

    return ::optimize( nodes, 0, nodes.size() );
}

[[nodiscard]] Vector3 randomInUnitSphere( void ) noexcept
{
    while ( true ) {
        const auto v = Vector3 {
            Random::generate< Real >( -1, 1 ),
            Random::generate< Real >( -1, 1 ),
            Random::generate< Real >( -1, 1 ),
        };

        if ( lengthSquared( v ) >= 1 ) {
            continue;
        } else {
            return v;
        }
    }
}

[[nodiscard]] Vector3 randomInUnitDisk( void ) noexcept
{
    while ( true ) {
        const auto v = Vector3 {
            Random::generate< Real >( -1, 1 ),
            Random::generate< Real >( -1, 1 ),
            Real( 0 ),
        };

        if ( lengthSquared( v ) >= 1 ) {
            continue;
        } else {
            return v;
        }
    }
}

[[nodiscard]] Vector3 randomUnitVector( void ) noexcept
{
    return normalize( randomInUnitSphere() );
}

[[nodiscard]] Vector3 randomInHemisphere( const Normal3 &N ) noexcept
{
    const auto inUnitSphere = randomInUnitSphere();
    if ( dot( inUnitSphere, N ) > 0 ) {
        return inUnitSphere;
    } else {
        return -inUnitSphere;
    }
}

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

[[nodiscard]] Bool intersect( const Ray3 &R, const RTAcceleration::Result &scene, UInt32 nodeId, IntersectionResult &result ) noexcept
{
    const auto &node = scene.nodes[ nodeId ];

    switch ( node.type ) {
        case RTAcceleratedNode::Type::GROUP: {
            auto ret = false;
            for ( auto i = 0; i < node.childCount; ++i ) {
                ret = intersect( R, scene, node.firstChildIndex + i, result ) || ret;
            }
            return ret;
        }

        case RTAcceleratedNode::Type::PRIMITIVE_SPHERE: {
            const auto S = Sphere {};
            const auto R1 = Ray3 {
                .o = point3( xyz( node.invWorld * vector4( R.o, 1 ) ) ),
                .d = xyz( node.invWorld * vector4( R.d, 0 ) ),
            };

            Real t0 = numbers::POSITIVE_INFINITY;
            Real t1 = numbers::POSITIVE_INFINITY;
            auto hasResult = false;
            if ( intersect( R1, S, t0, t1 ) ) {
                if ( t0 >= numbers::EPSILON && t0 <= result.t ) {
                    result.t = t0;
                    result.materialId = node.index;
                    hasResult = true;
                }

                if ( t1 >= numbers::EPSILON && t1 <= result.t ) {
                    result.t = t1;
                    result.materialId = node.index;
                    hasResult = true;
                }
            }
            return hasResult;
        }

        default: {
            return false;
        }
    }
}

[[nodiscard]] Bool intersect( const Ray3 &R, const RTAcceleration::Result &scene, IntersectionResult &result ) noexcept
{
    if ( scene.nodes.empty() ) {
        return false;
    }

    return intersect( R, scene, 0, result );
}

[[nodiscard]] Bool intersectNR( const Ray3 &R, const RTAcceleration::Result &scene, IntersectionResult &result ) noexcept
{
    if ( scene.nodes.empty() ) {
        return false;
    }

    Bool hasResult = false;

    utils::traverseNonRecursive(
        scene,
        [ & ]( const auto &node, auto index ) -> Bool {
            const auto R1 = Ray3 {
                .o = point3( xyz( node.invWorld * vector4( R.o, 1 ) ) ),
                .d = xyz( node.invWorld * vector4( R.d, 0 ) ),
            };

            auto resolveCSG = [ & ]( auto t0, auto t1 ) {
                if ( node.parentIndex < 0 ) {
                    return false;
                }

                auto parentIndex = node.parentIndex;
                while ( parentIndex >= 0 ) {
                    const auto &parent = scene.nodes[ node.parentIndex ];
                    if ( parent.type == RTAcceleratedNode::Type::CSG_UNION ) {
                        break;
                    } else if ( parent.type == RTAcceleratedNode::Type::CSG_INTERSECTION ) {
                        break;
                    } else if ( parent.type == RTAcceleratedNode::Type::CSG_DIFFERENCE ) {
                        break;
                    } else {
                        parentIndex = scene.nodes[ parentIndex ].parentIndex;
                    }
                }

                if ( parentIndex < 0 ) {
                    return false;
                }

                const auto &parent = scene.nodes[ parentIndex ];
                if ( parent.type == RTAcceleratedNode::Type::CSG_UNION ) {
                    const auto t = min( t0, t1 );
                    if ( t < result.t ) {
                        result.t = t0;
                        result.materialId = node.index;
                        hasResult = true;
                    }
                    return true;
                } else if ( parent.type == RTAcceleratedNode::Type::CSG_INTERSECTION ) {
                    const auto t = min( t0, t1 );
                    if ( t > result.t ) {
                        result.t = t0;
                        result.materialId = node.index;
                        hasResult = true;
                    }
                    return true;
                } else if ( parent.type == RTAcceleratedNode::Type::CSG_DIFFERENCE ) {
                    const auto t = min( t0, t1 );
                    if ( t > result.t ) {
                        result.t = t0;
                        result.materialId = node.index;
                        hasResult = true;
                    }
                    return true;
                }

                return false;
            };

            switch ( node.type ) {
                case RTAcceleratedNode::Type::GROUP: {
                    const auto S = Sphere {};
                    Real t0, t1;
                    return intersect( R1, S, t0, t1 );
                }
                case RTAcceleratedNode::Type::PRIMITIVE_SPHERE: {
                    const auto S = Sphere {};
                    Real t0 = numbers::POSITIVE_INFINITY;
                    Real t1 = numbers::POSITIVE_INFINITY;
                    if ( intersect( R1, S, t0, t1 ) ) {
                        if ( resolveCSG( t0, t1 ) ) {
                            return true;
                        }

                        if ( t0 >= numbers::EPSILON && t0 <= result.t ) {
                            result.t = t0;
                            result.materialId = node.index;
                            hasResult = true;
                        }

                        if ( t1 >= numbers::EPSILON && t1 <= result.t ) {
                            result.t = t1;
                            result.materialId = node.index;
                            hasResult = true;
                        }
                    }
                    return true;
                }
                case RTAcceleratedNode::Type::PRIMITIVE_BOX: {
                    const auto B = Box {};
                    Real t0, t1;
                    if ( intersect( R1, B, t0, t1 ) ) {
                        if ( resolveCSG( t0, t1 ) ) {
                            return true;
                        }

                        if ( t0 >= numbers::EPSILON && t0 <= result.t ) {
                            result.t = t0;
                            result.materialId = node.index;
                            hasResult = true;
                        }

                        if ( t1 >= numbers::EPSILON && t1 <= result.t ) {
                            result.t = t1;
                            result.materialId = node.index;
                            hasResult = true;
                        }
                    }
                    return true;
                }
                case RTAcceleratedNode::Type::CSG_UNION:
                case RTAcceleratedNode::Type::CSG_INTERSECTION:
                case RTAcceleratedNode::Type::CSG_DIFFERENCE: {
                    const auto S = Sphere {};
                    Real t0, t1;
                    return intersect( R1, S, t0, t1 );
                }

                default: {
                    return false;
                }
            }
        } );

    return hasResult;
}

[[nodiscard]] ColorRGB rayColor( const Ray3 &R, const RTAcceleration::Result &scene, const ColorRGB &backgroundColor, Int32 depth ) noexcept
{
    auto ret = backgroundColor;

    if ( depth <= 0 ) {
        return ColorRGB::Constants::BLACK;
    }

    IntersectionResult result;
    if ( intersectNR( R, scene, result ) ) {
        if ( result.materialId >= 0 ) {
            return scene.materials[ result.materialId ].albedo;
        }
    }

#if 0

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
            Real scale = settings->get< Real >( "rt.scale", 1 );
            m_width = scale * settings->get< Int32 >( "video.width", 320 );
            m_height = scale * settings->get< Int32 >( "video.height", 240 );
            m_bpp = 4;
            m_aspectRatio = Real( m_width ) / Real( m_height );
            m_tileSize = settings->get< Int32 >( "rt.tile_size", 64 );
            m_workerCount = std::max( 1, settings->get< Int32 >( "rt.workers", std::thread::hardware_concurrency() ) );

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
            // clear all pending jobs
            m_jobs.clear();

            // wait for workers
            reset();

            m_workers.clear();
        }

    private:
        Bool execute( void ) noexcept
        {
            if ( !isDone() ) {
                return true;
            }

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

                ShallowCopy copy;
                scene->perform( copy );
                m_scene = copy.getResult< Node >();
                m_scene->perform( UpdateWorldState() );

                RTAcceleration accelerate;
                m_scene->perform( accelerate );
                m_acceleratedScene = accelerate.getResult();

                return m_scene;
            }();

            if ( !scene ) {
                std::cout << "no scene" << std::endl;
                exit( -1 );
                return true;
            }

            // Reset all jobs and wait for workers to finish
            reset();

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

                                    color = color + rayColor( ray, self->m_acceleratedScene, backgroundColor, bounces );
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

            settings->set( "rt.samples.count", sampleCount + CRIMILD_RT_SAMPLES_PER_FRAME );

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
