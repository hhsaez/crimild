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

#ifndef CRIMILD_GLFW_SIMULATION_TASKS_LOAD_SCENE_
#define CRIMILD_GLFW_SIMULATION_TASKS_LOAD_SCENE_

#include <Crimild.hpp>
#include <Crimild_Scripting.hpp>

namespace crimild {

	class LoadSceneMessage : public DeferredMessage {
	public:
		LoadSceneMessage( std::string fileName ) : _fileName( fileName ) { }
		virtual ~LoadSceneMessage( void ) { }

		std::string getFileName( void ) const { return _fileName; }

	private:
		std::string _fileName;
	};

	class ReloadSceneMessage : public DeferredMessage {
	public:
		ReloadSceneMessage( void ) { }
		virtual ~ReloadSceneMessage( void ) { }
	};

	class SceneLoadedMessage : public DeferredMessage {
	public:
		SceneLoadedMessage( void ) { }
		virtual ~SceneLoadedMessage( void ) { }
	};

	class LoadSceneTask : 
		public Task,
		public MessageHandler< LoadSceneMessage >,
		public MessageHandler< ReloadSceneMessage > {
	public:
		LoadSceneTask( int priority, std::string sceneFileName );
		virtual ~LoadSceneTask( void );

		scripting::SceneBuilder &getBuilder( void ) { return _builder; }

		virtual void start( void ) override;
		virtual void update( void ) override;
		virtual void stop( void ) override;

		virtual void handleMessage( LoadSceneMessage *message ) override;
		virtual void handleMessage( ReloadSceneMessage *message ) override;

	private:
		void load( void );

	private:
		std::string _sceneFileName;
		scripting::SceneBuilder _builder;
	};

}

#endif

