#ifndef CRIMILD_CORE_DEBUG_RENDER_COMPONENT_
#define CRIMILD_CORE_DEBUG_RENDER_COMPONENT_

#include "Components/NodeComponent.hpp"
#include "Crimild_Mathematics.hpp"

namespace crimild {

    class DebugRenderComponent : public NodeComponent {
        CRIMILD_IMPLEMENT_RTTI( crimild::DebugRenderComponent )

    public:
        explicit DebugRenderComponent( const ColorRGBA &color );
        virtual ~DebugRenderComponent( void ) = default;

        virtual void renderDebugInfo( crimild::Renderer *, crimild::Camera * ) override;

    private:
        ColorRGBA _color;
    };

}

#endif
