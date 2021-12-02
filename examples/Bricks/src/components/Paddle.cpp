#include "Paddle.hpp"

using namespace crimild;
using namespace crimild::scripting;
using namespace crimild::bricks;

CRIMILD_SCRIPTING_REGISTER_COMPONENT_BUILDER( crimild::bricks::Paddle )

Paddle::Paddle( ScriptEvaluator &eval )
{
	eval.getPropValue( "speed", _speed );
}

Paddle::~Paddle( void )
{

}

void Paddle::start( void )
{

}

void Paddle::update( const Clock & )
{
	if ( Input::getInstance()->isKeyDown( 'A' ) ) {
		getNode()->local().translate() -= _speed * Vector3f( 1.0f, 0.0f, 0.0 );
	}

	if ( Input::getInstance()->isKeyDown( 'D' ) ) {
		getNode()->local().translate() += _speed * Vector3f( 1.0f, 0.0f, 0.0 );
	}
}

