#ifndef CRIMILD_RAYTRACING_RENDERING_RENDERER_
#define CRIMILD_RAYTRACING_RENDERING_RENDERER_

#include <Crimild.hpp>

namespace crimild {

	namespace raytracing {

		class RTRenderer : public SharedObject {
        private:
            using Mutex = std::mutex;
            using Lock = std::lock_guard< Mutex >;
            
		public:
			RTRenderer( int width, int height, int samples );
			virtual ~RTRenderer( void );
			
			SharedPointer< Image > render( SharedPointer< Node > const &scene, SharedPointer< Camera > camera ) const;
			
		private:
			RGBColorf computeColor( SharedPointer< Node > const &scene, const Ray3f &r, int depth = 0 ) const;
			
			float getRandom() const;
			
			Vector3f randomInUnitSphere( void ) const;
			
			Vector3f reflect( const Vector3f &v, const Vector3f &n ) const;
			
			bool refract( const Vector3f &v, const Vector3f &n, float refIndex, Vector3f &refracted ) const;
			
			// glass reflectivity
			float schlick( float cosine, float refIndex ) const;
			
		private:
			int _width;
			int _height;
			int _samples;
            
            Mutex _mutex;
		};
		
	}

}

#endif

