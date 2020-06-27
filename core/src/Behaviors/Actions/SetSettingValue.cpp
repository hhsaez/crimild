#include "SetSettingValue.hpp"

#include "Simulation/Simulation.hpp"
#include "Coding/Encoder.hpp"
#include "Coding/Decoder.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

SetSettingValue::SetSettingValue( void )
{

}

SetSettingValue::SetSettingValue( std::string key, std::string value )
	: _key( key ),
	  _value( value )
{

}

SetSettingValue::~SetSettingValue( void )
{

}

Behavior::State SetSettingValue::step( BehaviorContext *context )
{
	Simulation::getInstance()->getSettings()->set( _key, _value );
	return Behavior::State::SUCCESS;
}

void SetSettingValue::encode( coding::Encoder &encoder )
{
	Behavior::encode( encoder );

	encoder.encode( "key", _key );
	encoder.encode( "value", _value );
}

void SetSettingValue::decode( coding::Decoder &decoder )
{
	Behavior::decode( decoder );

	decoder.decode( "key", _key );
	decoder.decode( "value", _value );
}
