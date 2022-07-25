#ifndef CRIMILD_DEBUG_RENDER_HELPER_
#define CRIMILD_DEBUG_RENDER_HELPER_

#include "Mathematics/ColorRGBA.hpp"
#include "Mathematics/Transformation.hpp"
#include "Mathematics/Vector3.hpp"

#include <string>

namespace crimild {

    class Renderer;
    class Camera;
    class Primitive;
    class Geometry;

    class [[deprecated]] DebugRenderHelper {
    public:
        static void init( void );

        static void renderLine( Renderer *renderer, Camera *camera, const Vector3f &from, const Vector3f &to, const ColorRGBA &color = ColorRGBA::Constants::WHITE );

        static void renderLines( Renderer *renderer, Camera *camera, const Vector3f *lines, unsigned int count, const ColorRGBA &color = ColorRGBA::Constants::WHITE );

        /**
            \brief Render lines on screen space
        */
        static void renderLines( const Vector3f *lines, unsigned int count, const ColorRGBA &color = ColorRGBA { 1.0f, 1.0f, 1.0f, 1.0f } );

        static void render( Renderer *renderer, Camera *camera, Primitive *primitive, const Transformation &model, const ColorRGBA &color );

        static void render( const Transformation &transform, crimild::Real32 axisSize = 1.0f );

        /**
            \brief Renders a geometry on screen space
        */
        static void render( Geometry *geometry );

        static void renderBox( Renderer *renderer, Camera *camera, const Vector3f &position, float scale, const ColorRGBA &color = ColorRGBA::Constants::WHITE );
        static void renderBox( Renderer *renderer, Camera *camera, const Vector3f &position, const Vector3f &size, const ColorRGBA &color = ColorRGBA::Constants::WHITE );
        static void renderSphere( Renderer *renderer, Camera *camera, const Vector3f &position, float scale, const ColorRGBA &color = ColorRGBA::Constants::WHITE );
        static void renderWireframeSphere( Renderer *renderer, Camera *camera, const Vector3f &position, float scale, const ColorRGBA &color = ColorRGBA::Constants::WHITE );

        static void renderText( std::string str, const Vector3f &position, const ColorRGBA &color = ColorRGBA::Constants::WHITE );
    };

}

#endif
