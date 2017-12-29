#include "HasTargets.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::conditions;

HasTargets::HasTargets( void )
{

}

HasTargets::~HasTargets( void )
{

}

Behavior::State HasTargets::step( BehaviorContext *context )
{
	return context->hasTargets() ? Behavior::State::SUCCESS : Behavior::State::FAILURE;
}

