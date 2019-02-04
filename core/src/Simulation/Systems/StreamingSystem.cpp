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
#include "Visitors/UpdateWorldState.hpp"
#include "Visitors/UpdateRenderState.hpp"
#include "Visitors/StartComponents.hpp"

using namespace crimild;

StreamingSystem::StreamingSystem( void )
{
	CRIMILD_BIND_MEMBER_MESSAGE_HANDLER( messaging::LoadScene, StreamingSystem, onLoadScene );
	CRIMILD_BIND_MEMBER_MESSAGE_HANDLER( messaging::AppendScene, StreamingSystem, onAppendScene );
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
    auto fileName = message.fileName;
    auto fileType = StringUtils::getFileExtension( fileName );

    if ( !_builders.contains( fileType ) ) {
        std::string message = "Cannot find builder for file" + fileName;
        crimild::Log::error( CRIMILD_CURRENT_CLASS_NAME, message );
        broadcastMessage( messaging::SceneLoadFailed { fileName, message } );
        return;
    }

    _lastSceneFileName = fileName;
    auto builder = _builders[ fileType ];

    crimild::concurrency::async_frame( [ builder, fileName ] {
        AssetManager::getInstance()->clear();

        auto scene = builder( fileName );
        if ( scene == nullptr ) {
            std::string message = "Cannot load scene from file: " + fileName;
            crimild::Log::error( CRIMILD_CURRENT_CLASS_NAME, message );
            MessageQueue::getInstance()->broadcastMessage( messaging::SceneLoadFailed { fileName, message } );
            return;
        }
        
        crimild::concurrency::sync_frame( [ scene ] {
            Simulation::getInstance()->setScene( scene );
        });
    });
}

void StreamingSystem::onAppendScene( messaging::AppendScene const &message )
{
    auto fileName = message.fileName;
    auto fileType = StringUtils::getFileExtension( fileName );
    auto onLoadSceneCallback = message.onLoadSceneCallback;

    if ( !_builders.contains( fileType ) ) {
        std::string message = "Cannot find builder for file" + fileName;
        crimild::Log::error( CRIMILD_CURRENT_CLASS_NAME, message );
        broadcastMessage( messaging::SceneLoadFailed { fileName, message } );
        return;
    }

    auto builder = _builders[ fileType ];

    crimild::concurrency::async_frame( [ builder, fileName, onLoadSceneCallback ] {
        auto scene = builder( fileName );
        if ( scene == nullptr ) {
            std::string message = "Cannot load scene from file: " + fileName;
            crimild::Log::error( CRIMILD_CURRENT_CLASS_NAME, message );
            MessageQueue::getInstance()->broadcastMessage( messaging::SceneLoadFailed { fileName, message } );
            return;
        }
        
        crimild::concurrency::sync_frame( [ scene, onLoadSceneCallback ] {
            auto parentNode = static_cast< Group * >( Simulation::getInstance()->getScene() );

			parentNode->attachNode( scene );

			scene->perform( UpdateWorldState() );
			scene->perform( UpdateRenderState() );
			scene->perform( StartComponents() );
			// update state one more time after starting components
			scene->perform( UpdateWorldState() );
			scene->perform( UpdateRenderState() );

            if ( onLoadSceneCallback != nullptr ) {
                onLoadSceneCallback( crimild::get_ptr( scene ) );
            }
        });
    });
}

void StreamingSystem::onReloadScene( messaging::ReloadScene const &message )
{
    auto fileName = _lastSceneFileName;
    auto fileType = StringUtils::getFileExtension( fileName );

    if ( !_builders.contains( fileType ) ) {
        std::string message = "Cannot find builder for file" + fileName;
        crimild::Log::error( CRIMILD_CURRENT_CLASS_NAME, message );
        broadcastMessage( messaging::SceneLoadFailed { fileName, message } );
        return;
    }

    auto builder = _builders[ fileType ];

    crimild::concurrency::sync_frame( [ builder, fileName ] {
        Simulation::getInstance()->setScene( nullptr );

        crimild::concurrency::async_frame( [ builder, fileName ] {
            AssetManager::getInstance()->clear();

            auto scene = builder( fileName );
            if ( scene == nullptr ) {
                std::string message = "Cannot load scene from file: " + fileName;
                crimild::Log::error( CRIMILD_CURRENT_CLASS_NAME, message );
                MessageQueue::getInstance()->broadcastMessage( messaging::SceneLoadFailed { fileName, message } );
                return;
            }
            
            crimild::concurrency::sync_frame( [ scene ] {
                Simulation::getInstance()->setScene( scene );
            });
        });        
    });
}

