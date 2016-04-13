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

vertex VertexOut crimild_vertex_shader_forward( VertexIn vert [[ stage_in ]],
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

fragment float4 crimild_fragment_shader_forward( VertexOut projectedVertex [[ stage_in ]],
                                                 constant crimild::metal::MetalStandardUniforms &uniforms [[ buffer( 1 ) ]] )
{
    return float4( uniforms.material.diffuse[ 0 ], uniforms.material.diffuse[ 1 ], uniforms.material.diffuse[ 2 ], uniforms.material.diffuse[ 3 ] );
}

