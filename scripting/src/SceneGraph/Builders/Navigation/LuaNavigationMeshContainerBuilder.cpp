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

#include "LuaNavigationMeshContainerBuilder.hpp"

#include "SceneGraph/LuaSceneBuilder.hpp"

#include <Navigation/NavigationMeshOBJ.hpp>
#include <Simulation/FileSystem.hpp>

using namespace crimild;
using namespace crimild::navigation;
using namespace crimild::scripting;

SharedPointer< NavigationMeshContainer > LuaNavigationMeshContainerBuilder::build( ScriptEvaluator &eval )
{
	std::string filename;
	if ( !eval.getPropValue( "navmesh", filename ) ) {
		Log::error( CRIMILD_CURRENT_CLASS_NAME, "No navmesh provided when building navigation container" );
		return nullptr;
	}

	auto navigationMesh = crimild::alloc< NavigationMeshOBJ >( FileSystem::getInstance().pathForResource( filename ) );

	return crimild::alloc< NavigationMeshContainer >( navigationMesh );
}

