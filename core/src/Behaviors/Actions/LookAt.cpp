#include "LookAt.hpp"

#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Transformation_lookAt.hpp"
#include "SceneGraph/Node.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

LookAt::LookAt( void )
{
}

LookAt::LookAt( const Vector3f &target, crimild::Real32 duration )
    : _target( target )
{
}

LookAt::~LookAt( void )
{
}

void LookAt::init( BehaviorContext *context )
{
    Behavior::init( context );

    _clock.reset();
}

Behavior::State LookAt::step( BehaviorContext *context )
{
    auto agent = context->getAgent();

    if ( !context->hasTargets() ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Behavior requires at least one target" );
        return Behavior::State::FAILURE;
    }

    auto target = context->getTargetAt( 0 );
    auto targetPos = location( target->getLocal() );
    auto agentPos = location( agent->getLocal() );

    agent->setLocal(
        lookAt(
            agentPos,
            targetPos,
            Vector3 { 0, 1, 0 }
        )
    );

    return Behavior::State::SUCCESS;
}

void LookAt::encode( coding::Encoder &encoder )
{
    Behavior::encode( encoder );

    encoder.encode( "target", _target );
}

void LookAt::decode( coding::Decoder &decoder )
{
    Behavior::decode( decoder );

    decoder.decode( "target", _target );
}
