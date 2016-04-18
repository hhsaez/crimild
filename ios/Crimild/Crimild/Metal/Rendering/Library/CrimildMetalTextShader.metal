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

vertex VertexOut crimild_vertex_shader_text( VertexIn vert [[ stage_in ]],
                                                   constant crimild::metal::MetalStandardUniforms &uniforms [[ buffer( 1 ) ]] )
{
    float4x4 mvMatrix = uniforms.vMatrix * uniforms.mMatrix;
    float4 mvPosition = mvMatrix * float4( vert.position, 1.0 );
    
    VertexOut out;
    out.position = uniforms.pMatrix * mvPosition;
    out.textureCoords = vert.textureCoords;
    
    return out;
}

fragment float4 crimild_fragment_shader_text( VertexOut projectedVertex [[ stage_in ]],
                                              texture2d< float > texture [[ texture( 0 ) ]],
                                              constant crimild::metal::MetalStandardUniforms &uniforms [[ buffer( 1 ) ]] )
{
    constexpr sampler s( coord::normalized, address::repeat, filter::linear);
    
    float4 color = texture.sample(s, projectedVertex.textureCoords);
    color = color.x * uniforms.material.diffuse;
    
    if ( color.a < 0.1 ) {
        discard_fragment();
    }
    
    return color;
}

