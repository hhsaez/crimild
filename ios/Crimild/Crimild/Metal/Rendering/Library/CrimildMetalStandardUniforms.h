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
            simd::float4 ambient;
            simd::float4 diffuse;
            simd::float4 specular;
            float shininess;
        } MaterialUniform;
        
        typedef struct {
            simd::float3 position;
            simd::float3 attenuation;
            simd::float3 direction;
            simd::float4 color;
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