#include "MotionApply.hpp"

#include "Navigation/NavigationController.hpp"

#include "SceneGraph/Node.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;
using namespace crimild::navigation;

Vector3f truncate( Vector3f v, float max )
{
	auto i = max / ( Numericf::ZERO_TOLERANCE + v.getMagnitude() );
    i = i > 0 && i < 1.0 ? i : 1.0;
	return v * i;
}

MotionApply::MotionApply( void )
{

}

MotionApply::~MotionApply( void )
{

}

Behavior::State MotionApply::step( BehaviorContext *context )
{
	auto velocity = context->getValue< Vector3f >( "motion.velocity" );
	auto steering = context->getValue< Vector3f >( "motion.steering" );
	auto position = context->getValue< Vector3f >( "motion.position" );

	auto maxForce = context->getValue< crimild::Real32 >( "motion.max_force" );
	steering = steering.getTruncated( maxForce );

	auto mass = context->getValue< crimild::Real32 >( "motion.mass" );
	steering /= mass;

	auto maxVelocity = context->getValue< crimild::Real32 >( "motion.max_velocity" );
	velocity = truncate( velocity + steering, maxVelocity );

	position += context->getClock().getDeltaTime() * velocity;
	
	auto agent = context->getAgent();
	auto nav = agent->getComponent< NavigationController >();
	if ( nav != nullptr ) {
		nav->move( position );
		position = agent->getLocal().getTranslate();
	}
	else {
		agent->local().setTranslate( position );
	}

	auto velocityMagnitude = velocity.getMagnitude();
	
	if ( velocityMagnitude > 1.0f ) {
		// only apply rotation if needed
		auto dir = velocity;
		dir[ 1 ] = 0.0f;
		dir.normalize();
		agent->local().setRotate( Quaternion4f::createFromDirection( dir ) );
	}
	
	context->setValue( "motion.position", position );
	context->setValue( "motion.velocity", velocity );
	context->setValue( "motion.velocity.magnitude", velocityMagnitude );
	
	return Behavior::State::SUCCESS;
}

void MotionApply::encode( coding::Encoder &encoder )
{
	Behavior::encode( encoder );
}

void MotionApply::decode( coding::Decoder &decoder )
{
	Behavior::decode( decoder );
}

