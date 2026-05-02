#include "TestInputAxis.hpp"

#include "Simulation/Input.hpp"

#include <crimild/coding/Decoder.hpp>
#include <crimild/coding/Encoder.hpp>

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::conditions;

TestInputAxis::TestInputAxis( void )
{
}

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
