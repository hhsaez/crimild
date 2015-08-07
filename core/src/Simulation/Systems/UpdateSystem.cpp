#include "UpdateSystem.hpp"
#include "RenderSystem.hpp"

#include "Concurrency/Async.hpp"

#include "Components/NodeComponentCatalog.hpp"
#include "Components/BehaviorComponent.hpp"

#include "Visitors/UpdateWorldState.hpp"
#include "Visitors/UpdateComponents.hpp"
#include "Visitors/ComputeRenderQueue.hpp"

#include "Rendering/RenderQueue.hpp"

#include "SceneGraph/Node.hpp"

#include "Simulation/Simulation.hpp"
#include "Simulation/Tasks/AsyncTask.hpp"

#define CRIMILD_SIMULATION_TIME 1.0f / 60.0f

using namespace crimild;

UpdateSystem::UpdateSystem( void )
	: System( "Update System" )
{
    auto weakSelf = this;
    registerMessageHandler< messaging::SimulationWillUpdate >( [weakSelf]( messaging::SimulationWillUpdate const &message ) {
        crimild::async( AsyncDispatchPolicy::BACKGROUND_QUEUE_SYNC, std::bind( &UpdateSystem::update, weakSelf ) );
    });
}

UpdateSystem::~UpdateSystem( void )
{

}

bool UpdateSystem::start( void )
{	
	if ( !System::start() ) {
		return false;
	}
    
    _accumulator = 0.0;
    
	return true;
}

void UpdateSystem::update( void )
{
    CRIMILD_PROFILE( "Simulation step" )
    
	auto scene = Simulation::getInstance()->getScene();
    if ( scene == nullptr ) {
    	Log::Debug << "No scene found" << Log::End;
        return;
    }
    
    auto camera = Simulation::getInstance()->getMainCamera();
    if ( camera == nullptr ) {
    	Log::Debug << "No camera detected" << Log::End;
        return;
    }

    const Clock &c = Simulation::getInstance()->getSimulationClock();
    _accumulator += Numericd::min( CRIMILD_SIMULATION_TIME, c.getDeltaTime() );


    broadcastMessage( messaging::WillUpdateScene { scene, camera } );
    updateBehaviors( scene );
    broadcastMessage( messaging::DidUpdateScene { scene, camera } );

    computeRenderQueue( scene, camera );
}

void UpdateSystem::updateBehaviors( NodePtr const &scene )
{
    Clock fixed( CRIMILD_SIMULATION_TIME );
    while ( _accumulator >= CRIMILD_SIMULATION_TIME ) {
        NodeComponentCatalog< BehaviorComponent >::getInstance().forEach( [&]( BehaviorComponent *behavior ) {
            if ( behavior != nullptr && behavior->isEnabled() && behavior->getNode() != nullptr && behavior->getNode()->isEnabled() ) {
                behavior->update( fixed );
            }
        });
        
        updateWorldState( scene );
        
        _accumulator -= CRIMILD_SIMULATION_TIME;
    }
}

void UpdateSystem::updateWorldState( NodePtr const &scene )
{
    scene->perform( UpdateWorldState() );
}

void UpdateSystem::computeRenderQueue( NodePtr const &scene, CameraPtr const &camera )
{
    CRIMILD_PROFILE( "Compute Render Queue" )
    auto renderQueue = crimild::alloc< RenderQueue >();
    scene->perform( ComputeRenderQueue( camera, renderQueue ) );
    broadcastMessage( messaging::RenderQueueAvailable { renderQueue } );
}

void UpdateSystem::stop( void )
{
	System::stop();
}

