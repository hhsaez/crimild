#include "LookAt.hpp"

#include "SceneGraph/Node.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

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
	auto dir = target->getLocal().getTranslate() - agent->getLocal().getTranslate();
	dir[ 1 ] = 0.0f;
	dir.normalize();
	agent->local().setRotate( Quaternion4f::createFromDirection( dir ) );
	
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
