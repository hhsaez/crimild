#include "Ball.hpp"

#include "messaging/GameOver.hpp"

#include <Crimild_Physics.hpp>

using namespace crimild;
using namespace crimild::scripting;
using namespace crimild::physics;
using namespace crimild::bricks;

CRIMILD_SCRIPTING_REGISTER_COMPONENT_BUILDER( crimild::bricks::Ball )

Ball::Ball( ScriptEvaluator &eval )
{

}

Ball::~Ball( void )
{

}

void Ball::start( void )
{

}

void Ball::update( const Clock &c )
{
	if ( getNode()->getLocal().getTranslate()[ 1 ] < -10.0f ) {
		broadcastMessage( GameOver {} );
	}
}

