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

#include "Mathematics/Vector4_constants.hpp"
#include "Rendering/CommandBuffer.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Image.hpp"
#include "Rendering/ImageView.hpp"
#include "Rendering/Operations/ImGUIOperations.hpp"
#include "Rendering/Operations/OperationUtils.hpp"
#include "Rendering/Operations/Operations.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Uniforms/CallbackUniformBuffer.hpp"
#include "Rendering/Vertex.hpp"
#include "imgui.h"

#define MAX_VERTEX_COUNT 10000
#define MAX_INDEX_COUNT 10000

using namespace crimild;

SharedPointer< FrameGraphOperation > crimild::framegraph::imgui::renderUI( void ) noexcept
{
    auto renderPass = crimild::alloc< RenderPass >();
    renderPass->setName( "imgui_renderUI" );

    auto color = useColorAttachment( "imgui_renderUI/color" );

    renderPass->attachments = { color };

    auto pipeline = [ & ] {
        auto pipeline = crimild::alloc< GraphicsPipeline >();
        pipeline->setProgram(
            [ & ] {
                auto program = crimild::alloc< ShaderProgram >();
                program->setShaders(
                    Array< SharedPointer< Shader > > {
                        crimild::alloc< Shader >(
                            Shader::Stage::VERTEX,
                            R"(
                                layout ( location = 0 ) in vec2 aPosition;
                                layout ( location = 1 ) in vec2 aTexCoord;
                                layout ( location = 2 ) in vec4 aColor;

                                layout ( set = 0, binding = 0 ) uniform TransformBuffer {
                                    vec4 scale;
                                    vec4 translate;
                                } ubo;

                                layout ( location = 0 ) out vec4 vColor;
                                layout ( location = 1 ) out vec2 vTexCoord;

                                void main()
                                {
                                    gl_Position = vec4( aPosition * ubo.scale.xy + ubo.translate.xy, 0.0, 1.0 );
                                    vColor = aColor;
                                    vTexCoord = aTexCoord;
                                }
                            )" ),
                        crimild::alloc< Shader >(
                            Shader::Stage::FRAGMENT,
                            R"(
                                layout ( location = 0 ) in vec4 vColor;
                                layout ( location = 1 ) in vec2 vTexCoord;

                                layout ( set = 0, binding = 1 ) uniform sampler2D uTexture;

                                layout ( location = 0 ) out vec4 FragColor;

                                void main()
                                {
                                    vec2 uv = vTexCoord;
                                    FragColor = vColor * texture( uTexture, uv );
                                }
                            )" ),
                    } );
                program->vertexLayouts = { VertexP2TC2C4::getLayout() };
                program->descriptorSetLayouts = {
                    [] {
                        auto layout = crimild::alloc< DescriptorSetLayout >();
                        layout->bindings = {
                            {
                                .descriptorType = DescriptorType::UNIFORM_BUFFER,
                                .stage = Shader::Stage::VERTEX,
                            },
                            {
                                .descriptorType = DescriptorType::TEXTURE,
                                .stage = Shader::Stage::FRAGMENT,
                            },
                        };
                        return layout;
                    }(),
                };
                return program;
            }() );
        pipeline->depthStencilState.depthTestEnable = false;
        pipeline->depthStencilState.stencilTestEnable = false;
        pipeline->rasterizationState.cullMode = CullMode::NONE;
        pipeline->colorBlendState = ColorBlendState {
            .enable = true,
            .srcColorBlendFactor = BlendFactor::SRC_ALPHA,
            .dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = BlendOp::ADD,
            .srcAlphaBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
            .dstAlphaBlendFactor = BlendFactor::ZERO,
            .alphaBlendOp = BlendOp::ADD,
        };
        return pipeline;
    }();

    auto vbo = crimild::alloc< VertexBuffer >( VertexP2TC2C4::getLayout(), MAX_VERTEX_COUNT );
    vbo->getBufferView()->setUsage( BufferView::Usage::DYNAMIC );

    auto ibo = crimild::alloc< IndexBuffer >( Format::INDEX_16_UINT, MAX_INDEX_COUNT );
    ibo->getBufferView()->setUsage( BufferView::Usage::DYNAMIC );

    auto createFontAtlas = []( void ) -> SharedPointer< Texture > {
        auto &io = ImGui::GetIO();

        io.Fonts->AddFontDefault();

        unsigned char *pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32( &pixels, &width, &height );

        auto texture = crimild::alloc< Texture >();
        texture->imageView = crimild::alloc< ImageView >();
        texture->imageView->image = [ & ] {
            auto image = crimild::alloc< Image >();
            image->setName( "ImGUI_font_atlas" );
            image->extent = {
                .width = Real32( width ),
                .height = Real32( height ),
            };
            image->format = Format::R8G8B8A8_UNORM;
            image->setBufferView(
                crimild::alloc< BufferView >(
                    BufferView::Target::IMAGE,
                    crimild::alloc< Buffer >(
                        [ & ] {
                            auto data = ByteArray( width * height * 4 );
                            memset( data.getData(), 0, data.size() );
                            memcpy( data.getData(), pixels, data.size() );
                            return data;
                        }() ) ) );
            return image;
        }();

        texture->sampler = crimild::alloc< Sampler >();
        texture->sampler->setWrapMode( Sampler::WrapMode::CLAMP_TO_EDGE );
        texture->sampler->setBorderColor( Sampler::BorderColor::FLOAT_OPAQUE_WHITE );

        int idx = 1;
        io.Fonts->TexID = ( ImTextureID )( intptr_t ) idx;

        return texture;
    };

    auto descriptors = [ & ] {
        auto descriptorSet = crimild::alloc< DescriptorSet >();
        descriptorSet->descriptors = {
            {
                .descriptorType = DescriptorType::UNIFORM_BUFFER,
                .obj = [ & ] {
                    struct UITransformBuffer {
                        alignas( 16 ) Vector4f scale;
                        alignas( 16 ) Vector4f translate;
                    };

                    return crimild::alloc< CallbackUniformBuffer< UITransformBuffer > >(
                        [] {
                            auto drawData = ImGui::GetDrawData();
                            if ( drawData == nullptr ) {
                                return UITransformBuffer {
                                    Vector4f::Constants::ONE,
                                    Vector4f::Constants::ZERO,
                                };
                            }

                            // TODO: this scale value seems wrongs. It probably works only on Retina displays
                            auto scale = Vector4f {
                                4.0f / drawData->DisplaySize.x,
                                -4.0f / drawData->DisplaySize.y,
                                0,
                                0,
                            };
                            auto translate = Vector4f {
                                -1.0,
                                1.0,
                                0,
                                0,
                            };
                            return UITransformBuffer {
                                .scale = scale,
                                .translate = translate,
                            };
                        } );
                }(),
            },
            {
                .descriptorType = DescriptorType::TEXTURE,
                .obj = createFontAtlas(),
            },
        };
        return descriptorSet;
    }();

    renderPass->reads( {} );
    renderPass->writes( { color } );
    renderPass->produces( { color } );

    return withDynamicGraphicsCommands(
        renderPass,
        [ pipeline, descriptors, vbo, ibo ]( auto commandBuffer ) {
            auto drawData = ImGui::GetDrawData();
            if ( drawData == nullptr ) {
                return;
            }

            auto vertexCount = drawData->TotalVtxCount;
            auto indexCount = drawData->TotalIdxCount;
            if ( vertexCount == 0 || indexCount == 0 ) {
                // No vertex data. Nothing to render
                return;
            }

            auto positions = vbo->get( VertexAttribute::Name::POSITION );
            auto texCoords = vbo->get( VertexAttribute::Name::TEX_COORD );
            auto colors = vbo->get( VertexAttribute::Name::COLOR );

            auto vertexId = 0l;
            auto indexId = 0l;

            for ( auto i = 0; i < drawData->CmdListsCount; i++ ) {
                const auto cmdList = drawData->CmdLists[ i ];
                for ( auto j = 0l; j < cmdList->VtxBuffer.Size; j++ ) {
                    auto vertex = cmdList->VtxBuffer[ j ];
                    positions->set( vertexId + j, Vector2f { vertex.pos.x, vertex.pos.y } );
                    texCoords->set( vertexId + j, Vector2f { vertex.uv.x, vertex.uv.y } );
                    colors->set( vertexId + j, ColorRGBA {
                                                   ( ( vertex.col >> 0 ) & 0xFF ) / 255.0f,
                                                   ( ( vertex.col >> 8 ) & 0xFF ) / 255.0f,
                                                   ( ( vertex.col >> 16 ) & 0xFF ) / 255.0f,
                                                   ( ( vertex.col >> 24 ) & 0xFF ) / 255.0f,
                                               } );
                }
                for ( auto j = 0l; j < cmdList->IdxBuffer.Size; j++ ) {
                    ibo->setIndex( indexId + j, cmdList->IdxBuffer[ j ] );
                }
                vertexId += cmdList->VtxBuffer.Size;
                indexId += cmdList->IdxBuffer.Size;
            }

            commandBuffer->bindGraphicsPipeline( crimild::get_ptr( pipeline ) );
            commandBuffer->bindDescriptorSet( crimild::get_ptr( descriptors ) );
            commandBuffer->bindVertexBuffer( crimild::get_ptr( vbo ) );
            commandBuffer->bindIndexBuffer( crimild::get_ptr( ibo ) );

            crimild::Size vertexOffset = 0;
            crimild::Size indexOffset = 0;
            for ( int i = 0; i < drawData->CmdListsCount; i++ ) {
                const auto cmds = drawData->CmdLists[ i ];
                for ( auto cmdIt = 0l; cmdIt < cmds->CmdBuffer.Size; cmdIt++ ) {
                    const auto cmd = &cmds->CmdBuffer[ cmdIt ];
                    if ( cmd->UserCallback != nullptr ) {
                        if ( cmd->UserCallback == ImDrawCallback_ResetRenderState ) {
                            // Do nothing?
                        } else {
                            cmd->UserCallback( cmds, cmd );
                        }
                    } else {
                        commandBuffer->drawIndexed(
                            DrawIndexedInfo {
                                .indexCount = cmd->ElemCount,
                                .firstIndex = UInt32( cmd->IdxOffset + indexOffset ),
                                .vertexOffset = Int32( cmd->VtxOffset + vertexOffset ),
                            } );
                    }
                }
                indexOffset += cmds->IdxBuffer.Size;
                vertexOffset += cmds->VtxBuffer.Size;
            }
        } );
}
