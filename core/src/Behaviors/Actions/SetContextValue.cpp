#include "SetContextValue.hpp"

#include "Crimild_Coding.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

SetContextValue::SetContextValue( void )
{
}

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

void SetContextValue::encode( coding::Encoder &encoder )
{
    Behavior::encode( encoder );

    encoder.encode( "key", _key );
    encoder.encode( "value", _value );
}

void SetContextValue::decode( coding::Decoder &decoder )
{
    Behavior::decode( decoder );

    decoder.decode( "key", _key );
    decoder.decode( "value", _value );
}
