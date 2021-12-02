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

#include <Crimild.hpp>

using namespace crimild;

class Example : public Simulation {
public:
    void onStarted( void ) noexcept override
    {
        setScene(
            [] {
                auto scene = crimild::alloc< Group >();

                scene->attachNode( crimild::alloc< Skybox >( RGBColorf( 0.1f, 0.2f, 0.3f ) ) );

                scene->attachNode(
                    [] {
                        auto cube = crimild::alloc< Geometry >();
                        cube->attachPrimitive(
                            crimild::alloc< BoxPrimitive >(
                                BoxPrimitive::Params {} ) );
                        cube->attachComponent< MaterialComponent >(
                            [] {
                                auto material = crimild::alloc< UnlitMaterial >();
                                material->setColor( RGBAColorf( 0.8f, 0.7f, 0.6f, 1.0f ) );
                                material->setTexture(
                                    [] {
                                        auto texture = crimild::alloc< Texture >();
                                        texture->imageView = crimild::alloc< ImageView >();
                                        texture->imageView->image = Image::CHECKERBOARD_16;
                                        texture->sampler = crimild::alloc< Sampler >();
                                        texture->sampler->setMinFilter( Sampler::Filter::NEAREST );
                                        texture->sampler->setMagFilter( Sampler::Filter::NEAREST );
                                        return texture;
                                    }() );
                                material->getGraphicsPipeline()->rasterizationState = RasterizationState {
                                    .cullMode = CullMode::NONE,
                                };
                                material->getGraphicsPipeline()->colorBlendState = ColorBlendState {
                                    .enable = true,
                                    .srcColorBlendFactor = BlendFactor::SRC_ALPHA,
                                    .dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
                                };
                                material->getGraphicsPipeline()->setProgram(
                                    [] {
                                        auto program = crimild::alloc< UnlitShaderProgram >();
                                        program->setShaders(
                                            Array< SharedPointer< Shader > > {
                                                crimild::alloc< Shader >(
                                                    Shader::Stage::VERTEX,
                                                    R"(
                                                        layout( location = 0 ) in vec3 inPosition;
                                                        layout( location = 1 ) in vec3 inNormal;
                                                        layout( location = 2 ) in vec2 inTexCoord;

                                                        layout ( set = 0, binding = 0 ) uniform RenderPassUniforms {
                                                            mat4 view;
                                                            mat4 proj;
                                                        };

                                                        layout ( set = 2, binding = 0 ) uniform GeometryUniforms {
                                                            mat4 model;
                                                        };

                                                        layout ( location = 0 ) out vec2 outTexCoord;
                                                        layout ( location = 1 ) out vec3 outRayOrigin;
                                                        layout ( location = 2 ) out vec3 outHitPos;

                                                        void main()
                                                        {
                                                            gl_Position = proj * view * model * vec4( inPosition, 1.0 );
                                                            outTexCoord = inTexCoord;
                                                            outRayOrigin = ( inverse( model ) * inverse( view ) * vec4( 0, 0, 0, 1 ) ).xyz;
                                                            outHitPos = inPosition;
                                                        }
                                                    )" ),
                                                crimild::alloc< Shader >(
                                                    Shader::Stage::FRAGMENT,
                                                    R"(
                                                        layout ( location = 0 ) in vec2 inTexCoord;
                                                        layout ( location = 1 ) in vec3 inRayOrigin;
                                                        layout ( location = 2 ) in vec3 inHitPos;

                                                        layout ( set = 1, binding = 0 ) uniform sampler2D uColorMap;

                                                        layout ( location = 0 ) out vec4 outColor;

                                                        const int MAX_STEPS = 100;
                                                        const float SURFACE_DISTANCE = 0.001;
                                                        const float MAX_DISTANCE = 100.0;

                                                        float getDist( vec3 p )
                                                        {
                                                            float d = length( p ) - 0.5;

                                                            d = length( vec2( length( p.xz ) - 0.5, p.y ) ) - 0.1;

                                                            return d;
                                                        }

                                                        float raymarch( vec3 ro, vec3 rd )
                                                        {
                                                            float dO = 0;

                                                            for ( int i = 0; i < MAX_STEPS; i++ ) {
                                                                vec3 p = ro + dO * rd;
                                                                float dS = getDist( p );
                                                                dO += dS;
                                                                if ( dS < SURFACE_DISTANCE || dO > MAX_DISTANCE ) {
                                                                    break;
                                                                }
                                                            }

                                                            return dO;
                                                        }

                                                        vec3 getNormal( vec3 p )
                                                        {
                                                            vec2 e = vec2( 1e-2, 0 );
                                                            vec3 n = getDist( p ) - vec3(
                                                                getDist( p - e.xyy ),
                                                                getDist( p - e.yxy ),
                                                                getDist( p - e.yyx )
                                                            );
                                                            return normalize( n );
                                                        }

                                                        void main()
                                                        {
                                                            vec2 uv = inTexCoord - 0.5;
                                                            // uv.y *= -1.0;

                                                            vec3 ro = vec3( 0, 0, -3 );
                                                            vec3 rd = normalize( vec3( uv.x, uv.y, 1 ) );
                                                            // rd = ( inView * vec4( normalize( vec3( uv.x, uv.y, -1 ) ), 0 ) ).xyz;

                                                            ro = inRayOrigin;
                                                            rd = normalize( inHitPos - inRayOrigin );

                                                            vec3 color = texture( uColorMap, inTexCoord ).rgb;

                                                            float d = raymarch( ro, rd );
                                                            if ( d < MAX_DISTANCE ) {
                                                                vec3 p = ro + d * rd;
                                                                vec3 n = getNormal( p );
                                                                color *= n;
                                                            }

float alpha = dot( color, color ) > 0.0 ? 1.0 : 0.0;

                                                            outColor = vec4( color, alpha );
                                                        }
                                                    )" ),
                                            } );
                                        return program;
                                    }() );
                                return material;
                            }() );
                        return cube;
                    }() );

                scene->attachNode(
                    [] {
                        auto camera = crimild::alloc< Camera >();
                        camera->local().setTranslate( 1, 3, 5 );
                        camera->local().lookAt( Vector3f::ZERO );
                        Camera::setMainCamera( camera );
                        camera->attachComponent< FreeLookCameraComponent >();
                        return camera;
                    }() );

                scene->perform( StartComponents() );

                return scene;
            }() );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Raymarching: Cube" );
