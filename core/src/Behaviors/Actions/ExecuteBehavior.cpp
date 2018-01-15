#include "ExecuteBehavior.hpp"

#include "SceneGraph/Node.hpp"
#include "Components/BehaviorController.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

ExecuteBehavior::ExecuteBehavior( std::string behaviorName )
	: _behaviorName( behaviorName )
{

}

ExecuteBehavior::~ExecuteBehavior( void )
{

}

void ExecuteBehavior::init( BehaviorContext *context )
{
	auto agent = context->getAgent();
	auto behaviors = agent->getComponent< BehaviorController >();
	if ( behaviors == nullptr ) {
		Log::error( CRIMILD_CURRENT_CLASS_NAME, "Behavior target does not have behaviors" );
		return;
	}

	_behavior = crimild::retain( behaviors->getBehavior( _behaviorName ) );
	if ( _behavior == nullptr ) {
		Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot find behavior named ", _behaviorName );
		return;
	}

	_behavior->init( context );
}

Behavior::State ExecuteBehavior::step( BehaviorContext *context )
{
	if ( _behavior == nullptr ) {
		return Behavior::State::FAILURE;
	}
	
	return _behavior->step( context );
}

