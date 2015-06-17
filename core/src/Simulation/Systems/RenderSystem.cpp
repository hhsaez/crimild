#include "RenderSystem.hpp"

#include "Simulation/Simulation.hpp"

#include "Concurrency/Async.hpp"

using namespace crimild;

RenderSystem::RenderSystem( void )
	: System( "Render System" )
{
	registerMessageHandler< messaging::RenderQueueAvailable >( [&]( messaging::RenderQueueAvailable const &message ) {
        if ( _renderQueue != nullptr ) {
            if ( _renderQueue->getTimestamp() >= message.renderQueue->getTimestamp() ) {
                return;
            }
        }
        
		_renderQueue = message.renderQueue;
	});
}

RenderSystem::~RenderSystem( void )
{

}

bool RenderSystem::start( void )
{	
	if ( !System::start() ) {
		return false;
	}
    
    crimild::async( crimild::AsyncDispatchPolicy::MAIN_QUEUE, std::bind( &RenderSystem::renderFrame, this ) );

	return true;
}

void RenderSystem::renderFrame( void )
{
	CRIMILD_PROFILE( "Render System" );

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
    
    broadcastMessage( messaging::WillRenderScene {} );
    
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
        // move to own system
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
    
    broadcastMessage( messaging::DidRenderScene {} );
    
    crimild::async( crimild::AsyncDispatchPolicy::MAIN_QUEUE, std::bind( &RenderSystem::renderFrame, this ) );
}

void RenderSystem::stop( void )
{
	System::stop();
}

