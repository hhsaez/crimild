#ifndef CRIMILD_DEBUG_RENDER_HELPER_
#define CRIMILD_DEBUG_RENDER_HELPER_

#include "Mathematics/Vector.hpp"
#include "Mathematics/Transformation.hpp"

namespace crimild {
    
    class Renderer;
    class Camera;
    class Primitive;
    class Geometry;

	class DebugRenderHelper {
	public:
		static void init( void );

		static void renderLine( Renderer *renderer, Camera *camera, const Vector3f &from, const Vector3f &to, const RGBAColorf &color = RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ) );

		static void renderLines( Renderer *renderer, Camera *camera, const Vector3f *lines, unsigned int count, const RGBAColorf &color = RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ) );

		/**
			\brief Render lines on screen space
		*/
		static void renderLines( const Vector3f *lines, unsigned int count, const RGBAColorf &color = RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ) );

		static void render( Renderer *renderer, Camera *camera, Primitive *primitive, const Transformation &model, const RGBAColorf &color );

		/** 
			\brief Renders a geometry on screen space
		*/
		static void render( Geometry *geometry );
		
		static void renderBox( Renderer *renderer, Camera *camera, const Vector3f &position, float scale, const RGBAColorf &color = RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ) );
		static void renderBox( Renderer *renderer, Camera *camera, const Vector3f &position, const Vector3f &size, const RGBAColorf &color = RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ) );
		static void renderSphere( Renderer *renderer, Camera *camera, const Vector3f &position, float scale, const RGBAColorf &color = RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ) );

		static void renderText( std::string str, const Vector3f &position, const RGBAColorf &color = RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ) );
	};

}

#endif

