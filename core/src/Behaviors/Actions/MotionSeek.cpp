#include "MotionSeek.hpp"

#include "SceneGraph/Node.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

MotionSeek::MotionSeek( void )
{
}

MotionSeek::~MotionSeek( void )
{
}

Behavior::State MotionSeek::step( BehaviorContext *context )
{
    assert( false && "TODO" );
#if 0
	auto velocity = context->getValue< Vector3f >( "motion.velocity" );
	auto targetPosition = context->getValue< Vector3f >( "motion.target" );
	auto position = context->getValue< Vector3f >( "motion.position" );
	auto steering = context->getValue< Vector3f >( "motion.steering" );

	auto maxVelocity = context->getValue< crimild::Real32 >( "motion.max_velocity" );
	auto slowingRadius = context->getValue< crimild::Real32 >( "motion.slowing_radius" );

	auto desiredVelocity = targetPosition - position;
	auto distance = desiredVelocity.getMagnitude();

	if ( distance == 0 ) {
		// agent and target are at the exact same location
		// nothing to do here
		return Behavior::State::SUCCESS;
	}

	auto arrivalCoeff = distance < slowingRadius ? ( distance / slowingRadius ) : 1.0f;

	// ignore arrival coefficient if we're following a path
	auto target = context->getTargetAt( 0 );
	if ( target->getLocal().getTranslate() != targetPosition ) {
		arrivalCoeff = 1.0f;
	}

	desiredVelocity = desiredVelocity.getNormalized() * arrivalCoeff * maxVelocity;

    steering += desiredVelocity - velocity;

	context->setValue( "motion.steering", steering );
#endif

    return Behavior::State::SUCCESS;
}

void MotionSeek::encode( coding::Encoder &encoder )
{
    Behavior::encode( encoder );
}

void MotionSeek::decode( coding::Decoder &decoder )
{
    Behavior::decode( decoder );
}
