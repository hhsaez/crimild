#include "Brick.hpp"

#include "messaging/BrickDestroyed.hpp"

#include <Crimild_Physics.hpp>

using namespace crimild;
using namespace crimild::scripting;
using namespace crimild::physics;
using namespace crimild::bricks;

CRIMILD_SCRIPTING_REGISTER_COMPONENT_BUILDER( crimild::bricks::Brick )

Brick::Brick( ScriptEvaluator &eval )
{

}

Brick::~Brick( void )
{

}

void Brick::start( void )
{
	auto rigidBody = getNode()->getComponent< RigidBodyComponent >();
	assert( rigidBody != nullptr );

	auto self = this;
	rigidBody->setCollisionCallback( [self]( RigidBodyComponent *other ) {
		MessageQueue::getInstance()->pushMessage( BrickDestroyed { crimild::retain( self->getNode() ) } );
		self->getNode()->detachFromParent();
	});
}

void Brick::update( const Clock &c )
{
	// getNode()->local().rotate().fromAxisAngle( Vector3f( 0.0f, 0.0f, 1.0f ), 0.25f * c.getCurrentTime() * Numericf::PI );
}

