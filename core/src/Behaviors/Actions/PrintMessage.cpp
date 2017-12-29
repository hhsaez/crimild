#include "PrintMessage.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

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

