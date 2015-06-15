#include "UpdateSystem.hpp"
#include "RenderSystem.hpp"

#include "Components/NodeComponentCatalog.hpp"
#include "Components/BehaviorComponent.hpp"

#include "Visitors/UpdateWorldState.hpp"
#include "Visitors/UpdateComponents.hpp"
#include "Visitors/ComputeRenderQueue.hpp"

#include "Rendering/RenderQueue.hpp"

#include "Simulation/Simulation.hpp"
#include "Simulation/TaskGroup.hpp"
#include "Simulation/Tasks/AsyncTask.hpp"
#include "Simulation/TaskManager.hpp"

#define CRIMILD_SIMULATION_TIME 1.0f / 60.0f

using namespace crimild;

UpdateSystem::UpdateSystem( void )
	: System( "Update" )
{
	enableUpdater();

    getUpdater()->setRepeatMode( Task::RepeatMode::REPEAT );
	getUpdater()->setThreadMode( Task::ThreadMode::BACKGROUND );
	getUpdater()->setSyncMode( Task::SyncMode::FRAME );
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
    
	System::update();

	auto scene = Simulation::getInstance()->getScene();
    if ( scene == nullptr ) {
    	Log::Debug << "No valid scene found" << Log::End;
        return;
    }
    
    auto camera = Simulation::getInstance()->getMainCamera();
    if ( camera == nullptr ) {
    	Log::Debug << "No camera detected" << Log::End;
        return;
    }

    const Time &t = Simulation::getInstance()->getSimulationTime();
    double dt = t.getDeltaTime();
    if ( dt > CRIMILD_SIMULATION_TIME ) dt = CRIMILD_SIMULATION_TIME;
    
    _accumulator += t.getDeltaTime();

    step();
}

void UpdateSystem::step( void )
{
    auto scene = Simulation::getInstance()->getScene();
    if ( scene == nullptr ) {
        Log::Debug << "No valid scene found" << Log::End;
        return;
    }
    
#if 0
    std::list< TaskPtr > tasks;
    NodeComponentCatalog< BehaviorComponent >::getInstance().forEach( [&tasks]( BehaviorComponent *b ) {
        if ( b->isEnabled() && b->getNode()->isEnabled() ) {
            tasks.push_back( crimild::alloc< AsyncTask >( [b]( void ) {
                b->update( Time( CRIMILD_SIMULATION_TIME ) );
            }));
        }
    });

    auto self = this;
    auto job = crimild::alloc< TaskGroup >( tasks, [self]() {
        self->_accumulator -= CRIMILD_SIMULATION_TIME;
        self->updateWorldState();
        
        if ( self->_accumulator >= CRIMILD_SIMULATION_TIME ) {
            self->step();
        }
        else {
            self->computeRenderQueue();
        }
    });
    
    broadcastMessage( messages::ExecuteTaskGroup { job } );
    
#else
    Time fixed;
    fixed.setDeltaTime( CRIMILD_SIMULATION_TIME );
    while ( _accumulator >= CRIMILD_SIMULATION_TIME ) {
        NodeComponentCatalog< BehaviorComponent >::getInstance().forEach( [&]( BehaviorComponent *behavior ) {
            if ( behavior != nullptr && behavior->isEnabled() && behavior->getNode()->isEnabled() ) {
                behavior->update( fixed );
            }
        });
        
        updateWorldState();
        
        _accumulator -= CRIMILD_SIMULATION_TIME;
    }

    computeRenderQueue();
#endif
}

void UpdateSystem::updateWorldState( void )
{
    auto scene = Simulation::getInstance()->getScene();
    if ( scene == nullptr ) {
        Log::Debug << "No valid scene found" << Log::End;
        return;
    }

    scene->perform( UpdateWorldState() );
}

void UpdateSystem::computeRenderQueue( void )
{
    auto scene = Simulation::getInstance()->getScene();
    if ( scene == nullptr ) {
        Log::Debug << "No valid scene found" << Log::End;
        return;
    }
    
    auto camera = Simulation::getInstance()->getMainCamera();
    if ( camera == nullptr ) {
        Log::Debug << "No camera detected" << Log::End;
        return;
    }
    
    CRIMILD_PROFILE( "Compute Render Queue" )
    auto renderQueue = crimild::alloc< RenderQueue >();
    scene->perform( ComputeRenderQueue( camera, renderQueue ) );
    broadcastMessage( messages::RenderQueueAvailable { renderQueue } );
}

void UpdateSystem::stop( void )
{
	System::stop();
}

