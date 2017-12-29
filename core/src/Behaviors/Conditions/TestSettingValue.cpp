#include "TestSettingValue.hpp"

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

