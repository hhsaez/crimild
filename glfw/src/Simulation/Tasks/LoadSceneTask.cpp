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

#include "LoadSceneTask.hpp"

#include <Crimild_Physics.hpp>

using namespace crimild;
using namespace crimild::physics;
using namespace crimild::scripting;

LoadSceneTask::LoadSceneTask( int priority, std::string sceneFileName, SceneBuilderPtr const &builder )
	: Task( priority ),
	  _sceneFileName( sceneFileName ),
      _builder( builder != nullptr ? builder : crimild::alloc< SceneBuilder >() )
{
	getBuilder()->registerComponentBuilder< physics::RigidBodyComponent >( []( ScriptContext::Iterable &it ) {
        auto rigidBody = crimild::alloc< RigidBodyComponent >();

		if ( it.test( "mass" ) ) rigidBody->setMass( it.eval< float >( "mass" ) );
		if ( it.test( "convex" ) ) rigidBody->setConvex( it.eval< bool >( "convex" ) );
		if ( it.test( "kinematic" ) ) rigidBody->setKinematic( it.eval< bool >( "kinematic" ) );
		if ( it.test( "linearVelocity" ) ) rigidBody->setLinearVelocity( it.eval< Vector3f >( "linearVelocity" ) );
		if ( it.test( "linearFactor" ) ) rigidBody->setLinearFactor( it.eval< Vector3f >( "linearFactor" ) );
		if ( it.test( "constraintVelocity" ) ) rigidBody->setConstraintVelocity( it.eval< bool >( "constraintVelocity" ) );

		return rigidBody;
	});

	CRIMILD_BIND_MEMBER_MESSAGE_HANDLER( Messages::LoadScene, LoadSceneTask, loadScene );
	CRIMILD_BIND_MEMBER_MESSAGE_HANDLER( Messages::ReloadScene, LoadSceneTask, reloadScene );
}

LoadSceneTask::~LoadSceneTask( void )
{

}

void LoadSceneTask::start( void )
{
//	load();
}

void LoadSceneTask::update( void )
{
    Simulation::getInstance()->setScene( nullptr );
    AssetManager::getInstance()->clear();
    MessageQueue::getInstance()->clear();
    
    getBuilder()->reset();
    auto scene = getBuilder()->fromFile( FileSystem::getInstance().pathForResource( _sceneFileName ) );
    Simulation::getInstance()->setScene( scene );
    
    broadcastMessage( Messages::SceneLoaded() );
    
    getRunLoop()->suspendTask( getShared< LoadSceneTask >() );
}

void LoadSceneTask::stop( void )
{

}

void LoadSceneTask::load( void )
{
	// clear current scene
}

void LoadSceneTask::loadScene( Messages::LoadScene const &message )
{
	_sceneFileName = message.fileName;
    getRunLoop()->resumeTask( getShared< LoadSceneTask >() );
}

void LoadSceneTask::reloadScene( Messages::ReloadScene const &message )
{
	load();
    getRunLoop()->resumeTask( getShared< LoadSceneTask >() );
}

