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

#include <linearizeDepth>
#include <textureCube>

layout( location = 0 ) in vec3 inNormal;
layout( location = 1 ) in vec3 inPosition;
layout( location = 2 ) in vec3 inViewPosition;
layout( location = 3 ) in vec2 inTexCoord;
layout( location = 4 ) in vec3 inEyePosition;
layout( location = 5 ) in mat3 inViewMatrix3;

struct LightProps {
    uint type;
    vec4 position;
    vec4 direction;
    vec4 ambient;
    vec4 color;
    vec4 attenuation;
    vec4 cutoff;
    bool castShadows;
    float shadowBias;
    vec4 cascadeSplits;
    mat4 lightSpaceMatrix[ 4 ];
    vec4 viewport;
};

layout( set = 0, binding = 1 ) uniform Lighting
{
    LightProps ambientLights[ 1 ];
    uint ambientLightCount;
    LightProps directionalLights[ 2 ];
    uint directionalLightCount;
    LightProps pointLights[ 10 ];
    uint pointLightCount;
    LightProps spotLights[ 4 ];
    uint spotLightCount;
}
lighting;

layout( set = 0, binding = 2 ) uniform sampler2D uShadowAtlas;
layout( set = 0, binding = 3 ) uniform sampler2D uReflectionAtlas;
layout( set = 0, binding = 4 ) uniform sampler2D uIrradianceAtlas;
layout( set = 0, binding = 5 ) uniform sampler2D uPrefilterAtlas;
layout( set = 0, binding = 6 ) uniform sampler2D uBRDFLUT;

layout( set = 1, binding = 0 ) uniform Material
{
    //vec3 albedo;
    //float metallic;
    //float roughness;
    //float ambientOcclusion;
    vec4 albedo;
    vec4 ops;
}
uMaterial;

layout( set = 1, binding = 1 ) uniform sampler2D uAlbedoMap;
layout( set = 1, binding = 2 ) uniform sampler2D uMetallicMap;
layout( set = 1, binding = 3 ) uniform sampler2D uRoughnessMap;
layout( set = 1, binding = 4 ) uniform sampler2D uAmbientOcclusionMap;
layout( set = 1, binding = 5 ) uniform sampler2D uOcclusionRoughnessMetallicMap;
layout( set = 1, binding = 6 ) uniform sampler2D uNormalMap;

layout( location = 0 ) out vec4 outColor;

const float PI = 3.14159265359;

const mat4 bias = mat4(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.5, 0.5, 0.0, 1.0
);

float calculateShadow( vec4 ligthSpacePosition, vec4 viewport, vec3 N, vec3 L, float bias )
{
    // perform perspective divide
    vec3 projCoords = ligthSpacePosition.xyz / ligthSpacePosition.w;
    projCoords.t = 1.0 - projCoords.t;

    // Compute shadow PCF
    float depth = projCoords.z;
    float shadow = 0.0f;
    vec2 texelSize = 1.0 / textureSize( uShadowAtlas, 0 );
    for ( int y = -1; y <= 1; ++y ) {
        for ( int x = -1; x <= 1; ++x ) {
            vec2 uv = projCoords.st + vec2( x, y ) * texelSize;
            uv.x = viewport.x + uv.x * viewport.z;
            uv.y = viewport.y + uv.y * viewport.w;
            float pcfDepth = texture( uShadowAtlas, uv ).x;
            shadow += depth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

int roughnessToLOD( float roughness, float maxLOD )
{
	return int( round( roughness * maxLOD ) );
}

float calculatePointShadow( sampler2D shadowAtlas, float dist, vec3 D, vec4 viewport, float bias )
{
#if 0
    float depth = dist / 200.0; //length( D ) / 200.0;
    float shadow = linearizeDepth( textureCubeUV( shadowAtlas, D, viewport ).r, 0.01, 200.0 );
    return depth - bias > shadow ? 1.0 : 0.0;
#else
float depth = dist;
float shadow = textureCubeUV( shadowAtlas, D, viewport ).r;
return depth <= ( shadow + bias ) ? 0.0 : 1.0;
#endif

    /*
                        // Compute shadow PCF
                        float bias = 0.05;
                        float shadow = 0.0f;

                        for ( int y = -1; y <= 1; ++y ) {
                            for ( int x = -1; x <= 1; ++x ) {
                                for ( int z = -1; z <= 1; ++z ) {
                                    float pcfDepth = textureCubeUV( uShadowAtlas, normalize( D + 0.01 * vec3( x, y, z ) ) ).r;// + vec3( x, y, z ) ).r;
                                    shadow += depth - bias > pcfDepth ? 1.0 : 0.0;
                                }
                            }
                        }
                        shadow /= 9.0;
                        return shadow;
                        */
}

// Computes T, B and TBN matrix based on surface N
// TODO (hernan): is this in view space? If so, we transform T and B to world space
mat3 computeTBN( mat3 invView, vec3 N )
{
    vec3 q1 = dFdx( inPosition );
    vec3 q2 = dFdy( inPosition );
    vec2 st1 = dFdx( inTexCoord );
    vec2 st2 = dFdy( inTexCoord );
    vec3 T = normalize( invView * ( q1 * st2.t - q2 * st1.t ) );
    vec3 B = -normalize( invView * ( -q1 * st2.s + q2 * st1.s ) );
    return mat3( T, B, N );
}

float distributionGGX( vec3 N, vec3 H, float roughness )
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max( dot( N, H ), 0.0 );
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = ( NdotH2 * ( a2 - 1.0 ) + 1.0 );
    denom = PI * denom * denom;

    return num / denom;
}

float geometrySchlickGGX( float NdotV, float roughness )
{
    float r = ( roughness + 1.0 );
    float k = ( r * r ) / 8.0;
    float num = NdotV;
    float denom = NdotV * ( 1.0 - k ) + k;
    return num / denom;
}

float geometrySmith( vec3 N, vec3 V, vec3 L, float roughness )
{
    float NdotV = max( dot( N, V ), 0.0 );
    float NdotL = max( dot( N, L ), 0.0 );
    float ggx1 = geometrySchlickGGX( NdotL, roughness );
    float ggx2 = geometrySchlickGGX( NdotV, roughness );
    return ggx1 * ggx2;
}

vec3 fresnelSchlick( float cosTheta, vec3 F0 )
{
    return F0 + ( 1.0 - F0 ) * pow( 1.0 - cosTheta, 5.0 );
}

vec3 fresnelSchlickRoughness( float cosTheta, vec3 F0, float roughness )
{
    return F0 + ( max( vec3( 1.0 - roughness ), F0 ) - F0 ) * pow( 1.0 - cosTheta, 5.0 );
}

vec3 computeF0( vec3 albedo, float metallic )
{
    vec3 F0 = vec3( 0.04 );
    F0 = mix( F0, albedo, metallic );
    return F0;
}

vec3 brdf( vec3 N, vec3 H, vec3 V, vec3 L, vec3 radiance, vec3 albedo, float metallic, float roughness )
{
    vec3 F0 = computeF0( albedo, metallic );

    float NDF = distributionGGX( N, H, roughness );
    float G = geometrySmith( N, V, L, roughness );
    vec3 F = fresnelSchlick( max( dot( H, V ), 0.0 ), F0 );

    vec3 kS = F;
    vec3 kD = vec3( 1.0 ) - kS;
    kD *= 1.0 - metallic;

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max( dot( N, V ), 0.0 ) * max( dot( N, L ), 0.0 );
    vec3 specular = numerator / max( denominator, 0.001 );

    // add outgoing radiance
    float NdotL = max( dot( N, L ), 0.0 );
    return ( kD * albedo / PI + specular ) * radiance * NdotL;
}

vec3 getPrefilteredColor( vec3 N, vec3 V, float roughness )
{
    const float MAX_REFLECTION_LOD = 4.0;
	const int lod = roughnessToLOD( roughness, MAX_REFLECTION_LOD );
	const vec2 texelSize = 1.0 / textureSize( uPrefilterAtlas, lod );
	const vec3 R = reflect( -V, N );

	vec3 forward = R;
	vec3 up = V;
	vec3 right = normalize( cross( up, forward ) );
	up = normalize( cross( right, forward ) );

	// compute a simple bilinear color by sampling using different vectors
	// this is not ideal, but it helps reduce some visual artifacts
    vec3 color = textureCubeLOD( uPrefilterAtlas, R, lod ).rgb;
    color += textureCubeLOD( uPrefilterAtlas, normalize( R + texelSize.x * up + 0.0 * right ), lod ).rgb;
    color += textureCubeLOD( uPrefilterAtlas, normalize( R + texelSize.x * up + texelSize.y * right ), lod ).rgb;
    color += textureCubeLOD( uPrefilterAtlas, normalize( R + 0.0 * up + 0.0 * right ), lod ).rgb;
    color += textureCubeLOD( uPrefilterAtlas, normalize( R + 0.0 * up + texelSize.y * right ), lod ).rgb;
    color /= 5.0;

    return color;
}

vec3 irradiance( vec3 N, vec3 V, vec3 albedo, float metallic, float roughness, float ao )
{
    vec3 F0 = computeF0( albedo, metallic );
    vec3 F = fresnelSchlickRoughness( max( dot( N, V ), 0.0 ), F0, roughness );

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 I = textureCubeUV( uIrradianceAtlas, N, vec4( 0, 0, 1, 1 ) ).rgb;
    vec3 diffuse = I * albedo;

    const float MAX_REFLECTION_LOD = 4.0;

	vec3 prefilteredColor = getPrefilteredColor( N, V, roughness );
    vec2 envBRDF = texture( uBRDFLUT, vec2( max( dot( N, V ), 0.0 ), roughness ) ).rg;
    vec3 specular = prefilteredColor * ( F * envBRDF.x + envBRDF.y );

    return ( kD * diffuse + specular ) * ao;
}

void main()
{
    // Load a normal from the Normal map.
    vec3 N = texture( uNormalMap, inTexCoord ).rgb;
    if ( N.x * N.y * N.z != 0 ) {
        // Transform normal to range [-1, 1]
        N = normalize( 2.0 * N - 1.0 );
        N = normalize( computeTBN( inViewMatrix3, inNormal ) * N );
    } else {
        // If no normal map is present, the normal vector will be zero
        // In this case, use the input normal from the vertex shader
        N = normalize( inNormal );
    }

    vec3 P = inPosition;
    vec3 E = inEyePosition;

    vec3 V = normalize( E - P );

    vec4 albedo = vec4( uMaterial.albedo.rgb, 1.0 ) * vec4( pow( texture( uAlbedoMap, inTexCoord ).rgb, vec3( 2.2 ) ), 1.0 );
    if ( albedo.a <= 0.01 ) {
        discard;
    }

    float metallic = uMaterial.ops.x * texture( uMetallicMap, inTexCoord ).r;
    float roughness = max( 0.05, uMaterial.ops.y * texture( uRoughnessMap, inTexCoord ).r );
    float ao = 1.0; //uMaterial.ops.z * texture( uAmbientOcclusionMap, inTexCoord ).r;

    metallic = clamp( metallic, 0.0, 1.0 );
	roughness = clamp( roughness, 0.0, 0.999 );

    vec3 F0 = vec3( 0.04 );
    F0 = mix( F0, albedo.rgb, metallic );

    vec3 cascadeColor = vec3( 0 );

    vec3 Lo = vec3( 0 );

    // point lights
    for ( uint i = 0; i < lighting.pointLightCount; ++i ) {
        vec3 L = lighting.pointLights[ i ].position.xyz - inPosition;
        float dist = length( L );
        L = normalize( L );
        vec3 H = normalize( V + L );
        float attenuation = 1.0 / ( dist * dist );
        vec3 radiance = lighting.pointLights[ i ].color.rgb * attenuation;
        vec3 irradiance = brdf( N, H, V, L, radiance, albedo.rgb, metallic, roughness );

        if ( lighting.pointLights[ i ].castShadows ) {
            float shadow = calculatePointShadow(
                uShadowAtlas,
                dist,
                -L,
                lighting.pointLights[ i ].viewport,
                lighting.pointLights[ i ].shadowBias );
            irradiance *= 1.0 - shadow;
        }

        Lo += irradiance;
    }

    // directional lights
    for ( uint i = 0; i < lighting.directionalLightCount; ++i ) {
        vec3 L = normalize( -lighting.directionalLights[ i ].direction.xyz );
        vec3 H = normalize( V + L );
        vec3 radiance = lighting.directionalLights[ i ].color.rgb;
        Lo += brdf( N, H, V, L, radiance, albedo.rgb, metallic, roughness );

#if 0
                                if ( lighting.directionalLights[ i ].castShadows ) {
                                    vec4 cascadeSplits = lighting.directionalLights[ i ].cascadeSplits;

                                    // these are all negative values. Lower means farther away from the eye
                                    float depth = inViewPosition.z;
                                    int cascadeId = 0;
                                    cascadeColor = vec3( 0.0, 1.0, 1.0 );
                                    if ( depth < cascadeSplits[ 0 ] ) {
                                        cascadeId = 1;
                                        cascadeColor = vec3( 1.0, 0.0, 0.0 );
                                    }
                                    if ( depth < cascadeSplits[ 1 ] ) {
                                        cascadeId = 2;
                                        cascadeColor = vec3( 1.0, 1.0, 0.0 );
                                    }
                                    if ( depth < cascadeSplits[ 2 ] ) {
                                        cascadeId = 3;
                                        cascadeColor = vec3( 0.0, 1.0, 0.0 );
                                    }

                                    mat4 lightSpaceMatrix = lighting.directionalLights[ i ].lightSpaceMatrix[ cascadeId ];
                                    vec4 viewport = lighting.directionalLights[ i ].viewport;
                                    vec2 viewportSize = viewport.zw;
                                    viewport.z = 0.5 * viewportSize.x;
                                    viewport.w = 0.5 * viewportSize.y;
                                    if ( cascadeId == 1 ) {
                                        // top-right cascade
                                        viewport.x += 0.5 * viewportSize.x;
                                    } else if ( cascadeId == 2 ) {
                                        // bottom-left cascade
                                        viewport.y += 0.5 * viewportSize.y;
                                    } else if ( cascadeId == 3 ) {
                                        // bottom-rigth cascade
                                        viewport.x += 0.5 * viewportSize.x;
                                        viewport.y += 0.5 * viewportSize.y;
                                    }

                                    vec4 lightSpacePos = ( bias * lightSpaceMatrix * vec4( inPosition, 1.0 ) );
                                    if ( lightSpacePos.z >= -1.0 && lightSpacePos.z <= 1.0 ) {
                                        float shadow = calculateShadow( lightSpacePos, viewport, N, L, 0.005 );
                                        D *= 1.0 - shadow;
                                        S *= 1.0 - shadow;
                                    } else {
                                        // the projected position in light space is outside the light's view frustum.
                                        // For directional lights, this means the object will be never in shadows
                                        // This might not be true for other types of lights. For example, a spot light
                                        // will produce shadows on objects outside its cone of influence
                                    }
                                }
#endif
    }

    /*
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

                                if ( lighting.spotLights[ i ].castShadows ) {
                                    vec4 lightSpacePos = ( bias * lighting.spotLights[ i ].lightSpaceMatrix[ 0 ] * vec4( inPosition, 1.0 ) );
                                    float shadow = calculateShadow( lightSpacePos, lighting.spotLights[ i ].viewport, N, L, 0.005 );
                                    D *= 1.0 - shadow;
                                    S *= 1.0 - shadow;
                                }
                                lightContribution += Lc * lAtt * ( A + D + S );
                            }
                            */

    vec3 ambient = irradiance( N, V, albedo.rgb, metallic, roughness, ao );

    //color.rgb *= cascadeColor;

    outColor = vec4( ambient + Lo, albedo.a );

}

)glsl"
