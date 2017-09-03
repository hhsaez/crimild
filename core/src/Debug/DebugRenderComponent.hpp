#ifndef CRIMILD_CORE_DEBUG_RENDER_COMPONENT_
#define CRIMILD_CORE_DEBUG_RENDER_COMPONENT_

#include "Components/NodeComponent.hpp"

namespace crimild {

	class DebugRenderComponent : public NodeComponent {
		CRIMILD_IMPLEMENT_RTTI( crimild::DebugRenderComponent )

	public:
		explicit DebugRenderComponent( const RGBAColorf &color );
		virtual ~DebugRenderComponent( void );

		virtual void renderDebugInfo( crimild::Renderer *, crimild::Camera * ) override;

	private:
		RGBAColorf _color;
	};
	
}

#endif
