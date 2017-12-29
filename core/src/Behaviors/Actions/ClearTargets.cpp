#include "ClearTargets.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

ClearTargets::ClearTargets( void )
{

}

ClearTargets::~ClearTargets( void )
{
	
}

Behavior::State ClearTargets::step( BehaviorContext *context )
{
	context->removeAllTargets();
	return Behavior::State::SUCCESS;
}

