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
    float2 textureCoords;
};

vertex VertexOut crimild_vertex_shader_screen_texture( VertexIn vert [[ stage_in ]],
                                                      constant crimild::metal::MetalStandardUniforms &uniforms [[ buffer( 1 ) ]] )
{
    VertexOut out;
    out.position = float4( vert.position.x, vert.position.y, 0.0, 1.0 );
    out.textureCoords = float2( vert.textureCoords.x, 1.0 - vert.textureCoords.y );
    
    return out;
}

fragment float4 crimild_fragment_shader_screen_texture( VertexOut projectedVertex [[ stage_in ]],
                                                       texture2d< float > texture [[ texture( 0 ) ]],
                                                       constant crimild::metal::MetalStandardUniforms &uniforms [[ buffer( 1 ) ]] )
{
    constexpr sampler s( coord::normalized, address::repeat, filter::linear);
    
    float4 sampledColor = texture.sample(s, projectedVertex.textureCoords);
    return sampledColor;
}

