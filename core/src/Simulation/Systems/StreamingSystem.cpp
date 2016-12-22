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

#include "StreamingSystem.hpp"

#include "Simulation/Simulation.hpp"
#include "Simulation/FileSystem.hpp"
 
#include "Concurrency/Async.hpp"

using namespace crimild;

StreamingSystem::StreamingSystem( void )
    : System( "Streaming System" )
{
	CRIMILD_BIND_MEMBER_MESSAGE_HANDLER( messaging::LoadScene, StreamingSystem, onLoadScene );
	CRIMILD_BIND_MEMBER_MESSAGE_HANDLER( messaging::ReloadScene, StreamingSystem, onReloadScene );
}

StreamingSystem::~StreamingSystem( void )
{

}

bool StreamingSystem::start( void )
{
    return System::start();
}

void StreamingSystem::stop( void )
{

}

void StreamingSystem::onLoadScene( messaging::LoadScene const &message )
{
    if ( message.sceneBuilder != nullptr ) {
        setSceneBuilder( message.sceneBuilder );
    }
    
    loadScene( message.filename, getSceneBuilder() );
}

void StreamingSystem::onReloadScene( messaging::ReloadScene const &message )
{
    auto sceneName = _lastSceneFileName;
    auto builder = getSceneBuilder();
    
    crimild::concurrency::sync_frame( [this, sceneName, builder] {
        Simulation::getInstance()->setScene( nullptr );
        loadScene( sceneName, builder );
    });
}

void StreamingSystem::loadScene( std::string filename, SceneBuilder *builder )
{
    if ( builder != nullptr ) {
        _sceneBuilder = crimild::retain( builder );
    }
    
    if ( _sceneBuilder == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Undefined scene builder" );
        return;
    }
    
    _lastSceneFileName = filename;
    
    // get an owner for the builder that we can use in the lambdas below
    auto sceneBuilder = crimild::retain( getSceneBuilder() );

	// Althougth the actual loading happens in background, we trigger it
	// in the main thread to ensure all systems are properly runnning. 
	// Then, once the scene is completely loaded, we set it as the current 
	// scene again in main thread to avoid changing scenes when rendering or updating
    crimild::concurrency::async_frame( [sceneBuilder, filename] {
        sceneBuilder->reset();
        auto scene = sceneBuilder->fromFile( FileSystem::getInstance().pathForResource( filename ) );
        sceneBuilder->reset();
        Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Scene loaded: ", filename );
        
        crimild::concurrency::sync_frame( [scene] {
            Simulation::getInstance()->setScene( scene );
        });
    });
}

