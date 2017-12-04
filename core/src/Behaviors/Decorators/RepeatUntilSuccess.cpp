#include "RepeatUntilSuccess.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::decorators;

RepeatUntilSuccess::RepeatUntilSuccess( void )
{

}

RepeatUntilSuccess::~RepeatUntilSuccess( void )
{

}

Behavior::State RepeatUntilSuccess::step( BehaviorContext *context )
{
	auto result = Decorator::step( context );
	if ( result == Behavior::State::SUCCESS ) {
		return Behavior::State::SUCCESS;
	}
	else if ( result == Behavior::State::FAILURE ) {
		// start over
		getBehavior()->init( context );
	}

	return Behavior::State::RUNNING;
}

