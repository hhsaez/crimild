#include "RTRenderer.hpp"
#include "RTMaterial.hpp"

#include "Visitors/RTRayCaster.hpp"
#include "Concurrency/Async.hpp"
#include "SceneGraph/Camera.hpp"
#include "Mathematics/Random.hpp"
#include "Mathematics/Interpolation.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <thread>
#include <chrono>
#include <ctime>
#include <fstream>

using namespace crimild;
using namespace crimild::raytracing;

RTRenderer::RTRenderer( int width, int height, int samples )
	: _width( width ),
	  _height( height ),
	  _samples( samples )
{
	
}

RTRenderer::~RTRenderer( void )
{
	
}

SharedPointer< Image > RTRenderer::render( SharedPointer< Node > const &scene, SharedPointer< Camera > camera ) const
{
	int bpp = 3;
	std::vector< unsigned char > pixels( _width * _height * bpp );
	
    // this might be overkill, but we're going to create a job
    // for each pixel in the image
    size_t dx = 1;
    size_t dy = 1;
	
	auto parentJob = crimild::concurrency::async();
    
    std::atomic< long > jobCount( 0 );
    const int JOB_TOTAL = _height * _width;
    
	for ( size_t y = 0; y < _height; y += dy ) {
		for ( size_t x = 0; x < _width; x += dx ) {
			crimild::concurrency::async( parentJob, [this, &jobCount, JOB_TOTAL, camera, x, y, dx, dy, bpp, scene, &pixels ]( void ) {
				for ( size_t t = y; t < y + dy; t++ ) {
					for ( size_t s = x; s < x + dx; s++ ) {
						RGBColorf c = RGBColorf::ZERO;
						Ray3f ray;
						if ( _samples > 1 ) {
							for ( int sample = 0; sample < _samples; sample++ ) {
								float u = ( float ) ( s + getRandom() ) / ( float ) _width;
								float v = ( float ) ( t + getRandom() ) / ( float ) _height;
								
								camera->getPickRay( u, v, ray );
								c += computeColor( scene, ray );							
							}
							c /= ( float ) _samples;
						}
						else {
							float u = ( float ) s / ( float ) _width;
							float v = ( float ) t / ( float ) _height;
							camera->getPickRay( u, v, ray );
							c = computeColor( scene, ray );
						}
						
						// gamma correction
						c = RGBColorf( Numericf::sqrt( c[ 0 ] ), Numericf::sqrt( c[ 1 ] ), Numericf::sqrt( c[ 2 ] ) );
						
						for ( int i = 0; i < bpp; i++ ) {
							pixels[ ( t * _width + s ) * bpp + i ] = ( unsigned char )( 255.99f * c[ i ] );
						}
                        
                        jobCount++;
                        Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Progress: ", jobCount, "/", JOB_TOTAL );
					}
				}
			});
		}
	}
	
	crimild::concurrency::wait( parentJob );
    Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Done rendering frames" );
    
    auto result = crimild::alloc< Image >( _width, _height, bpp, &pixels[ 0 ], Image::PixelFormat::RGB );
    return result;
}

RGBColorf RTRenderer::computeColor( SharedPointer< Node > const &scene, const Ray3f &r, int depth ) const
{
	RTRayCaster caster( r );
	scene->perform( caster );
	if ( caster.hasMatches() ) {
		auto &hit = caster.getBestMatch();
		auto material = hit.node->getComponent< RTMaterial >();
		Ray3f scattered;
		RGBColorf attenuation = material->getAlbedo();
		RGBColorf color = RGBColorf::ZERO;
		bool visible = true;
		
		switch ( material->getType() ) {
		case RTMaterial::Type::METALLIC: {
			auto reflected = reflect( r.getDirection(), hit.normal );
			reflected += material->getFuzz() * randomInUnitSphere();
			reflected.normalize();
			scattered = Ray3f( hit.position, reflected );
			visible = ( scattered.getDirection() * hit.normal > 0 );
			break;
		}
		case RTMaterial::Type::DIELECTRIC: {
			Vector3f outwardNormal;
			Vector3f reflected = reflect( r.getDirection(), hit.normal );
			float refIndex;
			Vector3f refracted;
			float reflectProb;
			float cosine;
			
			if ( r.getDirection() * hit.normal > 0 ) {
				outwardNormal = -hit.normal;
				refIndex = material->getRefractionIndex();
				// why not assume direction is unit-length?
				cosine = ( r.getDirection() * hit.normal ) / ( r.getDirection().getMagnitude() );
				cosine = Numericf::sqrt( 1.0f - refIndex * refIndex * ( 1.0f - cosine * cosine ) );
			}
			else {
				outwardNormal = hit.normal;
				refIndex = 1.0f / material->getRefractionIndex();
				cosine = -( r.getDirection() * hit.normal ) / ( r.getDirection().getMagnitude() );
			}
			
			if ( refract( r.getDirection(), outwardNormal, refIndex, refracted ) ) {
				reflectProb = schlick( cosine, refIndex );
			}
			else {
				reflectProb = 1.0f;
			}
			
			if ( getRandom() < reflectProb ) {
				scattered = Ray3f( hit.position, reflected );
			}
			else {
				scattered = Ray3f( hit.position, refracted );
			}
			break;
		}
		case RTMaterial::Type::LAMBERTIAN: 
		default: {
			Vector3f target = hit.normal + randomInUnitSphere();
			scattered = Ray3f( hit.position, target.getNormalized() );
			break;
		}
		};

		// TODO: max depth as a setting?
		if ( depth < 50 && visible ) {
			auto color = computeColor( scene, scattered, depth + 1 );
			color.times( attenuation );
			return color;
		}
		else {
			// the ray has scattered enough and it's colliding against
			// multiple surfaces. No ambient light is applied
			return RGBColorf::ZERO;
		}
	}
	
	Vector3f unitDirection = r.getDirection().getNormalized();
	float t = 0.5f * ( unitDirection.y() + 1.0f );
	RGBColorf output;
	Interpolation::linear( RGBColorf::ONE, RGBColorf( 0.5f, 0.7f, 1.0f ), t, output );
	return output;
}

float RTRenderer::getRandom() const
{
#if 1
	return Random::generate< float >();
#else
	return drand48();
#endif
}

Vector3f RTRenderer::randomInUnitSphere( void ) const
{
	return Vector3f(
		2.0f * getRandom() - 1.0f,
		2.0f * getRandom() - 1.0f,
		2.0f * getRandom() -1.0f )
	.getNormalized();
}

Vector3f RTRenderer::reflect( const Vector3f &v, const Vector3f &n ) const
{
	return v - 2 * ( v * n ) * n;
}

bool RTRenderer::refract( const Vector3f &v, const Vector3f &n, float refIndex, Vector3f &refracted ) const
{
	Vector3f uv = v.getNormalized();
	Vector3f un = n.getNormalized();
	float dt = uv * un;
	float discriminant = 1.0f - refIndex * refIndex * ( 1 - dt * dt );
	if ( discriminant > 0 ) {
		refracted = refIndex * ( uv - un * dt ) - un * Numericf::sqrt( discriminant );
		return true;
	}
	
	return false;
}

// glass reflectivity, using Schlick aproximation
float RTRenderer::schlick( float cosine, float refIndex ) const
{
	float r0 = ( 1.0f - refIndex ) / ( 1.0f + refIndex );
	r0 = r0 * r0;
	return r0 + ( 1.0f - r0 ) * Numericf::pow( ( 1.0f - cosine ), 5.0f );
}

