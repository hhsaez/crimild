#include "IsAtTarget.hpp"

#include "Mathematics/Distance.hpp"

#include "SceneGraph/Node.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::conditions;

IsAtTarget::IsAtTarget( crimild::Real32 minDistance )
	: _minDistance( minDistance )
{

}

IsAtTarget::~IsAtTarget( void )
{

}

Behavior::State IsAtTarget::step( BehaviorContext *context )
{
	auto agent = context->getAgent();

	if ( !context->hasTargets() ) {
		return Behavior::State::FAILURE;
	}
	
	auto target = context->getTargetAt( 0 );

	auto d = Distance::compute( agent->getLocal().getTranslate(), target->getLocal().getTranslate() );
	return d <= _minDistance ? Behavior::State::SUCCESS : Behavior::State::FAILURE;
}

