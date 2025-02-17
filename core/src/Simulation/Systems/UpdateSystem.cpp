#include "UpdateSystem.hpp"

#include "RenderSystem.hpp"
#include "Rendering/RenderQueue.hpp"
#include "SceneGraph/Node.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/ComputeRenderQueue.hpp"
#include "Visitors/ParallelApply.hpp"
#include "Visitors/UpdateComponents.hpp"
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild;

void UpdateSystem::start( void ) noexcept
{
    /*
	if ( !System::start() ) {
		return false;
	}

    auto settings = Simulation::getInstance()->getSettings();
    _targetFrameTime = settings->get< crimild::Real64 >( "simulation.targetFrameRate", 1.0 / 60.0 );
    _accumulator = 0.0;

	registerMessageHandler< messaging::SceneChanged >(
		[ this ]( messaging::SceneChanged const & ) {
			_skipFrames = 2;
		}
	);
    */

    //return true;
}

void UpdateSystem::update( void ) noexcept
{
    CRIMILD_PROFILE( "Update System" )

    auto scene = crimild::retain( Simulation::getInstance()->getScene() );
    if ( scene == nullptr ) {
        return;
    }

    auto clock = Simulation::getInstance()->getSimulationClock();

    scene->perform( UpdateComponents( clock ) );
    scene->perform( UpdateWorldState() );

    /*

	if ( _skipFrames > 0 ) {
		_accumulator = 0;
	}
	else {
		auto &c = Simulation::getInstance()->getSimulationClock();
		_accumulator += c.getDeltaTime();
	}

	updateBehaviors( crimild::get_ptr( scene ) );

	// TODO: move to another system
    computeRenderQueues( crimild::get_ptr( scene ) );

	if ( _skipFrames > 0 ) {
		--_skipFrames;
	}
    */
}

void UpdateSystem::updateBehaviors( Node *scene )
{
    // broadcastMessage( messaging::WillUpdateScene { scene } );

    const auto FIXED_CLOCK = Clock( _targetFrameTime );

    while ( _accumulator >= _targetFrameTime ) {
        CRIMILD_PROFILE( "Updating Components" )

        scene->perform( Apply( [ FIXED_CLOCK ]( Node *node ) {
            node->updateComponents( FIXED_CLOCK );
        } ) );

        {
            CRIMILD_PROFILE( "Updating World State" )
            scene->perform( UpdateWorldState() );
        }

        _accumulator -= _targetFrameTime;
    }

    // broadcastMessage( messaging::DidUpdateScene { scene } );
}

void UpdateSystem::computeRenderQueues( Node *scene )
{
    Array< SharedPointer< RenderQueue > > renderQueues;

    {
        CRIMILD_PROFILE( "Compute Render Queue" )

        auto enableCulling = _skipFrames <= 0;

        Simulation::getInstance()->forEachCamera( [ &renderQueues, scene, enableCulling ]( Camera *camera ) {
            if ( camera != nullptr && camera->isEnabled() ) {
                camera->setCullingEnabled( enableCulling );
                auto renderQueue = crimild::alloc< RenderQueue >();
                scene->perform( ComputeRenderQueue( camera, crimild::get_ptr( renderQueue ) ) );
                renderQueues.add( renderQueue );
            }
        } );
    }

    // broadcastMessage( messaging::RenderQueueAvailable { renderQueues } );
}
