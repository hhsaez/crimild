#include "MotionReset.hpp"

#include "SceneGraph/Node.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

MotionReset::MotionReset( void )
{

}

MotionReset::~MotionReset( void )
{

}

void MotionReset::init( BehaviorContext *context )
{
	Behavior::init( context );

	if ( !context->hasValue( "motion.velocity.x" ) ) context->setValue( "motion.velocity.x", 0.0f );
	if ( !context->hasValue( "motion.velocity.y" ) ) context->setValue( "motion.velocity.y", 0.0f );
	if ( !context->hasValue( "motion.velocity.z" ) ) context->setValue( "motion.velocity.z", 0.0f );
	if ( !context->hasValue( "motion.velocity.magnitude" ) ) context->setValue( "motion.velocity.magnitude", 0.0f );

	if ( !context->hasValue( "motion.steering.x" ) ) context->setValue( "motion.steering.x", 0.0f );
	if ( !context->hasValue( "motion.steering.y" ) ) context->setValue( "motion.steering.y", 0.0f );
	if ( !context->hasValue( "motion.steering.z" ) ) context->setValue( "motion.steering.z", -1.0f );

	if ( !context->hasValue( "motion.max_velocity" ) ) context->setValue( "motion.max_velocity", 1.0f );
	if ( !context->hasValue( "motion.max_force" ) ) context->setValue( "motion.max_force", 1.0f );
	if ( !context->hasValue( "motion.mass" ) ) context->setValue( "motion.mass", 1.0f );
	if ( !context->hasValue( "motion.slowing_radius" ) ) context->setValue( "motion.slowing_radius", 1.0f );

}

Behavior::State MotionReset::step( BehaviorContext *context )
{
	auto agent = context->getAgent();

	context->setValue( "motion.position", agent->getLocal().getTranslate() );
	context->setValue( "motion.steering", Vector3f::ZERO );

	if ( context->hasTargets() ) {
		auto target = context->getTargetAt( 0 );
		context->setValue( "motion.target", target->getLocal().getTranslate() );
	}
	else {
		// set target to self so no motion will be applied
		context->setValue( "motion.target", agent->getLocal().getTranslate() );
	}

	return Behavior::State::SUCCESS;
}

void MotionReset::encode( coding::Encoder &encoder )
{
	Behavior::encode( encoder );
}

void MotionReset::decode( coding::Decoder &decoder )
{
	Behavior::decode( decoder );
}

