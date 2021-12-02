#include "GameController.hpp"

#include "messaging/GameOver.hpp"

using namespace crimild;
using namespace crimild::scripting;
using namespace crimild::bricks;

CRIMILD_SCRIPTING_REGISTER_COMPONENT_BUILDER( crimild::bricks::GameController )

GameController::GameController( ScriptEvaluator &eval )
{
	registerMessageHandler< GameOver >( []( GameOver const & ) {
		MessageQueue::getInstance()->pushMessage( messaging::ReloadScene { } );
	});
}

GameController::~GameController( void )
{

}

void GameController::start( void )
{

}

void GameController::update( const Clock & )
{

}

