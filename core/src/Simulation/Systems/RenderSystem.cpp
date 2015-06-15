#include "RenderSystem.hpp"

#include "Simulation/Simulation.hpp"

using namespace crimild;

RenderSystem::RenderSystem( void )
	: System( "Render" )
{
	registerMessageHandler< messages::RenderQueueAvailable >( [&]( messages::RenderQueueAvailable const &message ) {
        if ( _renderQueue != nullptr ) {
            if ( _renderQueue->getTimestamp() >= message.renderQueue->getTimestamp() ) {
                return;
            }
        }
        
		_renderQueue = message.renderQueue;
	});

	enableUpdater();
}

RenderSystem::~RenderSystem( void )
{

}

bool RenderSystem::start( void )
{	
	if ( !System::start() ) {
		return false;
	}

	return true;
}

void RenderSystem::update( void )
{
	CRIMILD_PROFILE( "Render System" );
	System::update();

    auto renderer = Simulation::getInstance()->getRenderer();
	if ( renderer == nullptr ) {
		return;
	}

	auto renderQueue = _renderQueue;

	{
		CRIMILD_PROFILE( "Begin Render" );
		renderer->beginRender();
		renderer->clearBuffers();
	}

	{
		CRIMILD_PROFILE( "Render Scene" );
		if ( renderQueue != nullptr ) {
	    	renderer->render( renderQueue, renderQueue->getCamera()->getRenderPass() );
	    }
	}

    {
        CRIMILD_PROFILE( "End Render" );
        renderer->endRender();
    }

#if 0
    {
        Profiler::getInstance()->dump();
        
        static double accum = 0.0;
        auto t = Simulation::getInstance()->getSimulationTime();
        accum += t.getDeltaTime() * 100.0;
        if ( accum >= 1.0 ) {
            Profiler::getInstance()->resetAll();
            accum = 0.0;
        }
    }
#endif
}

void RenderSystem::stop( void )
{
	System::stop();
}

