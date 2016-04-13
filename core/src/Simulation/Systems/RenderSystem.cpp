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

#include "Rendering/RenderPass.hpp"
#include "Rendering/FrameBufferObject.hpp"

#include "Simulation/Simulation.hpp"

#include "Concurrency/Async.hpp"

using namespace crimild;

RenderSystem::RenderSystem( void )
	: System( "Render System" )
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
    
    auto self = this;
    
    registerMessageHandler< messaging::RenderQueueAvailable >( [&]( messaging::RenderQueueAvailable const &message ) {
        if ( _renderQueue != nullptr ) {
            if ( _renderQueue->getTimestamp() >= message.renderQueue->getTimestamp() ) {
                return;
            }
        }
        
        _renderQueue = message.renderQueue;
    });
    
    registerMessageHandler< messaging::SceneChanged >( [self]( messaging::SceneChanged const &message ) {
        self->_renderQueue = nullptr;
    });
    
    registerMessageHandler< messaging::RenderNextFrame >( [self]( messaging::RenderNextFrame const &message ) {
        self->renderFrame();
    });

    registerMessageHandler< messaging::PresentNextFrame >( [self]( messaging::PresentNextFrame const &message ) {
        self->presentFrame();
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
            renderer->render( crimild::get_ptr( renderQueue ), renderQueue->getCamera()->getRenderPass() );
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
}

void RenderSystem::presentFrame( void )
{
    auto renderer = Simulation::getInstance()->getRenderer();
    auto sBuffer = renderer->getFrameBuffer( RenderPass::S_BUFFER_NAME );
    if ( sBuffer != nullptr ) {
        auto color = sBuffer->getRenderTargets().get( RenderPass::S_BUFFER_COLOR_TARGET_NAME );
        auto program = renderer->getShaderProgram( crimild::Renderer::SHADER_PROGRAM_SCREEN_TEXTURE );
        if ( program != nullptr ) {
            renderer->bindProgram( program );
            renderer->bindTexture( program->getStandardLocation( crimild::ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), color->getTexture() );
            renderer->drawScreenPrimitive( program );
            renderer->unbindTexture( program->getStandardLocation( crimild::ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), color->getTexture() );
            renderer->unbindProgram( program );
        }
    }
}

void RenderSystem::stop( void )
{
	System::stop();
    
    unregisterMessageHandler< messaging::RenderQueueAvailable >();
    unregisterMessageHandler< messaging::SceneChanged >();
    unregisterMessageHandler< messaging::RenderNextFrame >();
}

