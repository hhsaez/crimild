#include "UpdateSystem.hpp"
#include "RenderSystem.hpp"

#include "Concurrency/Async.hpp"

#include "Visitors/UpdateWorldState.hpp"
#include "Visitors/ComputeRenderQueue.hpp"
#include "Visitors/UpdateComponents.hpp"

#include "Rendering/RenderQueue.hpp"

#include "SceneGraph/Node.hpp"

#include "Simulation/Simulation.hpp"

#define CRIMILD_SIMULATION_TIME 1.0f / 60.0f

using namespace crimild;

UpdateSystem::UpdateSystem( void )
	: System( "Update System" )
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
    CRIMILD_PROFILE( "Simulation step" )
    
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
    _accumulator += Numericd::min( 4 * CRIMILD_SIMULATION_TIME, c.getDeltaTime() );

    updateBehaviors( crimild::get_ptr( scene ) );

    computeRenderQueues( crimild::get_ptr( scene ) );
    
    // schedule next update
    crimild::concurrency::sync_frame( std::bind( &UpdateSystem::update, this ) );
}

void UpdateSystem::updateBehaviors( Node *scene )
{
    broadcastMessage( messaging::WillUpdateScene { scene } );

    static const Clock fixed( CRIMILD_SIMULATION_TIME );

    while ( _accumulator >= CRIMILD_SIMULATION_TIME ) {
        auto job = crimild::concurrency::async();
        scene->perform( Apply( [job]( Node *node ) {
            node->forEachComponent( [job, node] ( NodeComponent *component ) {
                crimild::concurrency::async( job, [component] {
                    component->update( fixed );
                });
            });
        }));
        crimild::concurrency::wait( job );
        _accumulator -= CRIMILD_SIMULATION_TIME;
    }
    
    updateWorldState( scene );

    broadcastMessage( messaging::DidUpdateScene { scene } );	
}

void UpdateSystem::updateWorldState( Node *scene )
{
    scene->perform( UpdateWorldState() );
}

void UpdateSystem::computeRenderQueues( Node *scene )
{
    CRIMILD_PROFILE( "Compute Render Queue" )

	auto renderQueueCollection = crimild::alloc< RenderQueueCollection >();
	
	Simulation::getInstance()->forEachCamera( [ this, &renderQueueCollection, scene ]( Camera *camera ) {
		if ( camera != nullptr && camera->isEnabled() ) {
			auto renderQueue = crimild::alloc< RenderQueue >();
			scene->perform( ComputeRenderQueue( camera, crimild::get_ptr( renderQueue ) ) );
			renderQueueCollection->add( renderQueue );
		}
	});
    
    crimild::concurrency::sync_frame( [this, renderQueueCollection]() {
        broadcastMessage( messaging::RenderQueueAvailable { renderQueueCollection } );
    });
}

void UpdateSystem::stop( void )
{
	System::stop();

    unregisterMessageHandler< messaging::SimulationWillUpdate >();
}

