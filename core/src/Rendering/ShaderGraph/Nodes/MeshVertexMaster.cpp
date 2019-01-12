/*
 * Copyright (c) 2002-preset, H. Hernan Saez
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

#include "MeshVertexMaster.hpp"

#include "Rendering/ShaderGraph/ShaderGraph.hpp"
#include "Rendering/ShaderGraph/Variable.hpp"
#include "Rendering/ShaderGraph/CSL.hpp"
#include "Rendering/ShaderGraph/Constants.hpp"
#include "Rendering/ShaderUniformImpl.hpp"

using namespace crimild;
using namespace crimild::shadergraph;

MeshVertexMaster::MeshVertexMaster( ShaderGraph *graph )
{

}

MeshVertexMaster::~MeshVertexMaster( void )
{
	
}

void MeshVertexMaster::setup( ShaderGraph *graph )
{
	if ( _textureCoords == nullptr ) _textureCoords = csl::modelTextureCoords();
	if ( _worldPosition == nullptr ) _worldPosition = csl::vec3( csl::worldPosition() );
	if ( _clipPosition == nullptr ) _clipPosition = csl::clipPosition();
	if ( _worldNormal == nullptr ) _worldNormal = csl::worldNormal();
	if ( _worldEye == nullptr ) _worldEye = csl::worldEyeVector();

	csl::vertexOutput( variants::MODEL_TEXTURE_COORDS_VARIANT, _textureCoords );
	csl::vertexOutput( variants::WORLD_POSITION_VARIANT, _worldPosition );
	csl::vertexOutput( variants::WORLD_NORMAL_VARIANT, _worldNormal );
	csl::vertexOutput( variants::WORLD_EYE_VARIANT, _worldEye );

	csl::vertexPosition( _clipPosition );
	
	graph->read(
		this,
		{
			_textureCoords,
			_worldPosition,
			_clipPosition,
			_worldNormal,
			_worldEye,
		}
	);
}

