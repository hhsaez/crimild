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
#include "Rendering/Image.hpp"
#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"
#include "Rendering/ScenePass.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/IntersectWorld.hpp"
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild;

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
        } else {
            // Lambertian model
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

SharedPointer< FrameGraphOperation > crimild::framegraph::softRT( void ) noexcept
{
    auto rt = crimild::alloc< ScenePass >();
    rt->setName( "softRT" );

    auto settings = Simulation::getInstance()->getSettings();
    const Int32 width = settings->get< Int32 >( "rt.width", 400 );
    const Int32 height = settings->get< Int32 >( "rt.height", 300 );
    const Int32 bpp = 4;
    const Real aspectRatio = Real( width ) / Real( height );
    const Int32 samples = settings->get< Int32 >( "rt.samples", 5 );

    auto image = crimild::alloc< Image >();
    image->extent = Extent3D {
        .width = Real32( width ),
        .height = Real32( height ),
    };
    // TODO: image format should be float, so we can use HDR colors
    image->format = Format::R8G8B8A8_UNORM;
    image->data = ByteArray( width * height * bpp );

    rt->apply = [ width, height, image, samples, done = false ]( auto, auto ) mutable {
        auto scene = Simulation::getInstance()->getScene();
        if ( !scene ) {
            return true;
        }

        auto camera = Camera::getMainCamera();
        if ( !camera ) {
            return true;
        }

        if ( done ) {
            return true;
        }

        scene->perform( UpdateWorldState() );

        std::cout << "Generating RT image...";

        Array< ColorRGBA > colors( width * height );
        const auto SKY_COLOR = ColorRGB { 0.5, 0.7, 1 };
        const auto HORIZONT_COLOR = ColorRGB { 1, 1, 1 };

        auto progress = []( auto pc ) {
            std::cout << "\33[2K\rGenerating RT Image... " << Int32( 100 * pc ) << "%";
        };

        for ( auto y = 0l; y < height; ++y ) {
            for ( auto x = 0l; x < width; ++x ) {
                const auto backgroundColor = lerp( SKY_COLOR, HORIZONT_COLOR, Real( y ) / Real( height ) );
                auto color = ColorRGB::Constants::BLACK;
                for ( auto s = 0l; s < samples; ++s ) {
                    Ray3 ray;
                    const auto u = ( x + Random::generate< Real >() ) / Real( width - 1 );
                    const auto v = ( y + Random::generate< Real >() ) / Real( height - 1 );
                    if ( camera->getPickRay( u, v, ray ) ) {
                        color = color + rayColor( ray, scene, backgroundColor, 50 );
                    }
                }
                colors[ y * width + x ] = rgba( color / samples );
                progress( Real( y * width + x ) / Real( width * height ) );
            }
        }

        std::cout << "\n";

        colors.each(
            [ image, i = 0 ]( const auto &c ) mutable {
                for ( auto j = 0l; j < 4; j++ ) {
                    image->data[ i * bpp + j ] = UInt8( 255.0f * c[ j ] );
                }
                i++;
            } );

        done = true;
        return true;
    };

    rt->writes( { image } );
    rt->produces( { image } );

    return rt;
}