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

#include "LuaGroupBuilder.hpp"

using namespace crimild;
using namespace crimild::scripting;

SharedPointer< Group > LuaGroupBuilder::build( ScriptEvaluator &eval )
{
	/*
	SharedPointer< Group > group;
    
	std::string filename;
	if ( eval.getPropValue( NODE_FILENAME, filename ) && filename != "" ) {
#ifdef CRIMILD_SCRIPTING_LOG_VERBOSE
		Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Building node" );
#endif
		
		auto scene = AssetManager::getInstance()->get< Group >( filename );
		if ( scene == nullptr ) {
			SharedPointer< Group > tmp;
			if ( StringUtils::getFileExtension( filename ) == ".crimild" ) {
				FileStream is( FileSystem::getInstance().pathForResource( filename ), FileStream::OpenMode::READ );
				is.load();
				if ( is.getObjectCount() > 0 ) {
					tmp = is.getObjectAt< Group >( 0 );
				}
			}
			else if ( StringUtils::getFileExtension( filename ) == ".obj" ) {
				OBJLoader loader( FileSystem::getInstance().pathForResource( filename ) );
				tmp = loader.load();
			}
#ifdef CRIMILD_ENABLE_IMPORT
			else {
				SceneImporter importer;
				tmp = importer.import( FileSystem::getInstance().pathForResource( filename ) );
			}
#endif
			AssetManager::getInstance()->set( filename, tmp );
			scene = crimild::get_ptr( tmp );
		}
		
		// always copy assets from the cache
		ShallowCopy shallowCopy;
		scene->perform( shallowCopy );
		group = shallowCopy.getResult< Group >();
	}
	else {
#ifdef CRIMILD_SCRIPTING_LOG_VERBOSE
		Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Building 'group' node" );
#endif
		group = std::make_shared< Group >();
	}
    
	eval.foreach( GROUP_NODES, [&]( ScriptEvaluator &childEval, int ) {
		self->buildNode( childEval, crimild::get_ptr( group ) );
	});
    
	return group;
	*/

	// TODO
	return nullptr;
}

