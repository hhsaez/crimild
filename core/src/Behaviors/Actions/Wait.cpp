#include "Wait.hpp"
#include "Coding/Encoder.hpp"
#include "Coding/Decoder.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

Wait::Wait( crimild::Real32 duration )
	: _duration( duration )
{

}

Wait::~Wait( void )
{

}

void Wait::init( BehaviorContext *context )
{
	Behavior::init( context );

	_clock.reset();
}
		
Behavior::State Wait::step( BehaviorContext *context )
{
	_clock += context->getClock();
	if ( _duration >= 0 &&  _clock.getAccumTime() >= _duration ) {
		return Behavior::State::SUCCESS;
	}
	
	return Behavior::State::RUNNING;
}

void Wait::encode( coding::Encoder &encoder )
{
	Behavior::encode( encoder );

	encoder.encode( "duration", _duration );
}

void Wait::decode( coding::Decoder &decoder )
{
	Behavior::decode( decoder );

	decoder.decode( "duration", _duration );
}

