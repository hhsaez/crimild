#include "TestInputAxis.hpp"
#include "Coding/Encoder.hpp"
#include "Coding/Decoder.hpp"

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

void TestInputAxis::encode( coding::Encoder &encoder )
{
	Behavior::encode( encoder );

	encoder.encode( "axis", _axis );
	encoder.encode( "value", _value );
}

void TestInputAxis::decode( coding::Decoder &decoder )
{
	Behavior::decode( decoder );

	decoder.decode( "axis", _axis );
	decoder.decode( "value", _value );
}

