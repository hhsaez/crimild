#include "DebugRenderComponent.hpp"
#include "DebugRenderHelper.hpp"

#include "SceneGraph/Node.hpp"

using namespace crimild;

DebugRenderComponent::DebugRenderComponent( const RGBAColorf &color )
	: _color( color )
{

}

DebugRenderComponent::~DebugRenderComponent( void )
{

}

void DebugRenderComponent::renderDebugInfo( Renderer *renderer, Camera *camera )
{
	auto node = getNode();
	auto center = node->getWorldBound()->getCenter();
	auto radius = node->getWorldBound()->getRadius();

	DebugRenderHelper::renderSphere( renderer, camera, center, radius, _color );
}

