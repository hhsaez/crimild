#include "TestSettingValue.hpp"
#include "Coding/Encoder.hpp"
#include "Coding/Decoder.hpp"

#include "Simulation/Simulation.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::conditions;

TestSettingValue::TestSettingValue( std::string key, std::string value, std::string comparator )
	: _key( key ),
	  _value( value ),
	  _comparator( comparator )
{

}

TestSettingValue::~TestSettingValue( void )
{

}

Behavior::State TestSettingValue::step( BehaviorContext *context )
{
	auto setting = Simulation::getInstance()->getSettings()->get( _key, "" );
	if ( setting != _value ) {
		return Behavior::State::FAILURE;
	}

	return Behavior::State::SUCCESS;
}

void TestSettingValue::encode( coding::Encoder &encoder )
{
	Behavior::encode( encoder );

	encoder.encode( "key", _key );
	encoder.encode( "value", _value );
}

void TestSettingValue::decode( coding::Decoder &decoder )
{
	Behavior::decode( decoder );

	decoder.decode( "key", _key );
	decoder.decode( "value", _value );
}

