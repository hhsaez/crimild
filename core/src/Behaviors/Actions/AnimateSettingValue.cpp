#include "AnimateSettingValue.hpp"

#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"
#include "Crimild_Mathematics.hpp"
#include "Simulation/Simulation.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

AnimateSettingValue::AnimateSettingValue( void )
{
}

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
        // context->setValue( _key, _value );
        return Behavior::State::SUCCESS;
    }

    float x = 0.0f;
    Interpolation::linear( _start, _value, _clock.getAccumTime(), x );

    Simulation::getInstance()->getSettings()->set( _key, x );

    return Behavior::State::RUNNING;
}

void AnimateSettingValue::encode( coding::Encoder &encoder )
{
    Behavior::encode( encoder );

    encoder.encode( "key", _key );
    encoder.encode( "value", _value );
    encoder.encode( "duration", _duration );
    encoder.encode( "ignore_global_time_scale", _ignoreGlobalTimeScale );
}

void AnimateSettingValue::decode( coding::Decoder &decoder )
{
    Behavior::decode( decoder );

    decoder.decode( "key", _key );
    decoder.decode( "value", _value );
    decoder.decode( "duration", _duration );
    decoder.decode( "ignore_global_time_scale", _ignoreGlobalTimeScale );
}
