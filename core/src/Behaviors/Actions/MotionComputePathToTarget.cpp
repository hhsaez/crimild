#include "MotionComputePathToTarget.hpp"

#include "Navigation/NavigationController.hpp"

#include "SceneGraph/Node.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;
using namespace crimild::navigation;

MotionComputePathToTarget::MotionComputePathToTarget( void )
{

}

MotionComputePathToTarget::~MotionComputePathToTarget( void )
{

}

Behavior::State MotionComputePathToTarget::step( BehaviorContext *context )
{
	auto agent = context->getAgent();
	if ( !context->hasTargets() ) {
		Log::debug( CRIMILD_CURRENT_CLASS_NAME, "No target defined for behavior" );
		return Behavior::State::FAILURE;
	}
	
	auto target = context->getTargetAt( 0 );

	auto nav = agent->getComponent< NavigationController >();
	if ( nav != nullptr ) {
		auto path = nav->computePathToTarget( target->getLocal().getTranslate() );
		if ( path.size() == 0 ) {
			// no path
			return Behavior::State::FAILURE;
		}
		else {
			context->setValue( "motion.target", path[ 0 ] );
		}
	}

	return Behavior::State::SUCCESS;
}

void MotionComputePathToTarget::encode( coding::Encoder &encoder )
{
	Behavior::encode( encoder );
}

void MotionComputePathToTarget::decode( coding::Decoder &decoder )
{
	Behavior::decode( decoder );
}

