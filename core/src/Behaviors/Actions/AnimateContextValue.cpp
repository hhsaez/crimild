#include "AnimateContextValue.hpp"

#include "Mathematics/Interpolation.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

AnimateContextValue::AnimateContextValue( std::string key, crimild::Real32 value, crimild::Real32 duration )
	: _key( key ),
	  _value( value ),
	  _duration( duration )
{

}

AnimateContextValue::~AnimateContextValue( void )
{

}

void AnimateContextValue::init( BehaviorContext *context )
{
	_clock.reset();
	_start = context->getValue< float >( _key );
}

Behavior::State AnimateContextValue::step( BehaviorContext *context )
{
	_clock += context->getClock().getDeltaTime() / _duration;
	if ( _clock.getAccumTime() >= 1.0f ) {
		context->setValue( _key, _value );
		return Behavior::State::SUCCESS;
	}

	float x = 0.0f;
	Interpolation::linear( _start, _value, _clock.getAccumTime(), x );

	context->setValue( _key, x );
	
	return Behavior::State::RUNNING;
}

