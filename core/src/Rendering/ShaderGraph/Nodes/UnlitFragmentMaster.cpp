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

#include "UnlitFragmentMaster.hpp"

#include "Rendering/ShaderGraph/ShaderGraph.hpp"
#include "Rendering/ShaderGraph/CSL.hpp"
#include "Rendering/ShaderGraph/Constants.hpp"

using namespace crimild;
using namespace crimild::shadergraph;

UnlitFragmentMaster::UnlitFragmentMaster( ShaderGraph *graph )
{

}

UnlitFragmentMaster::~UnlitFragmentMaster( void )
{
	
}

void UnlitFragmentMaster::setup( ShaderGraph *graph )
{
	if ( _textureCoords == nullptr ) _textureCoords = csl::vec2_in( variants::MODEL_TEXTURE_COORDS_VARIANT );
	
	auto color = csl::colorUniform();
	auto colorMap = csl::colorMapUniform();
	color = csl::mult(
		color,
		csl::textureColor( colorMap, _textureCoords )
	);

	if ( _color == nullptr ) _color = csl::vec3( color );
	if ( _alpha == nullptr ) _alpha = csl::vec_w( color );
	if ( _alphaClipThreshold == nullptr ) _alphaClipThreshold = csl::scalar( 0.0f );

	csl::alphaClip( _alpha, _alphaClipThreshold );
	csl::fragColor( csl::vec4( _color, _alpha ) );
	
	graph->read(
		this,
		{
			_textureCoords,
			_color,
			_alpha,
			_alphaClipThreshold,
		}
	);
}

