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

#include "ReflectiveShaderProgram.hpp"

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

ReflectiveShaderProgram::ReflectiveShaderProgram( void )
{
	createVertexShader();
	createFragmentShader();
}

ReflectiveShaderProgram::~ReflectiveShaderProgram( void )
{

}

void ReflectiveShaderProgram::createVertexShader( void )
{
	auto graph = Renderer::getInstance()->createShaderGraph();
    graph->addOutputNode< MeshVertexMaster >();

	buildVertexShader( graph );
}

void ReflectiveShaderProgram::createFragmentShader( void )
{
	auto graph = Renderer::getInstance()->createShaderGraph();

	auto I = normalize( neg( vec3_in( WORLD_EYE_VARIANT ) ) );
	auto N = normalize( vec3_in( WORLD_NORMAL_VARIANT ) );
	auto R = reflect( I, N );

	auto uv = vec2_in( MODEL_TEXTURE_COORDS_VARIANT );

	auto reflection = scalar_uniform( REFLECTION_UNIFORM, 0.0f );
	auto reflectionMap = texture2D_uniform( REFLECTION_MAP_UNIFORM, Texture::ONE );
	reflection = mult(
		reflection,
		vec_x( textureColor( reflectionMap, uv ) )
	);

	auto envMap = textureCube_uniform( ENVIRONMENT_MAP_UNIFORM, Texture::CUBE_ONE );
	auto envMapColor = vec3( textureColor( envMap, R ) );

	auto color = vec3( csl::colorUniform() );
	auto colorMap = csl::colorMapUniform();
	color = csl::mult(
		color,
		vec3( csl::textureColor( colorMap, uv ) )
	);

	color = add(
		mult( sub( scalar_one(), reflection ), color ),
		mult( reflection, envMapColor )
	);
	
	auto master = graph->addOutputNode< UnlitFragmentMaster >();
	master->setColor( color );

	buildFragmentShader( graph );
}

