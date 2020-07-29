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

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout ( location = 0 ) in vec3 inNormal;
layout ( location = 1 ) in vec3 inPosition;
layout ( location = 2 ) in vec2 inTexCoord;
layout ( location = 3 ) in vec3 inEyePosition;

struct LightProps {
    uint type;
    vec4 position;
    vec4 direction;
    vec4 ambient;
    vec4 color;
    vec4 attenuation;
    vec4 cutoff;
};

layout ( set = 0, binding = 1 ) uniform Lighting {
    LightProps ambientLights[ 1 ];
    uint ambientLightCount;
    LightProps directionalLights[ 2 ];
    uint directionalLightCount;
    LightProps pointLights[ 10 ];
    uint pointLightCount;
    LightProps spotLights[ 4 ];
    uint spotLightCount;
} lighting;

layout ( set = 1, binding = 0 ) uniform Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
} material;

layout ( set = 1, binding = 1 ) uniform sampler2D uDiffuseMap;
layout ( set = 1, binding = 2 ) uniform sampler2D uSpecularMap;

layout ( location = 0 ) out vec4 outColor;

vec3 ambient( vec3 lAmbient )
{
    return lAmbient * material.ambient.rgb;
}

vec3 diffuse( vec3 N, vec3 L )
{
    float d = max( dot( N, L ), 0.0 );
    return vec3( d ) * material.diffuse.rgb;
}

vec3 specular( vec3 N, vec3 L, vec3 E, vec3 P, vec3 S )
{
    vec3 V = normalize( E - P );
    vec3 H = normalize( L + V );
    float s = pow( max( dot( N, H ), 0.0 ), material.shininess );
    return vec3( s ) * S;
}

float attenuation( float d, vec3 K )
{
    return 1.0 / ( K.x + d * K.y + d * d * K.z );
}

void main()
{
    vec3 N = normalize( inNormal );
    vec3 P = inPosition;
    vec3 E = inEyePosition;
    vec3 MS = material.specular.rgb * texture( uSpecularMap, inTexCoord ).rgb;

    vec3 lightContribution = vec3( 0 );

    // ambient lights
    for ( uint i = 0; i < lighting.ambientLightCount; ++i ) {
        vec3 A = ambient( lighting.ambientLights[ i ].ambient.rgb );
        lightContribution += A;
    }

    // point lights
    for ( uint i = 0; i < lighting.pointLightCount; ++i ) {
        vec3 L = lighting.pointLights[ i ].position.xyz - inPosition;
        float dist = length( L );
        L = normalize( L );
        vec3 Lc = lighting.pointLights[ i ].color.rgb;
        vec3 A = ambient( lighting.pointLights[ i ].ambient.rgb );
        vec3 D = diffuse( N, L );
        vec3 S = specular( N, L, E, P, MS );
        float lAtt = attenuation( dist, lighting.pointLights[ i ].attenuation.xyz );
        lightContribution += Lc * lAtt * ( A + D + S );
    }

    // directional lights
    for ( uint i = 0; i < lighting.directionalLightCount; ++i ) {
        vec3 L = normalize( -lighting.directionalLights[ i ].direction.xyz );
        vec3 Lc = lighting.directionalLights[ i ].color.rgb;
        vec3 A = ambient( lighting.directionalLights[ i ].ambient.rgb );
        vec3 D = diffuse( N, L );
        vec3 S = specular( N, L, E, P, MS );
        lightContribution += Lc * ( A + D + S );
    }

    // spot light
    for ( uint i = 0; i < lighting.spotLightCount; ++i ) {
        vec3 L = lighting.spotLights[ i ].position.xyz - inPosition;
        float dist = length( L );
        L = normalize( L );
        float theta = dot( L, normalize( -lighting.spotLights[ i ].direction.xyz ) );
        float epsilon = lighting.spotLights[ i ].cutoff.x - lighting.spotLights[ i ].cutoff.y;
        float intensity = clamp( ( theta - lighting.spotLights[ i ].cutoff.y ) / epsilon, 0.0, 1.0 );
        vec3 Lc = lighting.spotLights[ i ].color.rgb;
        vec3 A = ambient( lighting.spotLights[ i ].ambient.rgb );
        vec3 D = intensity * diffuse( N, L );
        vec3 S = intensity * specular( N, L, E, P, MS );
        float lAtt = attenuation( dist, lighting.spotLights[ i ].attenuation.xyz );
        lightContribution += Lc * lAtt * ( A + D + S );
    }

    vec3 color = texture( uDiffuseMap, inTexCoord ).rgb;

	outColor = vec4( color * lightContribution, 1.0 );
}
