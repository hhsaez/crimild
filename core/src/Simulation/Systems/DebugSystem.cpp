#include "DebugSystem.hpp"

#include "Simulation/Simulation.hpp"

#include "Debug/DebugRenderHelper.hpp"

#include "Visitors/Apply.hpp"

#include "Concurrency/Async.hpp"

using namespace crimild;

DebugSystem::DebugSystem( void )
	: System( "Debug System" )
{
    CRIMILD_BIND_MEMBER_MESSAGE_HANDLER( messaging::DidRenderScene, DebugSystem, onDidRenderScene );
    CRIMILD_BIND_MEMBER_MESSAGE_HANDLER( messaging::ToggleDebugInfo, DebugSystem, onToggleDebugInfo );
}

DebugSystem::~DebugSystem( void )
{

}

bool DebugSystem::start( void )
{	
	if ( !System::start() ) {
		return false;
	}
    
    DebugRenderHelper::init();
    
	return true;
}

void DebugSystem::stop( void )
{
	System::stop();
}

void DebugSystem::onDidRenderScene( messaging::DidRenderScene const & )
{
    auto renderer = Simulation::getInstance()->getRenderer();
    auto scene = Simulation::getInstance()->getScene();
    auto camera = Simulation::getInstance()->getMainCamera();
    
    if ( renderer == nullptr || scene == nullptr || camera == nullptr ) {
        return;
    }
    
    if ( _debugInfoEnabled ) {
        scene->perform( Apply( [renderer, camera]( NodePtr const &node ) {
            node->foreachComponent( [renderer, camera]( NodeComponentPtr const &component ) {
                component->renderDebugInfo( renderer, camera );
            });
        }));
    }
}

void DebugSystem::onToggleDebugInfo( messaging::ToggleDebugInfo const & )
{
    _debugInfoEnabled = !_debugInfoEnabled;
}

