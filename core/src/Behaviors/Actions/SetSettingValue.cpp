#include "SetSettingValue.hpp"

#include "Simulation/Simulation.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

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

