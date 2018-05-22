#include "CopyTransformFromTarget.hpp"

#include "SceneGraph/Node.hpp"

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

void CopyTransformFromTarget::encode( coding::Encoder &encoder )
{
	Behavior::encode( encoder );
}

void CopyTransformFromTarget::decode( coding::Decoder &decoder )
{
	Behavior::decode( decoder );
}

