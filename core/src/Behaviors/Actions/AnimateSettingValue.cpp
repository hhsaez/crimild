#include "AnimateSettingValue.hpp"

#include "Mathematics/Interpolation.hpp"
#include "Simulation/Simulation.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

AnimateSettingValue::AnimateSettingValue( std::string key, crimild::Real32 value, crimild::Real32 duration, crimild::Bool ignoreGlobalTimeScale )
	: _key( key ),
	  _value( value ),
	  _duration( duration ),
	  _ignoreGlobalTimeScale( ignoreGlobalTimeScale )
{

}

AnimateSettingValue::~AnimateSettingValue( void )
{

}

void AnimateSettingValue::init( BehaviorContext *context )
{
	_clock.reset();
	_clock.setTimeScale( 1.0f / _duration );
	_clock.setIgnoreGlobalTimeScale( _ignoreGlobalTimeScale );

	_start = Simulation::getInstance()->getSettings()->get( _key, 0.0f );
}

Behavior::State AnimateSettingValue::step( BehaviorContext *context )
{
	_clock.tick();
	if ( _clock.getAccumTime() >= 1.0f ) {
		//context->setValue( _key, _value );
		return Behavior::State::SUCCESS;
	}

	float x = 0.0f;
	Interpolation::linear( _start, _value, _clock.getAccumTime(), x );

	Simulation::getInstance()->getSettings()->set( _key, x );
	
	return Behavior::State::RUNNING;
}

