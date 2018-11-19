/*
 * Copyright (c) 2013-2018, Hernan Saez
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

#include "Multiply.hpp"

#include "Rendering/ShaderGraph/ShaderGraph.hpp"
#include "Rendering/ShaderGraph/ShaderGraphVariable.hpp"

using namespace crimild;
using namespace crimild::shadergraph;

Multiply::Multiply( ShaderGraph *graph, ShaderGraphVariable *a, ShaderGraphVariable *b )
{
	_a = a;
	_b = b;

	ShaderGraphVariable::Type retType = _b->getType();
	switch ( _a->getType() ) {
		case ShaderGraphVariable::Type::VECTOR_2:
		case ShaderGraphVariable::Type::VECTOR_3:
		case ShaderGraphVariable::Type::VECTOR_4:
			retType = _a->getType();
			break;

		default:
			break;
	}
	
	_result = graph->addNode< ShaderGraphVariable >( retType );
}

Multiply::~Multiply( void )
{

}

void Multiply::setup( ShaderGraph *graph )	
{
	graph->read( this, { _a, _b } );
	graph->write( this, { _result } );
}

