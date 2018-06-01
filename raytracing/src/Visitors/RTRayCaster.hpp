#ifndef CRIMILD_RAYTRACING_VISITORS_RAY_CASTER_
#define CRIMILD_RAYTRACING_VISITORS_RAY_CASTER_

#include "Mathematics/Vector.hpp"
#include "Mathematics/Ray.hpp"
#include "Visitors/NodeVisitor.hpp"

#include <list>

namespace crimild {
    
	namespace raytracing {

		class RTRayCaster : public NodeVisitor {
		public:
			struct Result {
				float t;
				Vector3f position;
				Vector3f normal;
				Node *node;
			};
			
			RTRayCaster( const Ray3f &ray, float tMin = Numericf::ZERO_TOLERANCE, float tMax = std::numeric_limits< float >::max() );
			virtual ~RTRayCaster( void );
				
			const Ray3f &getRay( void ) const { return _ray; }
			
			virtual void visitGroup( Group *group );
			virtual void visitGeometry( Geometry *geometry );

			bool hasMatches( void ) const;

			const Result &getBestMatch( void );

		private:
			Ray3f _ray;
			std::list< Result > _candidates;
			float _tMin;
			float _tMax;
		};

	}

}

#endif

