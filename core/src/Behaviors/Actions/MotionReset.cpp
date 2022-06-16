#include "MotionReset.hpp"

#include "Mathematics/Point3_constants.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Vector3_constants.hpp"
#include "SceneGraph/Node.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

void MotionReset::init( BehaviorContext *context )
{
    Behavior::init( context );

    m_velocity = context->getOrCreate( "motion.velocity", Vector3::Constants::ZERO );
    m_position = context->getOrCreate( "motion.position", Point3::Constants::ZERO );
    m_steering = context->getOrCreate( "motion.steering", Vector3::Constants::ZERO );

    context->getOrCreate( "motion.velocity.magnitude", 0.0f );
    context->getOrCreate( "motion.velocity.max", 1.0f );
    context->getOrCreate( "motion.maxForce", 1.0f );
    context->getOrCreate( "motion.mass", 1.0f );
    context->getOrCreate( "motion.slowingRadius", 1.0f );
}

Behavior::State MotionReset::step( BehaviorContext *context )
{
    auto agent = context->getAgent();
    if ( agent == nullptr ) {
        CRIMILD_LOG_WARNING( "Attempting to use MotionReset behavior without an agent" );
        return Behavior::State::FAILURE;
    }

    m_position->get< Point3 >() = location( agent->getLocal() );
    m_steering->get< Vector3 >() = Vector3::Constants::ZERO;

    // if ( context->hasTargets() ) {
    //     auto target = context->getTargetAt( 0 );
    //     const auto targetLocation = location( target->getLocal() );
    //     context->setValue( "motion.target", targetLocation );
    // } else {
    //     // set target to self so no motion will be applied
    //     context->setValue( "motion.target", agentLocation );
    // }

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
