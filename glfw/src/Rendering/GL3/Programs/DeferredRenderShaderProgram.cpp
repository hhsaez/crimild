/*
 * Copyright (c) 2013, Hernan Saez
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

#include "DeferredRenderShaderProgram.hpp"
#include "Rendering/GL3/Utils.hpp"

using namespace crimild;
using namespace crimild::gl3;

const char *deferred_vs = { CRIMILD_TO_STRING(
    in vec3 aPosition;
    in vec3 aNormal;

    uniform mat4 uPMatrix;
    uniform mat4 uVMatrix;
    uniform mat4 uMMatrix;
    uniform mat4 uNMatrix;
                                              
    out vec4 vWorldVertex;
    out vec3 vWorldNormal;
    out float vLinearDepth;

    void main ()
    {
        // replace this with an actual uniform
        float uLinearDepth = 1000.0;
        
        vWorldVertex = uMMatrix * vec4( aPosition, 1.0 );
        vec4 viewVertex = uVMatrix * vWorldVertex;
        gl_Position = uPMatrix * viewVertex;
     
        vLinearDepth = length( viewVertex ) / uLinearDepth;
        vWorldNormal = mat3( uVMatrix * uNMatrix ) * aNormal;
    }
)};

const char *deferred_fs = { CRIMILD_TO_STRING(
    in vec4 vWorldVertex;
    in vec3 vWorldNormal;
    in float vLinearDepth;
                                              
    out vec4 vFragData[ 3 ];
                                              
    void main( void )
    {
        vec3 normal = normalize( vWorldNormal );
        
        vFragData[ 0 ] = vec4( 1.0, 1.0, 1.0, 1.0 );
        vFragData[ 1 ] = vec4( vWorldVertex.xyz, vLinearDepth );
        vFragData[ 2 ] = vec4( normal, 0.0 );
    }
)};

DeferredRenderShaderProgram::DeferredRenderShaderProgram( void )
    : ShaderProgram( Utils::getVertexShaderInstance( deferred_vs ), Utils::getFragmentShaderInstance( deferred_fs ) )
{ 
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::NORMAL_ATTRIBUTE, "aNormal" );
    
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM, "uPMatrix" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM, "uVMatrix" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM, "uMMatrix" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::NORMAL_MATRIX_UNIFORM, "uNMatrix" );
}

DeferredRenderShaderProgram::~DeferredRenderShaderProgram( void )
{
    
}

