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

#include "VertexShaderInput.hpp"

using namespace crimild;
using namespace crimild::shadergraph;
using namespace crimild::shadergraph::nodes;

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

