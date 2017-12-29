#include "Trigger.hpp"

using namespace crimild;
using namespace crimild::messaging;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

std::list< Trigger * > Trigger::_allTriggers;

void Trigger::each( std::function< void( std::string str ) > const &callback )
{
	for ( const auto &t : _allTriggers ) {
		callback( t->getTriggerName() );
	}
}

Trigger::Trigger( std::string triggerName )
	: _triggerName( triggerName )
{
	_allTriggers.push_back( this );
}

Trigger::~Trigger( void )
{
	_allTriggers.remove( this );
}

void Trigger::init( BehaviorContext *context )
{
	Behavior::init( context );
}
		
Behavior::State Trigger::step( BehaviorContext *context )
{
    Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Dispatching behavior event with name ", _triggerName );
	broadcastMessage( BehaviorEvent { _triggerName } );
	return Behavior::State::SUCCESS;
}

