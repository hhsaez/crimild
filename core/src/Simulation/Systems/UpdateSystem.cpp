#include "UpdateSystem.hpp"
#include "RenderSystem.hpp"

#include "Visitors/UpdateWorldState.hpp"
#include "Visitors/ComputeRenderQueue.hpp"
#include "Visitors/UpdateComponents.hpp"
#include "Visitors/ParallelApply.hpp"

#include "Rendering/RenderQueue.hpp"

#include "SceneGraph/Node.hpp"

#include "Simulation/Simulation.hpp"

using namespace crimild;

bool UpdateSystem::start( void )
{	
	if ( !System::start() ) {
		return false;
	}

    auto settings = Simulation::getInstance()->getSettings();
    _targetFrameTime = settings->get< crimild::Real64 >( "simulation.targetFrameRate", 1.0 / 60.0 );
    _accumulator = 0.0;
    
	return true;
}

void UpdateSystem::update( void )
{
    CRIMILD_PROFILE( "Update System" )
    
    auto scene = crimild::retain( Simulation::getInstance()->getScene() );
    if ( scene == nullptr ) {
        return;
    }
    
    auto &c = Simulation::getInstance()->getSimulationClock();
    _accumulator += c.getDeltaTime();
    if ( _accumulator > 2 * _targetFrameTime ) {
		_accumulator = 0;
    }

	updateBehaviors( crimild::get_ptr( scene ) );

	// TODO: move to another system
    computeRenderQueues( crimild::get_ptr( scene ) );
}

void UpdateSystem::updateBehaviors( Node *scene )
{
    broadcastMessage( messaging::WillUpdateScene { scene } );

    static const auto FIXED_CLOCK = Clock( _targetFrameTime );

    while ( _accumulator >= _targetFrameTime ) {
        CRIMILD_PROFILE( "Updating Components" )
		
        scene->perform( Apply( []( Node *node ) {
            node->updateComponents( FIXED_CLOCK );
        }));

		{
			CRIMILD_PROFILE( "Updating World State" )
			scene->perform( UpdateWorldState() );
		}			

        _accumulator -= _targetFrameTime;
    }

    broadcastMessage( messaging::DidUpdateScene { scene } );	
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
    
	broadcastMessage( messaging::RenderQueueAvailable { renderQueues } );
}

