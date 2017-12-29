#include "TestInputAxis.hpp"

#include "Simulation/Input.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::conditions;

TestInputAxis::TestInputAxis( std::string axis, crimild::Real32 value )
	: _axis( axis ),
	  _value( value )
{

}

TestInputAxis::~TestInputAxis( void )
{

}

Behavior::State TestInputAxis::step( BehaviorContext *context )
{
	float currentAxisValue = Input::getInstance()->getAxis( _axis );
	if ( currentAxisValue != _value ) {
		return Behavior::State::FAILURE;
	}

	return Behavior::State::SUCCESS;
}

