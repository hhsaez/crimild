#include "RepeatUntilFail.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::decorators;

RepeatUntilFail::RepeatUntilFail( void )
{

}

RepeatUntilFail::~RepeatUntilFail( void )
{

}

Behavior::State RepeatUntilFail::step( BehaviorContext *context )
{
	auto result = Decorator::step( context );
	if ( result == Behavior::State::FAILURE ) {
		return Behavior::State::SUCCESS;
	}
	else if ( result == Behavior::State::SUCCESS ) {
		getBehavior()->init( context );
	}

	return Behavior::State::RUNNING;
}

