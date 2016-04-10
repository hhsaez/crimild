/*
 Copyright (C) 2014 Apple Inc. All Rights Reserved.
 See LICENSE.txt for this sample’s licensing information
 
 Abstract:
 
 Shared data types between CPU code and metal shader code
 
 */

#ifndef CRIMILD_METAL_RENDERING_STANDARD_UNIFORMS_
#define CRIMILD_METAL_RENDERING_STANDARD_UNIFORMS_

#import <simd/simd.h>

#ifdef __cplusplus

#define CRIMILD_METAL_MAX_LIGHTS 4

namespace crimild {
    
    namespace metal {
        
        typedef struct {
            float ambient[ 4 ];
            float diffuse[ 4 ];
            float specular[ 4 ];
            float shininess;
        } MaterialUniform;
        
        typedef struct {
            float position[ 3 ];
            float attenuation[ 3 ];
            float direction[ 3 ];
            float color[ 4 ];
        } LightUniform;
        
        typedef struct {
            MaterialUniform material;
            
            unsigned int lightCount;
            LightUniform lights[ CRIMILD_METAL_MAX_LIGHTS ];
            
            simd::float4x4 pMatrix;
            simd::float4x4 vMatrix;
            simd::float4x4 mMatrix;
        } MetalStandardUniforms;
        
    }
}

#endif

#endif