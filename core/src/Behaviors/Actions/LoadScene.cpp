#include "LoadScene.hpp"

#include "Simulation/Simulation.hpp"
#include "Concurrency/Async.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

LoadScene::LoadScene( void )
{
	
}

LoadScene::LoadScene( std::string sceneFileName )
	: _sceneFileName( sceneFileName )
{

}

LoadScene::~LoadScene( void )
{

}

Behavior::State LoadScene::step( BehaviorContext *context )
{
    auto filename = _sceneFileName;
    crimild::concurrency::sync_frame( [filename] {
        Simulation::getInstance()->setScene( nullptr );
        Simulation::getInstance()->loadScene( filename );
    });
    
	return Behavior::State::SUCCESS;
}

void LoadScene::encode( coding::Encoder &encoder )
{
	Behavior::encode( encoder );

	encoder.encode( "fileName", _sceneFileName );
}

void LoadScene::decode( coding::Decoder &decoder )
{
	Behavior::decode( decoder );

	decoder.decode( "fileName", _sceneFileName );
}
