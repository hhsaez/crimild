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

#include "VertexShaderInputs.hpp"
#include "Multiply.hpp"
#include "Vector.hpp"
#include "Scalar.hpp"
#include "Convert.hpp"
#include "Normalize.hpp"
#include "Negate.hpp"

#include "Rendering/ShaderGraph/ShaderGraph.hpp"
#include "Rendering/ShaderGraph/Variable.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/ShaderLocation.hpp"

using namespace crimild;
using namespace crimild::shadergraph;

VertexShaderInputs::VertexShaderInputs( ShaderGraph *graph )
{
	_positionAttribute = graph->addInputNode< Variable >(
		Variable::Storage::INPUT,
		Variable::Type::VECTOR_3,
		"aPosition" );
	_normalAttribute = graph->addInputNode< Variable >(
		Variable::Storage::INPUT,
		Variable::Type::VECTOR_3,
		"aNormal" );

	_modelMatrixUniform = graph->addInputNode< Variable >(
		Variable::Storage::UNIFORM,
		Variable::Type::MATRIX_4,
		"uMMatrix" );
	_viewMatrixUniform = graph->addInputNode< Variable >(
		Variable::Storage::UNIFORM,
		Variable::Type::MATRIX_4,
		"uVMatrix" );
	_projectionMatrixUniform = graph->addInputNode< Variable >(
		Variable::Storage::UNIFORM,
		Variable::Type::MATRIX_4,
		"uPMatrix" );

	auto kONE = graph->addNode< ScalarConstant >( 1.0f, "kONE" );
	auto posXYZ = graph->addNode< VectorToScalars >( _positionAttribute );

	_modelPosition = graph->addNode< ScalarsToVector >(
		posXYZ->getX(),
		posXYZ->getY(),
		posXYZ->getZ(),
		kONE->getVariable()
	)->getVector()->setUniqueName( "modelPosition" );

	_worldPosition = graph->addNode< Multiply >(
		_modelMatrixUniform,
		_modelPosition
	)->getResult()->setUniqueName( "worldPosition" );

	_viewPosition = graph->addNode< Multiply >(
		_viewMatrixUniform,
		_worldPosition
	)->getResult()->setUniqueName( "viewPosition" );

	_projectedPosition = graph->addNode< Multiply >(
		_projectionMatrixUniform,
		_viewPosition
	)->getResult()->setUniqueName( "projectedPosition" );
}

VertexShaderInputs::~VertexShaderInputs( void )
{
	
}

void VertexShaderInputs::setup( ShaderGraph *graph )
{
	
	graph->write(
		this,
		{
			_positionAttribute,
			_normalAttribute,
			_uvAttribute,
			_colorAttribute,
			_modelMatrixUniform,
			_viewMatrixUniform,
			_projectionMatrixUniform,
			_worldPosition,
			_viewPosition,
			_projectedPosition,
		}
	);
}

/*

VertexShaderInput::VertexShaderInput( void )
{
	_position = addOutputOutlet( "aPosition", Outlet::Type::VECTOR_3 );
	_normal = addOutputOutlet( "aNormal", Outlet::Type::VECTOR_3 );
	_uv = addOutputOutlet( "aUV", Outlet::Type::VECTOR_2 );
	_color = addOutputOutlet( "aColor", Outlet::Type::VECTOR_4 );

	_mMatrix = addOutputOutlet( "mMatrix", Outlet::Type::MATRIX_4 );
	_vMatrix = addOutputOutlet( "vMatrix", Outlet::Type::MATRIX_4 );
	_pMatrix = addOutputOutlet( "pMatrix", Outlet::Type::MATRIX_4 );
	_mvMatrix = addOutputOutlet( "mvMatrix", Outlet::Type::MATRIX_4 );
	_mvpMatrix = addOutputOutlet( "mvpMatrix", Outlet::Type::MATRIX_4 );
}

VertexShaderInput::~VertexShaderInput( void )
{

}

void VertexShaderInput::prepare( ShaderGraph *graph, ShaderProgram *program )
{
	Node::prepare( graph, program );

	if ( graph->isConnected( getPosition() ) ) {
		program->registerStandardLocation(
			ShaderLocation::Type::ATTRIBUTE,
			ShaderProgram::StandardLocation::POSITION_ATTRIBUTE,
			getPosition()->getName() );
	}
	
	if ( graph->isConnected( getNormal() ) ) {
		program->registerStandardLocation(
			ShaderLocation::Type::ATTRIBUTE,
			ShaderProgram::StandardLocation::NORMAL_ATTRIBUTE,
			getNormal()->getName() );
	}
	
	if ( graph->isConnected( getUV() ) ) {
		program->registerStandardLocation(
			ShaderLocation::Type::ATTRIBUTE,
			ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE,
			getUV()->getName() );
	}
	
	if ( graph->isConnected( getColor() ) ) {
		program->registerStandardLocation(
			ShaderLocation::Type::ATTRIBUTE,
			ShaderProgram::StandardLocation::COLOR_ATTRIBUTE,
			getColor()->getName() );
	}
	
	if ( graph->isConnected( getMMatrix() ) ) {
		program->registerStandardLocation(
			ShaderLocation::Type::UNIFORM,
			ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM,
			getMMatrix()->getName() );
	}
	
	if ( graph->isConnected( getVMatrix() ) ) {
		program->registerStandardLocation(
			ShaderLocation::Type::UNIFORM,
			ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM,
			getVMatrix()->getName() );
	}
	
	if ( graph->isConnected( getPMatrix() ) ) {
		program->registerStandardLocation(
			ShaderLocation::Type::UNIFORM,
			ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM,
			getPMatrix()->getName() );
	}
}

*/

