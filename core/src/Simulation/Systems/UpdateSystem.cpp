#include "UpdateSystem.hpp"
#include "RenderSystem.hpp"

#include "Concurrency/Async.hpp"

#include "Visitors/UpdateWorldState.hpp"
#include "Visitors/ComputeRenderQueue.hpp"
#include "Visitors/UpdateComponents.hpp"
#include "Visitors/ParallelApply.hpp"

#include "Rendering/RenderQueue.hpp"

#include "SceneGraph/Node.hpp"

#include "Simulation/Simulation.hpp"

using namespace crimild;

UpdateSystem::UpdateSystem( void )
{

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
    
    crimild::concurrency::sync_frame( std::bind( &UpdateSystem::update, this ) );

	return true;
}

void UpdateSystem::update( void )
{
    CRIMILD_PROFILE( "Update System" )
    
    MessageQueue::getInstance()->dispatchDeferredMessages();
    
    auto scene = crimild::retain( Simulation::getInstance()->getScene() );
    if ( scene == nullptr ) {
    	// schedule next update
        crimild::concurrency::sync_frame( std::bind( &UpdateSystem::update, this ) );
        return;
    }
    
    // update simulation time
    // TODO: Should this system have its own clock?
    auto &c = Simulation::getInstance()->getSimulationClock();
    c.tick();
    
    // prevent integration errors when delta is too big (i.e. after loading a new scene)
    _accumulator += Numericd::min( 4 * Clock::getScaledTickTime(), c.getDeltaTime() );

	updateBehaviors( crimild::get_ptr( scene ) );

    computeRenderQueues( crimild::get_ptr( scene ) );
    
    // schedule next update
    crimild::concurrency::sync_frame( std::bind( &UpdateSystem::update, this ) );
}

void UpdateSystem::updateBehaviors( Node *scene )
{
    broadcastMessage( messaging::WillUpdateScene { scene } );

    // So, we're updating with a fixed time which may seem wrong
    // BUT! We're either sleeping on WindowSystem or waiting for OS display callback
    // TODO: This should be configurable.
    static const auto FIXED_CLOCK = Clock( Clock::DEFAULT_TICK_TIME );

	CRIMILD_PROFILE( "Updating Components" )
		
	scene->perform( Apply( []( Node *node ) {
		node->updateComponents( FIXED_CLOCK );
	}));
    
    updateWorldState( scene );

    broadcastMessage( messaging::DidUpdateScene { scene } );	
}

void UpdateSystem::updateWorldState( Node *scene )
{
	CRIMILD_PROFILE( "Updating World State" )
	
    scene->perform( UpdateWorldState() );
}

void UpdateSystem::computeRenderQueues( Node *scene )
{
	containers::Array< SharedPointer< RenderQueue >> renderQueues;

	{
		CRIMILD_PROFILE( "Compute Render Queue" )
	
		Simulation::getInstance()->forEachCamera( [ &renderQueues, scene ]( Camera *camera ) {
			if ( camera != nullptr && camera->isEnabled() ) {
				auto renderQueue = crimild::alloc< RenderQueue >();
				scene->perform( ComputeRenderQueue( camera, crimild::get_ptr( renderQueue ) ) );
				renderQueues.add( renderQueue );
			}
		});
	}
    
    crimild::concurrency::sync_frame( [ this, renderQueues ]() {
        broadcastMessage( messaging::RenderQueueAvailable { renderQueues } );
    });
}

void UpdateSystem::stop( void )
{
	System::stop();

    unregisterMessageHandler< messaging::SimulationWillUpdate >();
}

