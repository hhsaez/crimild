#include "CopyTransformFromTarget.hpp"

#include "SceneGraph/Node.hpp"
#include "Components/BehaviorController.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

CopyTransformFromTarget::CopyTransformFromTarget( void )
{

}

CopyTransformFromTarget::~CopyTransformFromTarget( void )
{

}

Behavior::State CopyTransformFromTarget::step( BehaviorContext *context )
{
	if ( !context->hasTargets() ) {
		return Behavior::State::FAILURE;
	}

	auto target = context->getTargetAt( 0 );
	if ( target == nullptr ) {
		return Behavior::State::FAILURE;
	}

	auto agent = context->getAgent();
	agent->setLocal( target->getLocal() );
	
	return Behavior::State::SUCCESS;
}

