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

    _accumulator += c.getDeltaTime();
    if ( _accumulator > 2 * Clock::DEFAULT_TICK_TIME ) {
        _accumulator = Clock::DEFAULT_TICK_TIME;
    }

	updateBehaviors( crimild::get_ptr( scene ) );

    computeRenderQueues( crimild::get_ptr( scene ) );

    // tick after update
    c.tick();

    // schedule next update
    crimild::concurrency::sync_frame( std::bind( &UpdateSystem::update, this ) );
}

void UpdateSystem::updateBehaviors( Node *scene )
{
    broadcastMessage( messaging::WillUpdateScene { scene } );

    static const auto FIXED_CLOCK = Clock( Clock::DEFAULT_TICK_TIME );

    while ( _accumulator >= Clock::DEFAULT_TICK_TIME ) {
        CRIMILD_PROFILE( "Updating Components" )
		
        scene->perform( Apply( []( Node *node ) {
            node->updateComponents( FIXED_CLOCK );
        }));
    
        updateWorldState( scene );

        _accumulator -= Clock::DEFAULT_TICK_TIME;
    }

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

