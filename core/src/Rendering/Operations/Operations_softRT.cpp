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
#include "Rendering/ScenePass.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/IntersectWorld.hpp"
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild;

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

[[nodiscard]] ColorRGB rayColor( const Ray3 &R, Node *scene, const ColorRGB &backgroundColor, Int32 depth ) noexcept
{
    auto ret = backgroundColor;

    if ( depth <= 0 ) {
        return ColorRGB::Constants::BLACK;
    }

    auto results = IntersectWorld::Results {};
    scene->perform( IntersectWorld( R, results ) );
    if ( !results.empty() ) {
        auto res = results.first();
        const auto material = static_cast< materials::PrincipledBSDF * >( res.geometry->getComponent< MaterialComponent >()->first() );
        const auto albedo = material->getAlbedo();
        const auto metallic = material->getMetallic();
        const auto roughness = material->getRoughness();
        const auto transmission = material->getTransmission();
        const auto ior = material->getIndexOfRefraction();
        const auto emissive = material->getEmissive();

        Ray3 scattered;
        ColorRGB attenuation;

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

        return attenuation * rayColor( scattered, scene, backgroundColor, depth - 1 );
    }

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
            m_width = settings->get< Int32 >( "video.width", 320 );
            m_height = settings->get< Int32 >( "video.height", 240 );
            m_bpp = 4;
            m_aspectRatio = Real( m_width ) / Real( m_height );
            m_samples = settings->get< Int32 >( "rt.samples", 1 );
            m_depth = settings->get< Int32 >( "rt.depth", 10 );
            m_tileSize = settings->get< Int32 >( "rt.tile_size", 64 );
            m_workerCount = std::max( 1, settings->get< Int32 >( "rt.workers", std::thread::hardware_concurrency() ) );
            m_backgroundColor = ColorRGB {
                settings->get< Real >( "rt.background_color.r", 0.5f ),
                settings->get< Real >( "rt.background_color.g", 0.7f ),
                settings->get< Real >( "rt.background_color.b", 1.0f ),
            };

            m_sampleCount = -m_samples;

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

            writes( { m_image } );
            produces( { m_image } );
        }

        virtual ~SoftRT( void ) noexcept
        {
            m_jobs.clear();
            reset();
        }

    private:
        Bool execute( void ) noexcept
        {
            if ( m_nextJobIdx < m_jobs.size() ) {
                // Because scenes are not immutable, any rendering operation must be done here.
                // Otherwise, we'll see some artifacts because nodes might change positions and
                // boundings might be reset during scene traversals.
                // TODO(hernan): In order to fully unlock the main thread, we need to make a copy
                // of the scene to render
                std::vector< std::thread > workers;
                for ( auto i = 0l; i < m_workerCount; ++i ) {
                    workers.push_back(
                        std::move(
                            std::thread(
                                [ & ] {
                                    Job job;
                                    if ( getNextJob( job ) ) {
                                        job();
                                    }
                                } ) ) );
                }

                for ( auto &worker : workers ) {
                    worker.join();
                }

                return true;
            }

            auto scene = Simulation::getInstance()->getScene();
            if ( !scene ) {
                return true;
            }

            auto camera = Camera::getMainCamera();
            if ( !camera ) {
                return true;
            }

            // TODO: Is this required?
            scene->perform( UpdateWorldState() );

            // Reset all jobs and wait for workers to finish
            reset();

            for ( auto y = 0; y < m_height; y += m_tileSize ) {
                for ( auto x = 0; x < m_width; x += m_tileSize ) {
                    m_jobs.push_back(
                        [ self = this, scene, camera, x, y ] {
                            for ( auto dy = 0; dy < self->m_tileSize; ++dy ) {
                                if ( y + dy >= self->m_height ) {
                                    break;
                                }
                                for ( auto dx = 0; dx < self->m_tileSize; ++dx ) {
                                    if ( x + dx >= self->m_width ) {
                                        break;
                                    }

                                    auto color = ColorRGB::Constants::BLACK;
                                    for ( auto s = 0l; s < self->m_samples; ++s ) {
                                        Ray3 ray;
                                        const auto u = ( ( x + dx ) + Random::generate< Real >() ) / Real( self->m_width - 1 );
                                        const auto v = ( ( y + dy ) + Random::generate< Real >() ) / Real( self->m_height - 1 );
                                        if ( camera->getPickRay( u, v, ray ) ) {
                                            // TODO: defocus blur
                                            // const auto aperture = 2.0f;
                                            // const auto lensRadius = 0.5 * aperture;
                                            // const auto rd = lensRadius * randomInUnitDisk();
                                            // const auto offset = rd.x * R + rd.y * U;
                                            // const auto R = Ray3 { origin( ray ) + offset, direction( ray ) - offset };
                                            color = color + rayColor( ray, scene, self->m_backgroundColor, self->m_depth );
                                        }
                                    }
                                    const Index idx = ( y + dy ) * self->m_width + ( x + dx );
                                    const auto prevColor = self->m_imageAaccessor->get< ColorRGBA >( idx ) * self->m_sampleCount;
                                    self->m_imageAaccessor->set( idx, ( prevColor + rgba( color ) ) / ( self->m_sampleCount + self->m_samples ) );

                                    std::this_thread::yield();
                                }
                            }
                        } );
                }
            }

            CRIMILD_LOG_INFO( "RT Samples: ", m_sampleCount );

            return true;
        }

        void reset( void ) noexcept
        {
            m_nextJobIdx = 0;
            m_jobs.clear();
            m_sampleCount += m_samples;
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

    private:
        Int32 m_width;
        Int32 m_height;
        Int32 m_bpp;
        Real m_aspectRatio;
        Int32 m_samples;
        Int32 m_depth;
        Int32 m_tileSize;
        Int32 m_workerCount;
        ColorRGB m_backgroundColor;
        Int32 m_sampleCount = 0;

        SharedPointer< Image > m_image;
        SharedPointer< BufferAccessor > m_imageAaccessor;

        std::vector< Job > m_jobs;
        Index m_nextJobIdx = 0;
        std::mutex m_mutex;
    };
}

SharedPointer< FrameGraphOperation > crimild::framegraph::softRT( void ) noexcept
{
    return crimild::alloc< SoftRT >();

    // auto rt = crimild::alloc< ScenePass >();
    // rt->setName( "softRT" );

    // auto settings = Simulation::getInstance()->getSettings();
    // const Int32 width = settings->get< Int32 >( "video.width", 320 );
    // const Int32 height = settings->get< Int32 >( "video.height", 240 );
    // const Int32 bpp = 4;
    // const Real aspectRatio = Real( width ) / Real( height );
    // const Int32 samples = settings->get< Int32 >( "rt.samples", 1 );
    // const Int32 depth = settings->get< Int32 >( "rt.depth", 10 );
    // const Int32 tileSize = settings->get< Int32 >( "rt.tile_size", 64 );
    // const Int32 workerCount = std::max( 1, settings->get< Int32 >( "rt.workers", std::thread::hardware_concurrency() ) );

    // const auto backgroundColor = ColorRGB {
    //     settings->get< Real >( "rt.background_color.r", 0.5f ),
    //     settings->get< Real >( "rt.background_color.g", 0.7f ),
    //     settings->get< Real >( "rt.background_color.b", 1.0f ),
    // };

    // auto image = crimild::alloc< Image >();
    // image->extent = Extent3D {
    //     .width = Real32( width ),
    //     .height = Real32( height ),
    // };

    // image->format = crimild::utils::getFormat< ColorRGBA >();

    // image->setBufferView(
    //     [ & ] {
    //         auto buffer = crimild::alloc< Buffer >( Array< ColorRGBA >( width * height ) );
    //         auto bufferView = crimild::alloc< BufferView >( BufferView::Target::IMAGE, buffer, 0, sizeof( ColorRGBA ) );
    //         bufferView->setUsage( BufferView::Usage::DYNAMIC );
    //         return bufferView;
    //     }() );

    // auto colors = crimild::alloc< BufferAccessor >( image->getBufferView(), 0, sizeof( ColorRGBA ) );
    // for ( auto i = 0l; i < width * height; ++i ) {
    //     colors->set( i, ColorRGBA { 0, 0, 0, 1 } );
    // }

    // rt->apply = [ width, height, image, samples, tileSize, depth, workerCount, backgroundColor, colors, sampleCount = 0 ]( auto, auto ) mutable {
    // auto scene = Simulation::getInstance()->getScene();
    // if ( !scene ) {
    //     return true;
    // }

    // auto camera = Camera::getMainCamera();
    // if ( !camera ) {
    //     return true;
    // }

    // scene->perform( UpdateWorldState() );

    // Array< ColorRGBA > colors( width * height );

    // auto reportProgress = [ mutex = std::mutex(),
    //                         progress = 1l,
    //                         max = size_t( width * height ),
    //                         workerCount ]() mutable {
    //     std::lock_guard< std::mutex > lock( mutex );
    //     std::cout << "\rProcessing "
    //               << progress
    //               << "/"
    //               << max
    //               << " ("
    //               << std::setprecision( 3 )
    //               << std::setw( 5 )
    //               << std::fixed
    //               << 100.0f * float( progress ) / float( max )
    //               << "%) - "
    //               << workerCount << " workers"
    //               << std::flush;
    //     progress++;
    // };

    // using Job = std::function< void( void ) >;
    // std::vector< Job > jobs;

    // for ( auto y = 0; y < height; y += tileSize ) {
    //     for ( auto x = 0; x < width; x += tileSize ) {
    //         jobs.push_back(
    //             [ &, x, y ] {
    //                 for ( auto dy = 0; dy < tileSize; ++dy ) {
    //                     if ( y + dy >= height )
    //                         break;
    //                     for ( auto dx = 0; dx < tileSize; ++dx ) {
    //                         if ( x + dx >= width )
    //                             break;
    //                         // reportProgress();

    //                         auto color = ColorRGB::Constants::BLACK;
    //                         for ( auto s = 0l; s < samples; ++s ) {
    //                             Ray3 ray;
    //                             const auto u = ( ( x + dx ) + Random::generate< Real >() ) / Real( width - 1 );
    //                             const auto v = ( ( y + dy ) + Random::generate< Real >() ) / Real( height - 1 );
    //                             if ( camera->getPickRay( u, v, ray ) ) {
    //                                 // TODO: defocus blur
    //                                 // const auto aperture = 2.0f;
    //                                 // const auto lensRadius = 0.5 * aperture;
    //                                 // const auto rd = lensRadius * randomInUnitDisk();
    //                                 // const auto offset = rd.x * R + rd.y * U;
    //                                 // const auto R = Ray3 { origin( ray ) + offset, direction( ray ) - offset };
    //                                 color = color + rayColor( ray, scene, backgroundColor, depth );
    //                             }
    //                         }
    //                         const Index idx = ( y + dy ) * width + ( x + dx );
    //                         const auto prevColor = colors->get< ColorRGBA >( idx ) * sampleCount;
    //                         colors->set( idx, ( prevColor + rgba( color ) ) / ( sampleCount + samples ) );

    //                         std::this_thread::yield();
    //                     }
    //                 }
    //             } );
    //     }
    // }

    // auto nextJob = [ mutex = std::mutex(),
    //                  i = 0l,
    //                  &jobs ]( auto &job ) mutable -> bool {
    //     std::lock_guard< std::mutex > lock( mutex );
    //     if ( i >= jobs.size() ) {
    //         return false;
    //     }
    //     job = jobs[ i++ ];
    //     return true;
    // };

    // // launch all threads
    // std::vector< std::thread > workers;
    // for ( auto i = 0; i < workerCount; ++i ) {
    //     workers.push_back(
    //         std::thread(
    //             [ & ] {
    //                 Job job;
    //                 while ( nextJob( job ) ) {
    //                     job();
    //                 }
    //             } ) );
    // }

    // for ( auto &worker : workers ) {
    //     // wait for all threads to finish
    //     worker.join();
    // }

    // // const auto prevSamples = sampleCount;
    // sampleCount += samples;

    // CRIMILD_LOG_INFO( "RT Samples: ", sampleCount );

    // return true;
    // };

    // rt->writes( { image } );
    // rt->produces( { image } );

    // return rt;
}
