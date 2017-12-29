#include "Inverter.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::decorators;

Inverter::Inverter( void )
{

}

Inverter::~Inverter( void )
{

}

Behavior::State Inverter::step( BehaviorContext *context )
{
	auto result = Decorator::step( context );
	if ( result == Behavior::State::SUCCESS ) {
		return Behavior::State::FAILURE;
	}
	else if ( result == Behavior::State::FAILURE ) {
		return Behavior::State::SUCCESS;
	}

	return result;
}

