#ifndef CRIMILD_DEBUG_RENDER_HELPER_
#define CRIMILD_DEBUG_RENDER_HELPER_

#include "Mathematics/Vector.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/AlphaState.hpp"
#include "Rendering/DepthState.hpp"

#include "Primitives/Primitive.hpp"

#include "SceneGraph/Camera.hpp"

namespace crimild {

	class DebugRenderHelper {
	public:
		static void init( void );

		static void renderLine( crimild::Renderer *renderer, Camera *camera, const Vector3f &from, const Vector3f &to, const RGBAColorf &color = RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ) );
		static void renderLines( crimild::Renderer *renderer, Camera *camera, const Vector3f *lines, unsigned int count, const RGBAColorf &color = RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ) );

		static void render( crimild::Renderer *renderer, Camera *camera, Primitive *primitive, const TransformationImpl &model, const RGBAColorf &color );
		
		static void renderBox( crimild::Renderer *renderer, Camera *camera, const Vector3f &position, float scale, const RGBAColorf &color = RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ) );
		static void renderSphere( crimild::Renderer *renderer, Camera *camera, const Vector3f &position, float scale, const RGBAColorf &color = RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ) );

	private:
		static Pointer< Primitive > _boxPrimitive;
		static Pointer< Primitive > _spherePrimitive;

		static Pointer< VertexBufferObject > _linesVBO;

		static Pointer< DepthState > _depthState;
		static Pointer< AlphaState > _alphaState;
	};

}

#endif

