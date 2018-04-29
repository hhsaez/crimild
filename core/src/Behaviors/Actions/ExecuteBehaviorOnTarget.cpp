#include "ExecuteBehaviorOnTarget.hpp"

#include "SceneGraph/Node.hpp"
#include "Behaviors/BehaviorController.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

ExecuteBehaviorOnTarget::ExecuteBehaviorOnTarget( std::string behaviorName, crimild::Bool overrideTarget )
	: _behaviorName( behaviorName ),
	  _overrideTarget( overrideTarget )
{

}

ExecuteBehaviorOnTarget::~ExecuteBehaviorOnTarget( void )
{

}

Behavior::State ExecuteBehaviorOnTarget::step( BehaviorContext *context )
{
	if ( !context->hasTargets() ) {
		return Behavior::State::FAILURE;
	}

	auto target = context->getTargetAt( 0 );
	if ( target == nullptr ) {
		Log::error( CRIMILD_CURRENT_CLASS_NAME, "Behavior requires at least one target" );
		return Behavior::State::FAILURE;
	}

	auto behaviors = target->getComponent< BehaviorController >();
	if ( behaviors == nullptr ) {
		Log::error( CRIMILD_CURRENT_CLASS_NAME, "Behavior target does not have behaviors" );
		return Behavior::State::FAILURE;
	}

	if ( _overrideTarget ) {
		behaviors->getContext()->removeAllTargets();
		behaviors->getContext()->addTarget( context->getAgent() );
	}

	if ( !behaviors->executeBehaviorTree( _behaviorName ) ) {
		Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot execute behavior on target with name ", _behaviorName );
		return Behavior::State::FAILURE;
	}

	return Behavior::State::SUCCESS;
}

void ExecuteBehaviorOnTarget::encode( coding::Encoder &encoder )
{
	Behavior::encode( encoder );

	encoder.encode( "behavior_name", _behaviorName );
	encoder.encode( "override_target", _overrideTarget );
}

void ExecuteBehaviorOnTarget::decode( coding::Decoder &decoder )
{
	Behavior::decode( decoder );

	decoder.decode( "behavior_name", _behaviorName );
	decoder.decode( "override_target", _overrideTarget );
}
