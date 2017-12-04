#include "Decorator.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::decorators;

Decorator::Decorator( void )
{

}

Decorator::~Decorator( void )
{

}

void Decorator::init( BehaviorContext *context )
{
	Behavior::init( context );

	if ( getBehavior() != nullptr ) {
		getBehavior()->init( context );
	}
}
		
Behavior::State Decorator::step( BehaviorContext *context )
{
	if ( getBehavior() == nullptr ) {
		return Behavior::State::FAILURE;
	}

	return getBehavior()->step( context );
}

