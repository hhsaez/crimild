#include "LookAt.hpp"

#include "SceneGraph/Node.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

LookAt::LookAt( const Vector3f &target, crimild::Real32 duration )
	: _target( target ),
	  _duration( duration )
{

}

LookAt::~LookAt( void )
{

}

void LookAt::init( BehaviorContext *context )
{
	Behavior::init( context );

	_clock.reset();
}
		
Behavior::State LookAt::step( BehaviorContext *context )
{
	context->getAgent()->local().lookAt( _target );
	
	_clock += context->getClock();
	if ( _clock.getAccumTime() >= _duration ) {
		return Behavior::State::SUCCESS;
	}
	
	return Behavior::State::RUNNING;
}

