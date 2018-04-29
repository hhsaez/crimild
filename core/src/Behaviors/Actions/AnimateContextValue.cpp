#include "AnimateContextValue.hpp"
#include "Coding/Encoder.hpp"
#include "Coding/Decoder.hpp"

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

void AnimateContextValue::encode( coding::Encoder &encoder )
{
	Behavior::encode( encoder );

	encoder.encode( "key", _key );
	encoder.encode( "value", _value );
	encoder.encode( "duration", _duration );
}

void AnimateContextValue::decode( coding::Decoder &decoder )
{
	Behavior::decode( decoder );

	decoder.decode( "key", _key );
	decoder.decode( "value", _value );
	decoder.decode( "duration", _duration );
}

