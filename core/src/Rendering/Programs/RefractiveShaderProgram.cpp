/*
 * Copyright (c) 2002-present, H. Hernan Saez
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

#include "RefractiveShaderProgram.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/ShaderGraph/ShaderGraph.hpp"
#include "Rendering/ShaderGraph/CSL.hpp"
#include "Rendering/ShaderGraph/Constants.hpp"
#include "Rendering/ShaderGraph/Nodes/MeshVertexMaster.hpp"
#include "Rendering/ShaderGraph/Nodes/UnlitFragmentMaster.hpp"

using namespace crimild;
using namespace crimild::shadergraph;
using namespace crimild::shadergraph::locations;
using namespace crimild::shadergraph::variants;
using namespace crimild::shadergraph::csl;

RefractiveShaderProgram::RefractiveShaderProgram( void )
{
	createVertexShader();
	createFragmentShader();
}

RefractiveShaderProgram::~RefractiveShaderProgram( void )
{

}

void RefractiveShaderProgram::createVertexShader( void )
{
	auto graph = Renderer::getInstance()->createShaderGraph();

	auto master = graph->addOutputNode< MeshVertexMaster >();

	buildVertexShader( graph );
}

void RefractiveShaderProgram::createFragmentShader( void )
{
	auto graph = Renderer::getInstance()->createShaderGraph();

	auto uv = vec2_in( MODEL_TEXTURE_COORDS_VARIANT );

	auto I = normalize( neg( vec3_in( WORLD_EYE_VARIANT ) ) );
	auto N = normalize( vec3_in( WORLD_NORMAL_VARIANT ) );

	auto refraction = scalar_uniform( REFRACTION_UNIFORM, 1.0f );
	auto refractionMap = texture2D_uniform( REFRACTION_MAP_UNIFORM, Texture::ONE );
	refraction = mult(
		refraction,
		vec_x( textureColor( refractionMap, uv ) )
	);

	auto R = refract( I, N, refraction );

	auto envMap = textureCube_uniform( ENVIRONMENT_MAP_UNIFORM, Texture::CUBE_ONE );
	auto envMapColor = vec3( textureColor( envMap, R ) );

	auto color = envMapColor;
	
	auto master = graph->addOutputNode< UnlitFragmentMaster >();
	master->setColor( color );

	buildFragmentShader( graph );
}

