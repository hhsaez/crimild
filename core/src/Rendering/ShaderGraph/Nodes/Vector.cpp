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

#include "Vector.hpp"
#include "Rendering/ShaderGraph/ShaderGraph.hpp"
#include "Rendering/ShaderGraph/Variable.hpp"

using namespace crimild;
using namespace crimild::shadergraph;

VectorToScalars::VectorToScalars( ShaderGraph *graph, Variable *vector )
{
	_vector = vector;
	_x = graph->addNode< Variable >( Variable::Type::SCALAR );
	_y = graph->addNode< Variable >( Variable::Type::SCALAR );
	_z = graph->addNode< Variable >( Variable::Type::SCALAR );
	_w = graph->addNode< Variable >( Variable::Type::SCALAR );
}

VectorToScalars::~VectorToScalars( void )
{

}

void VectorToScalars::setup( ShaderGraph *graph )
{
	graph->read( this, { _vector } );
	graph->write( this, { _x, _y, _z, _w } );
}

ScalarsToVector::ScalarsToVector( ShaderGraph *graph, Variable *x, Variable *y )
{
	_vector = graph->addNode< Variable >( Variable::Type::VECTOR_2 );
	_x = x;
	_y = y;
}

ScalarsToVector::ScalarsToVector( ShaderGraph *graph, Variable *x, Variable *y, Variable *z )
{
	_vector = graph->addNode< Variable >( Variable::Type::VECTOR_3 );
	_x = x;
	_y = y;
	_z = z;
}

ScalarsToVector::ScalarsToVector( ShaderGraph *graph, Variable *x, Variable *y, Variable *z, Variable *w )
{
	_vector = graph->addNode< Variable >( Variable::Type::VECTOR_4 );
	_x = x;
	_y = y;
	_z = z;
	_w = w;
}

ScalarsToVector::~ScalarsToVector( void )
{

}

void ScalarsToVector::setup( ShaderGraph *graph )
{
	graph->read( this, { _x, _y, _z, _w } );
	graph->write( this, { _vector } );
}

VectorConstant::VectorConstant( ShaderGraph *graph, const Vector2f &v )
	: _value( v.x(), v.y(), 0, 0 )
{
	_vector = graph->addNode< Variable >( Variable::Type::VECTOR_2 );
}

VectorConstant::VectorConstant( ShaderGraph *graph, const Vector3f &v )
	: _value( v.x(), v.y(), v.z(), 0 )
{
	_vector = graph->addNode< Variable >( Variable::Type::VECTOR_3 );
}

VectorConstant::VectorConstant( ShaderGraph *graph, const Vector4f &v )
	: _value( v )
{
	_vector = graph->addNode< Variable >( Variable::Type::VECTOR_4 );
}

VectorConstant::~VectorConstant( void )
{

}

void VectorConstant::setup( ShaderGraph *graph )
{
	graph->write( this, { _vector } );
}

