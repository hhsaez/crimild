/*
 * Copyright (c) 2002 - present, H. Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

R"glsl(

layout( location = 0 ) in vec3 inPosition;
layout( location = 1 ) in vec3 inNormal;
layout( location = 2 ) in vec2 inTexCoord;

layout( set = 0, binding = 0 ) uniform RenderPassUniforms
{
    mat4 view;
    mat4 proj;
};

layout( set = 2, binding = 0 ) uniform GeometryUniforms
{
    mat4 model;
};

layout( location = 0 ) out vec3 outNormal;
layout( location = 1 ) out vec3 outPosition;
layout( location = 2 ) out vec3 outViewPosition;
layout( location = 3 ) out vec2 outTexCoord;
layout( location = 4 ) out vec3 outEyePosition;
layout( location = 5 ) out mat3 outViewMatrix3;

void main()
{
    gl_Position = proj * view * model * vec4( inPosition, 1.0 );    

    mat3 nMatrix = mat3( transpose( inverse( model ) ) );

    outViewMatrix3 = mat3( inverse( view ) );
    outNormal = normalize( nMatrix * inNormal );
    outPosition = vec3( model * vec4( inPosition, 1.0 ) );
    outViewPosition = ( view * model * vec4( inPosition, 1.0 ) ).xyz;
    outEyePosition = inverse( view )[ 3 ].xyz;
    outTexCoord = inTexCoord;
}

)glsl"
