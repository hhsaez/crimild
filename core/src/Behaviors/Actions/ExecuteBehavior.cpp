#include "ExecuteBehavior.hpp"

#include "Behaviors/BehaviorController.hpp"
#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"
#include "SceneGraph/Node.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

ExecuteBehavior::ExecuteBehavior( void )
{
}

ExecuteBehavior::ExecuteBehavior( std::string behaviorName )
    : _behaviorName( behaviorName )
{
}

ExecuteBehavior::~ExecuteBehavior( void )
{
}

Behavior::State ExecuteBehavior::step( BehaviorContext *context )
{
    assert( false );
    // auto agent = context->getAgent();
    // if ( agent == nullptr ) {
    // 	return Behavior::State::FAILURE;
    // }

    // auto behaviors = agent->getComponent< BehaviorController >();
    // if ( behaviors == nullptr ) {
    // 	Log::error( CRIMILD_CURRENT_CLASS_NAME, "Agent does not have behaviors" );
    // 	return Behavior::State::FAILURE;
    // }

    // if ( !behaviors->executeBehaviorTree( _behaviorName ) ) {
    // 	Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot execute behavior on agent with name ", _behaviorName );
    // 	return Behavior::State::FAILURE;
    // }

    return Behavior::State::SUCCESS;
}

void ExecuteBehavior::encode( coding::Encoder &encoder )
{
    Behavior::encode( encoder );

    encoder.encode( "behaviorName", _behaviorName );
}

void ExecuteBehavior::decode( coding::Decoder &decoder )
{
    Behavior::decode( decoder );

    decoder.decode( "behaviorName", _behaviorName );
}
