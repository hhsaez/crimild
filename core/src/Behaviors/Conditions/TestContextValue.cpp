#include "TestContextValue.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::conditions;

TestContextValue::TestContextValue( std::string key, std::string value, std::string comparator )
	: _key( key ),
	  _value( value ),
	  _comparator( comparator )
{

}

TestContextValue::~TestContextValue( void )
{

}

Behavior::State TestContextValue::step( BehaviorContext *context )
{
	auto setting = context->getValue< std::string >( _key );

	if ( setting != _value ) {
		return Behavior::State::FAILURE;
	}

	return Behavior::State::SUCCESS;
}

