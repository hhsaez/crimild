#include "PrintMessage.hpp"

#include <crimild/coding/Decoder.hpp>
#include <crimild/coding/Encoder.hpp>

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

PrintMessage::PrintMessage( void )
{
}

PrintMessage::PrintMessage( std::string message )
   : _message( message )
{
}

PrintMessage::~PrintMessage( void )
{
}

Behavior::State PrintMessage::step( BehaviorContext *context )
{
   Log::debug( "Behavior", _message );

   return Behavior::State::SUCCESS;
}

void PrintMessage::encode( coding::Encoder &encoder )
{
   Behavior::encode( encoder );

   encoder.encode( "message", _message );
}

void PrintMessage::decode( coding::Decoder &decoder )
{
   Behavior::decode( decoder );

   decoder.decode( "message", _message );
}
