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

#include "Components/MaterialComponent.hpp"
#include "Mathematics/Frustum.hpp"
#include "Primitives/BoxPrimitive.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Operations/OperationUtils.hpp"
#include "Rendering/Operations/Operations.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/UniformBuffer.hpp"

using namespace crimild;

SharedPointer< FrameGraphOperation > crimild::framegraph::computeBRDFLUT( SharedPointer< FrameGraphResource > const reflectionAtlas ) noexcept
{
    auto renderPass = crimild::alloc< RenderPass >();
    renderPass->setName( "computeBRDFLUT" );

    auto color = useColorAttachment( renderPass->getName() + "/color", Format::R32G32B32A32_SFLOAT );

    renderPass->attachments = { color };

    renderPass->extent = {
        .scalingMode = ScalingMode::FIXED,
        .width = 512.0f,
        .height = 512.0f,
    };

    renderPass->reads( { reflectionAtlas } );
    renderPass->writes( { color } );
    renderPass->produces( { color } );

    auto pipeline = [ & ] {
        auto pipeline = crimild::alloc< GraphicsPipeline >();
        pipeline->setProgram( [] {
            auto program = crimild::alloc< ShaderProgram >(
                Array< SharedPointer< Shader > > {
                    crimild::alloc< Shader >(
                        Shader::Stage::VERTEX,
                        R"(
                                vec2 positions[ 6 ] = vec2[](
                                    vec2( -1.0, 1.0 ),
                                    vec2( -1.0, -1.0 ),
                                    vec2( 1.0, -1.0 ),

                                    vec2( -1.0, 1.0 ),
                                    vec2( 1.0, -1.0 ),
                                    vec2( 1.0, 1.0 ) );

                                vec2 texCoords[ 6 ] = vec2[](
                                    vec2( 0.0, 0.0 ),
                                    vec2( 0.0, 1.0 ),
                                    vec2( 1.0, 1.0 ),

                                    vec2( 0.0, 0.0 ),
                                    vec2( 1.0, 1.0 ),
                                    vec2( 1.0, 0.0 ) );

                                layout( location = 0 ) out vec2 outTexCoord;

                                void main() {
                                    gl_Position = vec4( positions[ gl_VertexIndex ], 0.0, 1.0 );
                                    outTexCoord = texCoords[ gl_VertexIndex ];
                                }
                            )" ),
                    crimild::alloc< Shader >(
                        Shader::Stage::FRAGMENT,
                        R"(
                                layout( location = 0 ) in vec2 inTexCoord;

                                layout( location = 0 ) out vec4 outColor;

                                const float PI = 3.14159265359;

                                // Compute Van Der Corpus sequence
                                float radicalInverseVdC( uint bits )
                                {
                                    bits = ( bits << 16u ) | ( bits >> 16u );
                                    bits = ( ( bits & 0x55555555u ) << 1u ) | ( ( bits & 0xAAAAAAAAu ) >> 1u );
                                    bits = ( ( bits & 0x33333333u ) << 2u ) | ( ( bits & 0xCCCCCCCCu ) >> 2u );
                                    bits = ( ( bits & 0x0F0F0F0Fu ) << 4u ) | ( ( bits & 0xF0F0F0F0u ) >> 4u );
                                    bits = ( ( bits & 0x00FF00FFu ) << 8u ) | ( ( bits & 0xFF00FF00u ) >> 8u );
                                    return float( bits ) * 2.3283064365386963e-10; // / 0x100000000
                                }

                                vec2 hammersley( uint i, uint N )
                                {
                                    return vec2( float( i ) / float( N ), radicalInverseVdC( i ) );
                                }

                                float geometrySchlickGGX( float NdotV, float roughness )
                                {
                                    float a = roughness;
                                    float k = ( a * a ) / 2.0;

                                    float nom = NdotV;
                                    float denom = NdotV * ( 1.0 - k ) + k;

                                    return nom / denom;
                                }

                                float geometrySmith( vec3 N, vec3 V, vec3 L, float roughness )
                                {
                                    float NdotV = max( dot( N, V ), 0.0 );
                                    float NdotL = max( dot( N, L ), 0.0 );
                                    float ggx1 = geometrySchlickGGX( NdotV, roughness );
                                    float ggx2 = geometrySchlickGGX( NdotL, roughness );
                                    return ggx1 * ggx2;
                                }

                                vec3 importanceSampleGGX( vec2 xi, vec3 N, float roughness )
                                {
                                    float a = roughness * roughness;
                                    float phi = 2.0 * PI * xi.x;
                                    float cosTheta = sqrt( ( 1.0 - xi.y ) / ( 1.0 + ( a * a - 1.0 ) * xi.y ) );
                                    float sinTheta = sqrt( 1.0 - cosTheta * cosTheta );

                                    // form spherical coordinates to cartesian coordinates
                                    vec3 H;
                                    H.x = cos( phi ) * sinTheta;
                                    H.y = sin( phi ) * sinTheta;
                                    H.z = cosTheta;

                                    // from tangent-space vector to world-space sample vector
                                    vec3 up = abs( N.z ) < 0.999 ? vec3( 0.0, 0.0, 1.0 ) : vec3( 1.0, 0.0, 0.0 );
                                    vec3 tangent = normalize( cross( up, N ) );
                                    vec3 bitangent = cross( N, tangent );

                                    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
                                    return normalize( sampleVec );
                                }

                                vec2 integrateBRDF( float NdotV, float roughness )
                                {
                                    vec3 V;
                                    V.x = sqrt( 1.0 - NdotV * NdotV );
                                    V.y = 0.0;
                                    V.z = NdotV;

                                    float A = 0.0;
                                    float B = 0.0;

                                    vec3 N = vec3( 0.0, 0.0, 1.0 );

                                    const uint sampleCount = 1024u;
                                    for ( uint i = 0u; i < sampleCount; ++i ) {
                                        vec2 Xi = hammersley( i, sampleCount );
                                        vec3 H = importanceSampleGGX( Xi, N, roughness );
                                        vec3 L = normalize( 2.0 * dot( V, H ) * H - V );

                                        float NdotL = max( dot( N, L ), 0.0 );
                                        float NdotH = max( dot( N, H ), 0.0 );
                                        float VdotH = max( dot( V, H ), 0.0 );

                                        if ( NdotL > 0.0 ) {
                                            float G = geometrySmith( N, V, L, roughness );
                                            float GVis = ( G * VdotH ) / ( NdotH * NdotV );
                                            float Fc = pow( 1.0 - VdotH, 5.0 );

                                            A += ( 1.0 - Fc ) * GVis;
                                            B += Fc * GVis;
                                        }
                                    }

                                    A /= float( sampleCount );
                                    B /= float( sampleCount );

                                    return vec2( A, B );
                                }

                                void main() {
                                    vec2 integratedBRDF = integrateBRDF( inTexCoord.x, 1.0 - inTexCoord.y );
                                    outColor = vec4( integratedBRDF, 0, 1 );
                                }
                            )" ),
                } );
            program->descriptorSetLayouts = {
                [] {
                    auto layout = crimild::alloc< DescriptorSetLayout >();
                    layout->bindings = {};
                    return layout;
                }(),
            };
            return program;
        }() );
        pipeline->viewport = { .scalingMode = ScalingMode::DYNAMIC };
        pipeline->scissor = { .scalingMode = ScalingMode::DYNAMIC };
        return pipeline;
    }();

    auto viewport = ViewportDimensions {
        .scalingMode = ScalingMode::RELATIVE,
    };

    return withConditionalGraphicsCommands(
        renderPass,
        [] {
            // only render once when forced by render system reset
            return false;
        },
        [ pipeline,
          viewport ]( auto commandBuffer ) {
            commandBuffer->setViewport( viewport );
            commandBuffer->setScissor( viewport );
            commandBuffer->bindGraphicsPipeline( crimild::get_ptr( pipeline ) );
            commandBuffer->draw( 6 );
        } );
}
