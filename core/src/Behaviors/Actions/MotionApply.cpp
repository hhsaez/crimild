#include "MotionApply.hpp"

#include "Components/MotionStateComponent.hpp"
#include "Crimild_Mathematics.hpp"
#include "Navigation/NavigationController.hpp"
#include "SceneGraph/Node.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;
using namespace crimild::navigation;

Vector3f clamp( Vector3f v, Real lo, Real hi ) noexcept
{
    const auto L = length( v );
    if ( isZero( L ) ) {
        return v;
    }
    auto i = abs( hi - lo ) / L;
    i = i > 0 && i < 1.0 ? i : 1.0;
    return v * i;
}

void MotionApply::init( BehaviorContext *context )
{
    Behavior::init( context );

    auto agent = context->getAgent();
    if ( agent == nullptr ) {
        return;
    }

    m_motion = agent->getComponent< MotionState >();
    if ( m_motion == nullptr ) {
        m_motion = agent->attachComponent< MotionState >();
    }
}

Behavior::State MotionApply::step( BehaviorContext *context )
{
    auto agent = context->getAgent();
    if ( agent == nullptr ) {
        CRIMILD_LOG_WARNING( "Attempting to use MotionReset behavior without an agent" );
        return Behavior::State::FAILURE;
    }

    if ( m_motion == nullptr ) {
        CRIMILD_LOG_WARNING( "MotionState not initialized" );
        return Behavior::State::FAILURE;
    }

    const auto dt = context->getClock().getDeltaTime();

    // Use references to simplify code
    auto &position = m_motion->position;
    auto &velocity = m_motion->velocity;
    auto &steering = m_motion->steering;
    auto &maxVelocity = m_motion->maxVelocity;

    const auto mass = m_motion->mass;
    const auto maxForce = m_motion->maxForce;

    steering = clamp( steering, Vector3( 0 ), Vector3( maxForce ) );

    if ( isZero( mass ) ) {
        // If there is no mass, then motion is instantaneus and no inertia is applied
        velocity = steering;
    } else {
        // Agent has a mass, so movement should account for some inertia.
        steering = steering / mass;
        // velocity = clamp( velocity + steering, Vector3::Constants::ZERO, maxVelocity );
        velocity = velocity + steering;
    }

    if ( !isZero( velocity ) ) {
        velocity = normalize( velocity );
    }

    position = position + dt * velocity;

    agent->setLocal( translation( Vector3f( position ) ) );

#if 0
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
    } else {
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
#endif

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
