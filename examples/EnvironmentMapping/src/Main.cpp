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
            [ & ] {
                auto scene = crimild::alloc< Group >();

                auto environmentTexture = [] {
                    auto texture = crimild::alloc< Texture >();
                    texture->imageView = [ & ] {
                        auto imageView = crimild::alloc< ImageView >();
                        imageView->image = ImageManager::getInstance()->loadCubemap(
                            {
                                .filePaths = {
                                    { .path = "assets/textures/right.png" },
                                    { .path = "assets/textures/left.png" },
                                    { .path = "assets/textures/top.png" },
                                    { .path = "assets/textures/bottom.png" },
                                    { .path = "assets/textures/back.png" },
                                    { .path = "assets/textures/front.png" },
                                },
                            } );
                        return imageView;
                    }();
                    texture->sampler = [ & ] {
                        auto sampler = crimild::alloc< Sampler >();
                        sampler->setMinFilter( Sampler::Filter::NEAREST );
                        sampler->setMagFilter( Sampler::Filter::NEAREST );
                        sampler->setWrapMode( Sampler::WrapMode::CLAMP_TO_BORDER );
                        sampler->setCompareOp( CompareOp::NEVER );
                        return sampler;
                    }();
                    return texture;
                }();

                auto createMaterial = [ & ]( std::string fs, auto texture ) {
                    auto material = crimild::alloc< UnlitMaterial >();
                    material->setGraphicsPipeline(
                        [ & ] {
                            auto pipeline = crimild::alloc< GraphicsPipeline >();
                            pipeline->setProgram(
                                [ & ] {
                                    auto program = crimild::alloc< UnlitShaderProgram >();
                                    program->setShaders(
                                        Array< SharedPointer< Shader > > {
                                            crimild::alloc< Shader >(
                                                Shader::Stage::VERTEX,
                                                CRIMILD_TO_STRING(
                                                    layout( location = 0 ) in vec3 inPosition;
                                                    layout( location = 1 ) in vec3 inNormal;
                                                    layout( location = 2 ) in vec3 inTexCoord;

                                                    layout( set = 0, binding = 0 ) uniform RenderPassUniforms {
                                                        mat4 view;
                                                        mat4 proj;
                                                    };

                                                    layout( set = 2, binding = 0 ) uniform GeometryUniforms {
                                                        mat4 model;
                                                    };

                                                    layout( location = 0 ) out vec3 outWorldPos;
                                                    layout( location = 1 ) out vec3 outWorldNormal;
                                                    layout( location = 2 ) out vec3 outWorldEye;

                                                    void main() {
                                                        vec4 worldPos = model * vec4( inPosition, 1.0 );
                                                        gl_Position = proj * view * worldPos;
                                                        outWorldPos = worldPos.xyz;

                                                        outWorldNormal = normalize( mat3( transpose( inverse( model ) ) ) * inNormal );

                                                        mat4 invView = inverse( view );
                                                        outWorldEye = vec3( invView[ 3 ].x, invView[ 3 ].y, invView[ 3 ].z );
                                                    } ) ),
                                                crimild::alloc< Shader >(
                                                    Shader::Stage::FRAGMENT,
                                                    fs ),
                                        } );

                                    return program;
                                }() );
                            return pipeline;
                        }() );
                    material->setTexture( texture );
                    return material;
                };

                auto reflectionMaterial = createMaterial(
                    CRIMILD_TO_STRING(
                        layout( location = 0 ) in vec3 inWorldPos;
                        layout( location = 1 ) in vec3 inWorldNormal;
                        layout( location = 2 ) in vec3 inWorldEye;

                        layout( set = 1, binding = 0 ) uniform samplerCube uSampler;

                        layout( location = 0 ) out vec4 outColor;

                        vec3 reflected( vec3 I ) {
                            vec3 R = reflect( I, normalize( inWorldNormal ) );
                            return texture( uSampler, R ).rgb;
                        }

                        void main() {
                            vec3 I = normalize( inWorldPos - inWorldEye );
                            outColor = vec4( reflected( I ), 1.0 );
                        } ),
                    environmentTexture );

                auto refractionMaterial = createMaterial(
                    CRIMILD_TO_STRING(
                        layout( location = 0 ) in vec3 inWorldPos;
                        layout( location = 1 ) in vec3 inWorldNormal;
                        layout( location = 2 ) in vec3 inWorldEye;

                        layout( set = 1, binding = 0 ) uniform samplerCube uSampler;

                        layout( location = 0 ) out vec4 outColor;

                        vec3 refracted( vec3 I ) {
                            float ratio = 1.0 / 1.33;
                            vec3 R = refract( I, normalize( inWorldNormal ), ratio );
                            return texture( uSampler, R ).rgb;
                        }

                        void main() {
                            vec3 I = normalize( inWorldPos - inWorldEye );

                            outColor = vec4( refracted( I ), 1.0 );
                        } ),
                    environmentTexture );

                math::fibonacciSquares( 20 ).each(
                    [ &, i = 0 ]( auto &it ) mutable {
                        scene->attachNode(
                            [ & ] {
                                auto geometry = crimild::alloc< Geometry >();
                                geometry->attachPrimitive(
                                    crimild::alloc< ParametricSpherePrimitive >(
                                        ParametricSpherePrimitive::Params {
                                            .type = Primitive::Type::TRIANGLES,
                                            .layout = VertexP3N3TC2::getLayout(),
                                        } ) );
                                geometry->setLocal( translation( it.first + Vector3::Constants::UNIT_Z * it.second ) * scale( 0.25f * it.second ) );
                                auto material = ( i++ % 2 == 0 ? reflectionMaterial : refractionMaterial );
                                geometry->attachComponent< MaterialComponent >()->attachMaterial( material );
                                return geometry;
                            }() );
                    } );

                scene->attachNode(
                    [ & ] {
                        return crimild::alloc< Skybox >( environmentTexture );
                    }() );

                scene->attachNode( [] {
                    auto camera = crimild::alloc< Camera >();
                    camera->setLocal( translation( 0.0f, 10.0f, 100.0f ) );
                    camera->attachComponent< FreeLookCameraComponent >();
                    return camera;
                }() );

                scene->perform( StartComponents() );

                return scene;
            }() );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Environment Mapping" );
