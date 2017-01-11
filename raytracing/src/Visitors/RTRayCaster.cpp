#include "RTRayCaster.hpp"

using namespace crimild;
using namespace crimild::raytracing;

RTRayCaster::RTRayCaster( const Ray3f &ray, float tMin, float tMax )
	: _ray( ray ),
	  _tMin( tMin ),
	  _tMax( tMax )
{
			
}

RTRayCaster::~RTRayCaster( void )
{
	
}

void RTRayCaster::visitGroup( Group *group )
{
	if ( true || group->getWorldBound()->testIntersection( getRay() ) ) {
		NodeVisitor::visitGroup( group );
	}
}

void RTRayCaster::visitGeometry( Geometry *geometry )
{
	Sphere3f s( geometry->getWorldBound()->getCenter(), geometry->getWorldBound()->getRadius() );
	float t = Intersection::find( s, getRay() );
	if ( t > _tMin && t < _tMax ) {
		auto p = getRay().getPointAt( t );
		_candidates.push_back( Result {
			t,
			p,
			( p - s.getCenter() ).getNormalized(),
			geometry
			});
	}
}

bool RTRayCaster::hasMatches( void ) const
{
	return _candidates.size() > 0;
}

const RTRayCaster::Result &RTRayCaster::getBestMatch( void )
{
	_candidates.sort( []( const Result &r1, const Result &r2 ) -> bool {
		return r1.t < r2.t;
	});
	return _candidates.front();
}

