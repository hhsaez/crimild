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
    float2 uv [[ attribute( 2 ) ]];
};

struct VertexOut {
    float4 position [[ position ]];
    float3 normal;
    float3 eye;
};

vertex VertexOut crimild_vertex_shader_lit_texture( VertexIn vert [[ stage_in ]],
                                                    constant crimild::metal::MetalStandardUniforms &uniforms [[ buffer( 1 ) ]] )
{
    float4x4 mvMatrix = uniforms.vMatrix * uniforms.mMatrix;
    float3x3 nMatrix( mvMatrix[ 0 ].xyz, mvMatrix[ 1 ].xyz, mvMatrix[ 2 ].xyz );
    float4 mvPosition = mvMatrix * float4( vert.position, 1.0 );
    
    VertexOut out;
    out.position = uniforms.pMatrix * mvPosition;
    out.normal = nMatrix * vert.normal;
    out.eye = -( mvPosition ).xyz;
    
    return out;
}

fragment float4 crimild_fragment_shader_lit_texture( VertexOut projectedVertex [[ stage_in ]],
                                                     constant crimild::metal::MetalStandardUniforms &uniforms [[ buffer( 1 ) ]] )
{
    float4 color = float4( 1.0 );
    
    float3 normal = normalize( projectedVertex.normal );
    float3 eye = normalize( projectedVertex.eye );

    unsigned int maxLights = uniforms.lightCount < CRIMILD_METAL_MAX_LIGHTS ? uniforms.lightCount : CRIMILD_METAL_MAX_LIGHTS;
    float4 accumLightColor = float4( 0.0 );
    for ( unsigned int i = 0; i < maxLights; i++ ) {
        crimild::metal::LightUniform light = uniforms.lights[ i ];
        
        float4 ambientColor = light.color * uniforms.material.ambient;
        
        float3 lightDirection = normalize( light.position - projectedVertex.position.xyz );
        
        float diffuseFactor = max( 0.0, dot( normal, lightDirection ) );
        float4 diffuseColor = light.color * uniforms.material.diffuse * diffuseFactor;
        
        float3 reflection = reflect( lightDirection, normal );
        float specularFactor = pow( max( 0.0, dot( reflection, eye ) ), uniforms.material.shininess );
        float4 specularColor = light.color * uniforms.material.specular * specularFactor;
        
        float d = distance( projectedVertex.position.xyz, light.position );
        float attenuation = 1.0 / ( light.attenuation.x + ( light.attenuation.y * d ) + ( light.attenuation.z * d * d ) );

        accumLightColor += float4( attenuation * ( ambientColor + diffuseColor + specularColor ).xyz, 1.0 );
    }
    
    return color * accumLightColor;
}


