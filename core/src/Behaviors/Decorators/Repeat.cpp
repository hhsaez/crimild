#include "Repeat.hpp"
#include "Coding/Encoder.hpp"
#include "Coding/Decoder.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::decorators;

Repeat::Repeat( void )
{
	
}

Repeat::Repeat( crimild::Int16 times )
	: _times( times )
{

}

Repeat::~Repeat( void )
{

}

void Repeat::init( BehaviorContext *context )
{
	Decorator::init( context );

	_loopCount = 0;
}

Behavior::State Repeat::step( BehaviorContext *context )
{
	auto result = Decorator::step( context );
	if ( result == Behavior::State::RUNNING ) {
		return Behavior::State::RUNNING;
	}
	else if ( result == Behavior::State::SUCCESS ) {
		if ( _times < 0 || ++_loopCount < _times ) {
			getBehavior()->init( context );
			return Behavior::State::RUNNING;
		}
		else {
			return Behavior::State::SUCCESS;
		}
	}

	return Behavior::State::FAILURE;
}

void Repeat::encode( coding::Encoder &encoder )
{
	Decorator::encode( encoder );

	encoder.encode( "times", _times );
}

void Repeat::decode( coding::Decoder &decoder )
{
	Decorator::decode( decoder );

	decoder.decode( "times", _times );
}

