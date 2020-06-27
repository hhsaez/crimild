#include "FindTarget.hpp"

#include "Simulation/Simulation.hpp"
#include "Visitors/Apply.hpp"
#include "Coding/Encoder.hpp"
#include "Coding/Decoder.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

FindTarget::FindTarget( void )
{

}

FindTarget::FindTarget( std::string targetName )
	: _targetName( targetName )
{

}

FindTarget::~FindTarget( void )
{

}

Behavior::State FindTarget::step( BehaviorContext *context )
{
	bool success = false;

	Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Finding target with name ", _targetName );

	auto scene = Simulation::getInstance()->getScene();
	if ( scene != nullptr ) {
		scene->perform( Apply( [this, context, &success]( Node *node ) {
			if ( node->getName() == _targetName ) {
				context->addTarget( node );
				success = true;
			}
		}));
	}

	if ( !success ) {
		Log::warning( CRIMILD_CURRENT_CLASS_NAME, "Couldn't find target with name ", _targetName );
		return Behavior::State::FAILURE;
	}

	return Behavior::State::SUCCESS;
}

void FindTarget::encode( coding::Encoder &encoder )
{
	Behavior::encode( encoder );

	encoder.encode( "targetName", _targetName );
}

void FindTarget::decode( coding::Decoder &decoder )
{
	Behavior::decode( decoder );

	decoder.decode( "targetName", _targetName );
}
