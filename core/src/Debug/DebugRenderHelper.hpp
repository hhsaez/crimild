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

		static void renderLine( RendererPtr const &renderer, CameraPtr const &camera, const Vector3f &from, const Vector3f &to, const RGBAColorf &color = RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ) );
		static void renderLines( RendererPtr const &renderer, CameraPtr const &camera, const Vector3f *lines, unsigned int count, const RGBAColorf &color = RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ) );

		static void render( RendererPtr const &renderer, CameraPtr const &camera, PrimitivePtr const &primitive, const TransformationImpl &model, const RGBAColorf &color );
		
		static void renderBox( RendererPtr const &renderer, CameraPtr const &camera, const Vector3f &position, float scale, const RGBAColorf &color = RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ) );
		static void renderSphere( RendererPtr const &renderer, CameraPtr const &camera, const Vector3f &position, float scale, const RGBAColorf &color = RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ) );

	private:
		static PrimitivePtr _boxPrimitive;
		static PrimitivePtr _spherePrimitive;

		static VertexBufferObjectPtr _linesVBO;

		static DepthStatePtr _depthState;
		static AlphaStatePtr _alphaState;
	};

}

#endif

