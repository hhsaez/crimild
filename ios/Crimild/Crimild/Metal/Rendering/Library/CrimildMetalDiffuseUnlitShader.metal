//
//  CrimildMetalDiffuseUnlitShader.metal
//  Voyage
//
//  Created by Hernan Saez on 4/9/16.
//  Copyright © 2016 Hernan Saez. All rights reserved.
//

#include <metal_stdlib>

using namespace metal;

struct VertexIn {
    float3 positions [[ attribute( 0 ) ]];
    float3 normals [[ attribute( 1 ) ]];
    float2 uvs [[ attribute( 2 ) ]];
};

struct VertexOut {
    float4 positions [[ position ]];
    float4 color;
};

vertex VertexOut crimild_vertex_shader_unlit_diffuse( VertexIn vert [[ stage_in ]],
                                                      constant float4x4 &mMatrix [[ buffer( 1 ) ]],
                                                      constant float4x4 &vMatrix [[ buffer( 2 ) ]],
                                                      constant float4x4 &pMatrix [[ buffer( 3 ) ]],
                                                      constant float4 &materialDiffuse [[ buffer( 4 ) ]] )
{
    VertexOut out;
    out.positions = pMatrix * vMatrix * mMatrix * float4( vert.positions, 1.0 );
    out.color = materialDiffuse;
    return out;
}

fragment float4 crimild_fragment_shader_unlit_diffuse( VertexOut projectedVertex [[ stage_in ]] )
{
    return projectedVertex.color;
}

