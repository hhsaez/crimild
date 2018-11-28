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
#include "Rendering/ShaderGraph/Nodes/Dot.hpp"
#include "Rendering/ShaderGraph/Nodes/Max.hpp"
#include "Rendering/ShaderGraph/Nodes/Add.hpp"
#include "Rendering/ShaderGraph/Nodes/Subtract.hpp"
#include "Rendering/ShaderGraph/Nodes/Multiply.hpp"
#include "Rendering/ShaderGraph/Nodes/Divide.hpp"
#include "Rendering/ShaderGraph/Nodes/Negate.hpp"
#include "Rendering/ShaderGraph/Nodes/Normalize.hpp"
#include "Rendering/ShaderGraph/Nodes/Vector.hpp"
#include "Rendering/ShaderGraph/Nodes/Scalar.hpp"
#include "Rendering/ShaderGraph/Nodes/Pow.hpp"
#include "Rendering/ShaderGraph/Nodes/Copy.hpp"
#include "Rendering/ShaderGraph/Nodes/Convert.hpp"
#include "Rendering/ShaderGraph/Nodes/TextureColor.hpp"

#include <sstream>

using namespace crimild;
using namespace crimild::shadergraph;

Variable *csl::scalar( crimild::Real32 value, std::string name )
{
	return ShaderGraph::getCurrent()->addNode< ScalarConstant> (
		value,
		name
	)->getVariable();
}

Variable *csl::scalar_uniform( std::string name )
{
	auto graph = ShaderGraph::getCurrent();

	auto ret = graph->getInput< Variable >( name );
	if ( ret == nullptr ) {
		ret = graph->addInputNode< Variable >(
			Variable::Storage::UNIFORM,
			Variable::Type::SCALAR,
			name
		);
	}
	return ret;
}

Variable *csl::scalar_constant( crimild::Real32 value )
{
	auto graph = ShaderGraph::getCurrent();

	std::stringstream ss;
	ss << value;
	std::string name = ss.str();

	auto ret = graph->getInput< Variable >( name );
	if ( ret == nullptr ) {
		ret = graph->addNode< Variable >(
			Variable::Storage::CONSTANT,
			Variable::Type::SCALAR,
			name
		);
	}
	return ret;
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
		ret = graph->addInputNode< Variable >(
			Variable::Storage::UNIFORM,
			Variable::Type::MATRIX_4,
			name
		);
	}
	return ret;
}

Variable *csl::vec2_in( std::string name )
{
	auto graph = ShaderGraph::getCurrent();

	auto ret = graph->getInput< Variable >( name );
	if ( ret == nullptr ) {
		ret = graph->addInputNode< Variable >(
			Variable::Storage::INPUT,
			Variable::Type::VECTOR_2,
			name
		);
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
		ret = graph->addInputNode< Variable >(
			Variable::Storage::INPUT,
			Variable::Type::VECTOR_3,
			name
		);
	}
	return ret;
}

Variable *csl::vec4( Variable *scalar )
{
	return ShaderGraph::getCurrent()->addNode< ScalarsToVector >(
		scalar,
		scalar,
		scalar,
		scalar
	)->getVector();	
}

Variable *csl::vec4( Variable *x, Variable *y, Variable *z, Variable *w )
{
	return ShaderGraph::getCurrent()->addNode< ScalarsToVector >(
		x,
		y,
		z,
	    w
	)->getVector();	
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

Variable *csl::vec_x( Variable *vector )
{
	return ShaderGraph::getCurrent()->addNode< VectorToScalars >( vector )->getX();
}

Variable *csl::vec_y( Variable *vector )
{
	return ShaderGraph::getCurrent()->addNode< VectorToScalars >( vector )->getY();
}

Variable *csl::vec_z( Variable *vector )
{
	return ShaderGraph::getCurrent()->addNode< VectorToScalars >( vector )->getZ();
}

Variable *csl::vec_w( Variable *vector )
{
	return ShaderGraph::getCurrent()->addNode< VectorToScalars >( vector )->getW();
}

Variable *csl::red( Variable *color )
{
	return vec_x( color );
}

Variable *csl::green( Variable *color )
{
	return vec_y( color );
}

Variable *csl::blue( Variable *color )
{
	return vec_z( color );
}

Variable *csl::alpha( Variable *color )
{
	return vec_w( color );
}

Variable *csl::dot( Variable *a, Variable *b )
{
	return ShaderGraph::getCurrent()->addNode< Dot >( a, b )->getResult();
}

Variable *csl::add( Variable *a, Variable *b )
{
	return ShaderGraph::getCurrent()->addNode< Add >( a, b )->getResult();
}

Variable *csl::sub( Variable *a, Variable *b )
{
	return ShaderGraph::getCurrent()->addNode< Subtract >( a, b )->getResult();
}

Variable *csl::mult( Variable *a, Variable *b )
{
	return ShaderGraph::getCurrent()->addNode< Multiply >( a, b )->getResult();
}

Variable *csl::mult( containers::Array< Variable * > const &inputs )
{
	return ShaderGraph::getCurrent()->addNode< Multiply >( inputs )->getResult();
}

Variable *csl::div( Variable *a, Variable *b )
{
	return ShaderGraph::getCurrent()->addNode< Divide >( a, b )->getResult();
}

Variable *csl::max( Variable *a, Variable *b )
{
	return ShaderGraph::getCurrent()->addNode< Max >( a, b )->getResult();
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

Variable *csl::modelPosition( void )
{
	return vec3_in( "aPosition" );
}

Variable *csl::worldPosition( void )
{
	auto graph = ShaderGraph::getCurrent();

	auto ret = graph->getInput< Variable >( "c_worldPosition" );
	if ( ret != nullptr ) {
		return ret;
	}

	auto aPosition = modelPosition();
	auto uMMatrix = mat4_uniform( "uMMatrix" );
	ret = mult( uMMatrix, vec4( aPosition, scalar_constant( 1.0 ) ) );
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

Variable *csl::screenPosition( void )
{
	// TODO
	auto P = modelPosition();
	auto xyzw = ShaderGraph::getCurrent()->addNode< VectorToScalars >( P );

	return ShaderGraph::getCurrent()->addNode< ScalarsToVector >(
		xyzw->getX(),
		xyzw->getY(),
		scalar_constant( 0 ),
		scalar_constant( 1 )
	)->getVector();
}

Variable *csl::modelTextureCoords( void )
{
	return vec2_in( "aTextureCoords" );
}

Variable *csl::modelNormal( void )
{
	return vec3_in( "aNormal" );
}

Variable *csl::worldNormal( Variable *worldMatrix, Variable *normal )
{
	return normalize( mult( mat3( worldMatrix ), normal ) );
}

Variable *csl::worldNormal( void )
{
	auto graph = ShaderGraph::getCurrent();

	auto ret = graph->getInput< Variable >( "c_worldNormal" );
	if ( ret == nullptr ) {
		auto aNormal = modelNormal();
		auto uMMatrix = mat4_uniform( "uMMatrix" );
	
		ret = worldNormal( uMMatrix, aNormal );
		ret->setName( "c_worldNormal" );
		graph->addInputNode( ret );
	}

	return ret;
}

Variable *csl::viewNormal( void )
{
	auto graph = ShaderGraph::getCurrent();

	auto ret = graph->getInput< Variable >( "c_viewNormal" );
	if ( ret == nullptr ) {
		auto N = worldNormal();
		auto V = mat4_uniform( "uVMatrix" );
	
		ret = normalize( mult( mat3( V ), N ) );
		ret->setName( "c_viewNormal" );
		graph->addInputNode( ret );
	}

	return ret;
}

Variable *csl::viewVector( Variable *viewPosition )
{
	return normalize( neg( vec3( viewPosition ) ) );
}


Variable *csl::linearizeDepth( Variable *z, Variable *N, Variable *F )
{
	// z = 2.0 * z - 1.0;
	// z = ( 2.0 * N * F ) / ( F + N - z * ( F - N ) );
	
	z = sub( mult( scalar_constant( 2 ), z ), scalar_constant( 1 ) );
	return div(
		mult( scalar_constant( 2 ), N, F ),
		sub( add( F, N ), mult( z, sub( F, N ) ) )
	);
}

Variable *csl::texture2D_uniform( std::string name )
{
	auto graph = ShaderGraph::getCurrent();

	auto ret = graph->getInput< Variable >( name );
	if ( ret == nullptr ) {
		ret = graph->addInputNode< Variable >(
			Variable::Storage::UNIFORM,
			Variable::Type::SAMPLER_2D,
			name
		);
	}
	return ret;
}

Variable *csl::textureColor( Variable *texture, Variable *uvs )
{
	auto graph = ShaderGraph::getCurrent();
	return graph->addNode< TextureColor >( texture, uvs )->getColor();
}

