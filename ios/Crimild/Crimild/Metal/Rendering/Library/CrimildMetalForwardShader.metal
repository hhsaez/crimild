//
//  CrimildMetalDiffuseUnlitShader.metal
//  Voyage
//
//  Created by Hernan Saez on 4/9/16.
//  Copyright © 2016 Hernan Saez. All rights reserved.
//

#include <metal_stdlib>

#include "CrimildMetalStandardUniforms.h"

using namespace metal;

struct VertexIn {
    float3 position [[ attribute( 0 ) ]];
    float3 normal [[ attribute( 1 ) ]];
    float2 textureCoords [[ attribute( 2 ) ]];
};

struct VertexOut {
    float4 position [[ position ]];
    float3 worldPosition;
    float3 normal;
    float3 eye;
    float2 textureCoords;
};

vertex VertexOut crimild_vertex_shader_forward( VertexIn vert [[ stage_in ]],
                                                constant crimild::metal::MetalStandardUniforms &uniforms [[ buffer( 1 ) ]] )
{
    float3x3 nMatrix( uniforms.mMatrix[ 0 ].xyz, uniforms.mMatrix[ 1 ].xyz, uniforms.mMatrix[ 2 ].xyz );
    float4 worldPosition = uniforms.mMatrix * float4( vert.position, 1.0 );
    
    VertexOut out;
    out.position = uniforms.pMatrix * uniforms.vMatrix * worldPosition;
    out.worldPosition = worldPosition.xyz;
    out.normal = nMatrix * vert.normal;
    out.eye = -( worldPosition ).xyz;
    out.textureCoords = vert.textureCoords;
    
    return out;
}

fragment float4 crimild_fragment_shader_forward( VertexOut projectedVertex [[ stage_in ]],
                                                 texture2d< float > texture [[ texture( 0 ) ]],
                                                 constant crimild::metal::MetalStandardUniforms &uniforms [[ buffer( 1 ) ]] )
{
    constexpr sampler s( coord::normalized, address::repeat, filter::linear);
    
    float4 diffuse = texture.sample( s, projectedVertex.textureCoords );
    diffuse *= uniforms.material.diffuse;
    if ( diffuse.a == 0.0 ) {
        return diffuse;
    }
    
    if ( uniforms.lightCount == 0 ) {
        return diffuse;
    }
    
    float4 specular = uniforms.material.specular;
    
    float4 finalColor = uniforms.material.ambient;
    finalColor.w = diffuse.w;
    
    float3 normal = normalize( projectedVertex.normal );
    float3 eye = normalize( projectedVertex.eye );
    
    unsigned int maxLights = uniforms.lightCount < CRIMILD_METAL_MAX_LIGHTS ? uniforms.lightCount : CRIMILD_METAL_MAX_LIGHTS;
    for ( unsigned int i = 0; i < maxLights; i++ ) {
        crimild::metal::LightUniform light = uniforms.lights[ i ];
        
        float3 lightVec = normalize( light.position - projectedVertex.worldPosition );
        float3 halfVector = -normalize( reflect( lightVec, normal ) );
        
        float l = dot( normal, lightVec );
        if ( l > 0.0 ) {
            float s = pow( max( dot( halfVector, eye ), 0.0 ), uniforms.material.shininess );
            float d = distance( projectedVertex.worldPosition, light.position );
            float a = 1.0 / ( light.attenuation.x + ( light.attenuation.y * d ) + ( light.attenuation.z * d * d ) );
            
            finalColor.xyz += ( ( diffuse.xyz * l ) + ( specular.xyz * s ) ) * light.color.rgb * a;
        }
    }
    
    return finalColor;
}

