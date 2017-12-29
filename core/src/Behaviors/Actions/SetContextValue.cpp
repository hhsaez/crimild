#include "SetContextValue.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

SetContextValue::SetContextValue( std::string key, std::string value )
	: _key( key ),
	  _value( value )
{

}

SetContextValue::~SetContextValue( void )
{

}

Behavior::State SetContextValue::step( BehaviorContext *context )
{
	context->setValue( _key, _value );
	return Behavior::State::SUCCESS;
}

