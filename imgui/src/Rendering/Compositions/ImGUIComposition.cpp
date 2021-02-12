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

#include "Rendering/Compositions/ImGUIComposition.hpp"

#include "Rendering/Compositions/OverlayComposition.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/ImageView.hpp"
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
using namespace crimild::compositions;
using namespace crimild::imgui;

Composition imgui::compositions::renderUI( void ) noexcept
{
    Composition cmp;
    auto renderPass = cmp.create< RenderPass >();
    renderPass->setName( "imgui_render_pass" );

    renderPass->attachments = {
        [ & ] {
            auto att = cmp.createAttachment( "imgui_color_attachment" );
            att->usage = Attachment::Usage::COLOR_ATTACHMENT;
            att->format = Format::R8G8B8A8_UNORM;
            att->imageView = crimild::alloc< ImageView >();
            att->imageView->image = crimild::alloc< Image >();
            att->imageView->image->format = Format::R8G8B8A8_UNORM;
            att->imageView->image->setName( "imgui_color_attachment" );
            return crimild::retain( att );
        }(),
    };

    auto pipeline = [ & ] {
        auto pipeline = cmp.create< GraphicsPipeline >();
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

    static auto vbo = crimild::alloc< VertexBuffer >( VertexP2TC2C4::getLayout(), MAX_VERTEX_COUNT );
    vbo->getBufferView()->setUsage( BufferView::Usage::DYNAMIC );
    static auto ibo = crimild::alloc< IndexBuffer >( Format::INDEX_16_UINT, MAX_INDEX_COUNT );
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
            image->data.resize( width * height * 4 );
            memset( image->data.getData(), 0, image->data.size() );
            memcpy( image->data.getData(), pixels, image->data.size() );
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
        auto descriptorSet = cmp.create< DescriptorSet >();
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
                                    Vector4f::ONE,
                                    Vector4f::ZERO,
                                };
                            }

                            // TODO: this scale value seems wrongs. It probably works only on Retina displays
                            auto scale = Vector4f(
                                4.0f / drawData->DisplaySize.x,
                                -4.0f / drawData->DisplaySize.y,
                                0,
                                0 );
                            auto translate = Vector4f(
                                -1.0,
                                1.0,
                                0,
                                0 );
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

    renderPass->setDescriptors( crimild::retain( descriptors ) );
    renderPass->setGraphicsPipeline( pipeline );

    auto commandBuffers = Swapchain::getInstance()->getImages().map(
        [ &, i = 0 ]( auto ) mutable {
            auto commandBuffer = cmp.create< CommandBuffer >();
            commandBuffer->setFrameIndex( i++ );
            return commandBuffer;
        } );

    renderPass->setCommandRecorder(
        [ commandBuffers,
          renderPass = crimild::retain( renderPass ),
          pipeline = crimild::retain( pipeline ),
          descriptors = crimild::retain( descriptors ) ]( Size imageIndex ) -> CommandBuffer * {
            auto commandBuffer = commandBuffers[ imageIndex ];
            commandBuffer->clear();
            commandBuffer->begin( CommandBuffer::Usage::SIMULTANEOUS_USE );
            commandBuffer->beginRenderPass( crimild::get_ptr( renderPass ), nullptr );

            auto drawData = ImGui::GetDrawData();
            if ( drawData == nullptr ) {
                commandBuffer->endRenderPass( crimild::get_ptr( renderPass ) );
                commandBuffer->end();
                return commandBuffer;
            }
            auto vertexCount = drawData->TotalVtxCount;
            auto indexCount = drawData->TotalIdxCount;
            if ( vertexCount == 0 || indexCount == 0 ) {
                CRIMILD_LOG_ERROR( "No vertex data " );
                commandBuffer->endRenderPass( crimild::get_ptr( renderPass ) );
                commandBuffer->end();
                return commandBuffer;
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
                    positions->set( vertexId + j, Vector2f( vertex.pos.x, vertex.pos.y ) );
                    texCoords->set( vertexId + j, Vector2f( vertex.uv.x, vertex.uv.y ) );
                    colors->set( vertexId + j, RGBAColorf( ( ( vertex.col >> 0 ) & 0xFF ) / 255.0f, ( ( vertex.col >> 8 ) & 0xFF ) / 255.0f, ( ( vertex.col >> 16 ) & 0xFF ) / 255.0f, ( ( vertex.col >> 24 ) & 0xFF ) / 255.0f ) );
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
                            cmd->ElemCount,
                            cmd->IdxOffset + indexOffset,
                            cmd->VtxOffset + vertexOffset );
                    }
                }
                indexOffset += cmds->IdxBuffer.Size;
                vertexOffset += cmds->VtxBuffer.Size;
            }

            commandBuffer->endRenderPass( crimild::get_ptr( renderPass ) );
            commandBuffer->end();

            return commandBuffer;
        } );

    cmp.setOutput( crimild::get_ptr( renderPass->attachments[ 0 ] ) );

    return cmp;
}
