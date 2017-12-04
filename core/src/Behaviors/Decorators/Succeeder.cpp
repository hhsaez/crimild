#include "Succeeder.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::decorators;

Succeeder::Succeeder( void )
{

}

Succeeder::~Succeeder( void )
{

}

Behavior::State Succeeder::step( BehaviorContext *context )
{
	auto result = Decorator::step( context );
	if ( result == Behavior::State::RUNNING ) {
		return Behavior::State::RUNNING;
	}

	return Behavior::State::SUCCESS;
}

