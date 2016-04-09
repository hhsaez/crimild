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
    packed_float3 positions;
    packed_float3 normals;
    packed_float2 uvs;
};

struct Uniforms {
    float4x4 mMatrix;
    float4x4 vMatrix;
    float4x4 pMatrix;
};

vertex float4 crimild_vertex_shader_unlit_diffuse( const device VertexIn *vertices [[ buffer( 0 ) ]],
                                                   const device ushort *indices [[ buffer( 1 ) ]],
                                                   const device Uniforms &uniforms [[ buffer( 2 ) ]],
                                                   unsigned int vid [[ vertex_id ]] )
{
    return uniforms.pMatrix * uniforms.vMatrix * uniforms.mMatrix * float4( vertices[ indices[ vid ] ].positions, 1.0 );
}

fragment half4 crimild_fragment_shader_unlit_diffuse()
{
    return half4( 1.0 );
}

