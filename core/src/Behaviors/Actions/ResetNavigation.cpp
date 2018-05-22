#include "ResetNavigation.hpp"

#include "Navigation/NavigationController.hpp"
#include "SceneGraph/Node.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;
using namespace crimild::navigation;

ResetNavigation::ResetNavigation( void )
{

}

ResetNavigation::~ResetNavigation( void )
{
	
}

Behavior::State ResetNavigation::step( BehaviorContext *context )
{
	auto agent = context->getAgent();
	auto nav = agent->getComponent< NavigationController >();
	if ( nav != nullptr ) {
		nav->teleport( agent->getLocal().getTranslate() );
	}

	return Behavior::State::SUCCESS;
}

void ResetNavigation::encode( coding::Encoder &encoder )
{
	Behavior::encode( encoder );
}

void ResetNavigation::decode( coding::Decoder &decoder )
{
	Behavior::decode( decoder );
}

