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

#include "PointSpriteShaderProgram.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/ShaderGraph/ShaderGraph.hpp"
#include "Rendering/ShaderGraph/CSL.hpp"
#include "Rendering/ShaderGraph/Nodes/UnlitFragmentMaster.hpp"

using namespace crimild;
using namespace crimild::shadergraph;

PointSpriteShaderProgram::PointSpriteShaderProgram( void )
{
	createVertexShader();
	createFragmentShader();
}

PointSpriteShaderProgram::~PointSpriteShaderProgram( void )
{

}

void PointSpriteShaderProgram::createVertexShader( void )
{
	auto graph = Renderer::getInstance()->createShaderGraph();

	auto P = csl::clipPosition();
	auto C = csl::modelColor();
	auto UV = csl::modelTextureCoords();

	csl::vertexOutput( "vParticleColor", C );
	csl::vertexPosition( P );
	csl::vertexPointSize( csl::vec_x( UV ) );

	buildVertexShader( graph );
}

void PointSpriteShaderProgram::createFragmentShader( void )
{
	auto graph = Renderer::getInstance()->createShaderGraph();

	auto particleColor = csl::vec4_in( "vParticleColor" );
	auto UV = csl::pointCoord();
    auto color = csl::colorUniform();
    auto colorMap = csl::colorMapUniform();
    color = csl::mult(
        color,
        csl::textureColor( colorMap, UV ),
        particleColor
    );

    auto master = graph->addOutputNode< UnlitFragmentMaster >();
    master->setColor( csl::vec3( color ) );
    master->setAlpha( csl::vec_w( color ) );
	master->setTextureCoords( UV );

	buildFragmentShader( graph );
}

