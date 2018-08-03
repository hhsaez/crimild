/*
 * Copyright (c) 2013, Hernan Saez
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "RenderSystem.hpp"

#include "Rendering/RenderPasses/RenderPass.hpp"
#include "Rendering/FrameBufferObject.hpp"

#include "Simulation/Simulation.hpp"

#include "Concurrency/Async.hpp"

using namespace crimild;

RenderSystem::RenderSystem( void )
{

}

RenderSystem::~RenderSystem( void )
{

}

bool RenderSystem::start( void )
{	
	if ( !System::start() ) {
		return false;
	}
    
    registerMessageHandler< messaging::RenderQueueAvailable >( [this]( messaging::RenderQueueAvailable const &message ) {
        _renderQueues = message.renderQueues;
    });
    
    registerMessageHandler< messaging::SceneChanged >( [this]( messaging::SceneChanged const &message ) {
        _renderQueues.clear();
    });
    
    registerMessageHandler< messaging::RenderNextFrame >( [this]( messaging::RenderNextFrame const &message ) {
        renderFrame();
    });

    registerMessageHandler< messaging::PresentNextFrame >( [this]( messaging::PresentNextFrame const &message ) {
        presentFrame();
    });
    
	return true;
}

void RenderSystem::renderFrame( void )
{
	CRIMILD_PROFILE( "Render System" );

    auto renderer = Simulation::getInstance()->getRenderer();
	if ( renderer == nullptr ) {
		return;
	}

	auto renderQueues = _renderQueues;
    
	{
		CRIMILD_PROFILE( "Begin Render" );
		renderer->beginRender();
		renderer->clearBuffers();
	}
    
    broadcastMessage( messaging::WillRenderScene {} );
    
	{
		CRIMILD_PROFILE( "Render Scene" );

		if ( !_renderQueues.empty() ) {
			RenderQueue *mainQueue = nullptr;
			_renderQueues.each( [ this, &mainQueue, renderer ]( SharedPointer< RenderQueue > &queue ) {
				// main camera is rendered last
				if ( queue->getCamera() != Camera::getMainCamera() ) {
					renderer->render( crimild::get_ptr( queue ), queue->getCamera()->getRenderPass() );
				}
				else {
					mainQueue = crimild::get_ptr( queue );
				}
			});

			if ( mainQueue != nullptr ) {
				renderer->render( mainQueue, mainQueue->getCamera()->getRenderPass() );
			}
	    }
	}

    {
        CRIMILD_PROFILE( "End Render" );
        renderer->endRender();
    }

    broadcastMessage( messaging::DidRenderScene {} );
}

void RenderSystem::presentFrame( void )
{
    auto renderer = Simulation::getInstance()->getRenderer();
    renderer->presentFrame();
}

void RenderSystem::stop( void )
{
	System::stop();

	_renderQueues.clear();

    unregisterMessageHandler< messaging::RenderQueueAvailable >();
    unregisterMessageHandler< messaging::SceneChanged >();
    unregisterMessageHandler< messaging::RenderNextFrame >();
}

