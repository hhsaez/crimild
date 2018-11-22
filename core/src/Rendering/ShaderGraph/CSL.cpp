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

#include "CSL.hpp"

#include "Rendering/ShaderGraph/ShaderGraph.hpp"
#include "Rendering/ShaderGraph/Nodes/VertexShaderInputs.hpp"
#include "Rendering/ShaderGraph/Nodes/VertexShaderOutputs.hpp"
#include "Rendering/ShaderGraph/Nodes/VertexOutput.hpp"
#include "Rendering/ShaderGraph/Nodes/FragmentInput.hpp"
#include "Rendering/ShaderGraph/Nodes/FragmentColorOutput.hpp"
#include "Rendering/ShaderGraph/Nodes/Multiply.hpp"
#include "Rendering/ShaderGraph/Nodes/Dot.hpp"
#include "Rendering/ShaderGraph/Nodes/Max.hpp"
#include "Rendering/ShaderGraph/Nodes/Subtract.hpp"
#include "Rendering/ShaderGraph/Nodes/Negate.hpp"
#include "Rendering/ShaderGraph/Nodes/Normalize.hpp"
#include "Rendering/ShaderGraph/Nodes/Vector.hpp"
#include "Rendering/ShaderGraph/Nodes/Scalar.hpp"
#include "Rendering/ShaderGraph/Nodes/Pow.hpp"
#include "Rendering/ShaderGraph/Nodes/Copy.hpp"
#include "Rendering/ShaderGraph/Nodes/Convert.hpp"

using namespace crimild;
using namespace crimild::shadergraph;

Variable *csl::scalar( crimild::Real32 value, std::string name )
{
	return ShaderGraph::getCurrent()->addNode< ScalarConstant> (
		value,
		name
	)->getVariable();
}

Variable *csl::mat3( Variable *matrix )
{
	return ShaderGraph::getCurrent()->addNode< Convert >(
		matrix,
		Variable::Type::MATRIX_3
	)->getResult();
}

Variable *csl::mat4_uniform( std::string name )
{
	auto graph = ShaderGraph::getCurrent();

	auto ret = graph->getInput< Variable >( name );
	if ( ret == nullptr ) {
		ret = ShaderGraph::getCurrent()
			->addInputNode< Variable >(
				Variable::Storage::UNIFORM,
				Variable::Type::MATRIX_4,
				name
			);
		graph->addInputNode( ret );
	}
	return ret;
}

Variable *csl::vec3( Variable *input )
{
	return ShaderGraph::getCurrent()->addNode< Convert >(
		input,
		Variable::Type::VECTOR_3
	)->getResult();
}

Variable *csl::vec3_in( std::string name )
{
	auto graph = ShaderGraph::getCurrent();

	auto ret = graph->getInput< Variable >( name );
	if ( ret == nullptr ) {
		ret = ShaderGraph::getCurrent()->addInputNode< Variable >(
			Variable::Storage::INPUT,
			Variable::Type::VECTOR_3,
			name
		);
		graph->addInputNode( ret );
	}
	return ret;
}

Variable *csl::vec4( Variable *vector, Variable *scalar )
{
	auto xyzw = ShaderGraph::getCurrent()->addNode< VectorToScalars >( vector );

	return ShaderGraph::getCurrent()->addNode< ScalarsToVector >(
		xyzw->getX(),
		xyzw->getY(),
		vector->getType() == Variable::Type::VECTOR_3 ? xyzw->getZ() : scalar,
		scalar
	)->getVector();
}

Variable *csl::vec4( const Vector4f &value )
{
	return ShaderGraph::getCurrent()->addNode< VectorConstant >( value )->getVector();
}

Variable *csl::dot( Variable *a, Variable *b )
{
	return ShaderGraph::getCurrent()->addNode< Dot >( a, b )->getResult();
}

Variable *csl::max( Variable *a, Variable *b )
{
	return ShaderGraph::getCurrent()->addNode< Max >( a, b )->getResult();
}

Variable *csl::mult( Variable *a, Variable *b )
{
	return ShaderGraph::getCurrent()->addNode< Multiply >(
		a,
		b
	)->getResult();
}

Variable *csl::neg( Variable *input )
{
	return ShaderGraph::getCurrent()->addNode< Negate >( input )->getResult();
}

Variable *csl::normalize( Variable *input )
{
	return ShaderGraph::getCurrent()
		->addNode< Normalize >( input )
		->getResult();
}

Variable *csl::pow( Variable *base, Variable *exp )
{
	return ShaderGraph::getCurrent()->addNode< Pow >( base, exp )->getResult();
}

Variable *csl::sub( Variable *a, Variable *b )
{
	return ShaderGraph::getCurrent()->addNode< Subtract >( a, b )->getResult();
}

void csl::fragColor( Variable *color )
{
	ShaderGraph::getCurrent()->addOutputNode< FragmentColorOutput >( color );
}

void csl::vertexPosition( Variable *position )
{
	ShaderGraph::getCurrent()
		->addOutputNode< StandardVertexOutputs >(
			position
		);
}

void csl::vertexOutput( std::string name, Variable *value )
{
	ShaderGraph::getCurrent()
		->addOutputNode< VertexOutput >(
			name,
			value
		);
}

Variable *csl::worldPosition( void )
{
	auto graph = ShaderGraph::getCurrent();

	auto ret = graph->getInput< Variable >( "c_worldPosition" );
	if ( ret != nullptr ) {
		return ret;
	}

	auto aPosition = vec3_in( "aPosition" );
	auto uMMatrix = mat4_uniform( "uMMatrix" );
	ret = mult( uMMatrix, vec4( aPosition, scalar( 1.0 ) ) );
	ret->setName( "c_worldPosition" );
	graph->addInputNode( ret );
	return ret;
}

Variable *csl::viewPosition( void )
{
	auto graph = ShaderGraph::getCurrent();

	auto ret = graph->getInput< Variable >( "c_wiewPosition" );
	if ( ret != nullptr ) {
		return ret;
	}

	auto P = worldPosition();
	auto V = mat4_uniform( "uVMatrix" );
	ret = mult( V, P );
	ret->setName( "c_wiewPosition" );
	graph->addInputNode( ret );
	return ret;
}

Variable *csl::projectedPosition( void )
{
	auto graph = ShaderGraph::getCurrent();

	auto ret = graph->getInput< Variable >( "c_projPosition" );
	if ( ret != nullptr ) {
		return ret;
	}

	auto P = viewPosition();
	auto M = mat4_uniform( "uPMatrix" );
	ret = mult( M, P );
	ret->setName( "c_projPosition" );
	graph->addInputNode( ret );
	return ret;
}

Variable *csl::worldNormal( Variable *worldMatrix, Variable *normal )
{
	return normalize( mult( mat3( worldMatrix ), normal ) );
}

Variable *csl::worldNormal( void )
{
	auto aNormal = vec3_in( "aNormal" );
	auto uMMatrix = mat4_uniform( "uMMatrix" );
	
	return worldNormal( uMMatrix, aNormal );
}

Variable *csl::viewVector( Variable *viewPosition )
{
	return normalize( neg( vec3( viewPosition ) ) );
}

/*
Variable *csl::viewVector( void )
{
	auto graph = ShaderGraph::getCurrent();

	auto P = viewPosition();

}
*/

