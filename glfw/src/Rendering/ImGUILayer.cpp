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
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Rendering/ImGUILayer.hpp"

#include "Foundation/ImGUIUtils.hpp"
#include "Rendering/IndexBuffer.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/Vertex.hpp"
#include "Rendering/VulkanGraphicsPipeline.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Simulation/Settings.hpp"

#define MAX_VERTEX_COUNT 100000
#define MAX_INDEX_COUNT 100000

using namespace crimild;
using namespace crimild::vulkan;

ImGUILayer::ImGUILayer( vulkan::RenderDevice *renderDevice ) noexcept
    : m_renderDevice( renderDevice ),
      m_program(
          [ & ] {
              auto program = std::make_unique< ShaderProgram >();
              program->setShaders(
                  Array< SharedPointer< Shader > > {
                      crimild::alloc< Shader >(
                          Shader::Stage::VERTEX,
                          R"(
                            layout ( location = 0 ) in vec2 aPosition;
                            layout ( location = 1 ) in vec2 aTexCoord;
                            layout ( location = 2 ) in vec4 aColor;

                            layout ( set = 0, binding = 0 ) uniform RenderPassUniforms {
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

                            layout ( set = 1, binding = 0 ) uniform sampler2D uTexture;

                            layout ( location = 0 ) out vec4 FragColor;

                            void main()
                            {
                                vec2 uv = vTexCoord;
                                FragColor = vColor * texture( uTexture, uv );
                            }
                        )" ) } );
              return program;
          }() ),
      m_vertices(
          [] {
              auto vbo = std::make_unique< VertexBuffer >( VertexP2TC2C4::getLayout(), MAX_VERTEX_COUNT );
              vbo->getBufferView()->setUsage( BufferView::Usage::DYNAMIC );
              return vbo;
          }() ),
      m_indices(
          [] {
              auto ibo = std::make_unique< IndexBuffer >( Format::INDEX_16_UINT, MAX_INDEX_COUNT );
              ibo->getBufferView()->setUsage( BufferView::Usage::DYNAMIC );
              return ibo;
          }() )
{
    CRIMILD_LOG_TRACE();

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    auto &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    // Only move windows by dragging from title bar (if present).
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
    io.KeyMap[ ImGuiKey_Tab ] = CRIMILD_INPUT_KEY_TAB;
    io.KeyMap[ ImGuiKey_LeftArrow ] = CRIMILD_INPUT_KEY_LEFT;
    io.KeyMap[ ImGuiKey_RightArrow ] = CRIMILD_INPUT_KEY_RIGHT;
    io.KeyMap[ ImGuiKey_UpArrow ] = CRIMILD_INPUT_KEY_UP;
    io.KeyMap[ ImGuiKey_DownArrow ] = CRIMILD_INPUT_KEY_DOWN;
    io.KeyMap[ ImGuiKey_PageUp ] = CRIMILD_INPUT_KEY_PAGE_UP;
    io.KeyMap[ ImGuiKey_PageDown ] = CRIMILD_INPUT_KEY_PAGE_DOWN;
    io.KeyMap[ ImGuiKey_Home ] = CRIMILD_INPUT_KEY_HOME;
    io.KeyMap[ ImGuiKey_End ] = CRIMILD_INPUT_KEY_END;
    io.KeyMap[ ImGuiKey_Insert ] = CRIMILD_INPUT_KEY_INSERT;
    io.KeyMap[ ImGuiKey_Delete ] = CRIMILD_INPUT_KEY_DELETE;
    io.KeyMap[ ImGuiKey_Backspace ] = CRIMILD_INPUT_KEY_BACKSPACE;
    io.KeyMap[ ImGuiKey_Space ] = CRIMILD_INPUT_KEY_SPACE;
    io.KeyMap[ ImGuiKey_Enter ] = CRIMILD_INPUT_KEY_ENTER;
    io.KeyMap[ ImGuiKey_Escape ] = CRIMILD_INPUT_KEY_ESCAPE;
    io.KeyMap[ ImGuiKey_KeyPadEnter ] = CRIMILD_INPUT_KEY_KP_ENTER;
    io.KeyMap[ ImGuiKey_A ] = CRIMILD_INPUT_KEY_A;
    io.KeyMap[ ImGuiKey_C ] = CRIMILD_INPUT_KEY_C;
    io.KeyMap[ ImGuiKey_V ] = CRIMILD_INPUT_KEY_V;
    io.KeyMap[ ImGuiKey_X ] = CRIMILD_INPUT_KEY_X;
    io.KeyMap[ ImGuiKey_Y ] = CRIMILD_INPUT_KEY_Y;
    io.KeyMap[ ImGuiKey_Z ] = CRIMILD_INPUT_KEY_Z;

    updateDisplaySize();
    init();
}

ImGUILayer::~ImGUILayer( void ) noexcept
{
    clean();
    ImGui::DestroyContext();
}

Event ImGUILayer::handle( const Event &e ) noexcept
{
    auto &io = ImGui::GetIO();

    switch ( e.type ) {
        case Event::Type::WINDOW_RESIZE: {
            clean();
            updateDisplaySize();
            init();
            break;
        }

        case Event::Type::KEY_DOWN: {
            io.KeysDown[ e.keyboard.key ] = true;
            io.KeyCtrl = io.KeysDown[ CRIMILD_INPUT_KEY_LEFT_CONTROL ] || io.KeysDown[ CRIMILD_INPUT_KEY_RIGHT_CONTROL ];
            io.KeyShift = io.KeysDown[ CRIMILD_INPUT_KEY_LEFT_SHIFT ] || io.KeysDown[ CRIMILD_INPUT_KEY_RIGHT_SHIFT ];
            io.KeyAlt = io.KeysDown[ CRIMILD_INPUT_KEY_LEFT_ALT ] || io.KeysDown[ CRIMILD_INPUT_KEY_RIGHT_ALT ];
            io.KeySuper = io.KeysDown[ CRIMILD_INPUT_KEY_LEFT_SUPER ] || io.KeysDown[ CRIMILD_INPUT_KEY_RIGHT_SUPER ];
            break;
        }

        case Event::Type::KEY_UP: {
            io.KeysDown[ e.keyboard.key ] = false;
            break;
        }

        case Event::Type::MOUSE_MOTION: {
            io.MousePos = ImVec2( e.motion.pos.x, e.motion.pos.y );
            break;
        }

        case Event::Type::MOUSE_BUTTON_DOWN: {
            io.MouseDown[ e.button.button ] = true;
            break;
        }

        case Event::Type::MOUSE_BUTTON_UP: {
            io.MouseDown[ e.button.button ] = false;
            break;
        }

        case Event::Type::MOUSE_CLICK: {
            break;
        }

        case Event::Type::MOUSE_WHEEL: {
            io.MouseWheelH += e.wheel.x;
            io.MouseWheel += e.wheel.y;
            break;
        }

        case Event::Type::TEXT: {
            if ( e.text.codepoint > 0 && e.text.codepoint < 0x10000 ) {
                io.AddInputCharacter( e.text.codepoint );
            }
            break;
        }

        default: {
            break;
        }
    }

    return Layer::handle( e );
}

void ImGUILayer::render( void ) noexcept
{
    auto &io = ImGui::GetIO();

    m_clock.tick();
    io.DeltaTime = Numericf::max( 1.0f / 60.0f, m_clock.getDeltaTime() );

    if ( !io.Fonts->IsBuilt() ) {
        CRIMILD_LOG_ERROR( "Font atlas is not built!" );
        return;
    }

    // This should not be necessary
    updateDisplaySize();

    ImGui::NewFrame();

    // Render sublayers now
    // We can render either ImGUI commands or render passes at this step.
    Layer::render();

    ImGui::Render();

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

    auto positions = m_vertices->get( VertexAttribute::Name::POSITION );
    auto texCoords = m_vertices->get( VertexAttribute::Name::TEX_COORD );
    auto colors = m_vertices->get( VertexAttribute::Name::COLOR );

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
            m_indices->setIndex( indexId + j, cmdList->IdxBuffer[ j ] );
        }
        vertexId += cmdList->VtxBuffer.Size;
        indexId += cmdList->IdxBuffer.Size;
    }

    const auto currentFrameIndex = m_renderDevice->getCurrentFrameIndex();
    auto commandBuffer = m_renderDevice->getCurrentCommandBuffer();

    auto renderPassInfo = VkRenderPassBeginInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = m_renderPass,
        .framebuffer = m_framebuffers[ currentFrameIndex ],
        .renderArea = m_renderArea,
        .clearValueCount = 0,
        .pClearValues = nullptr,
    };

    m_renderPassObjects.uniforms->setValue(
        [] {
            auto scale = Vector4::Constants::ONE;
            auto translate = Vector4::Constants::ZERO;

            auto drawData = ImGui::GetDrawData();
            if ( drawData != nullptr ) {
                // TODO: this scale value seems wrongs. It probably works only on Retina displays
                scale = Vector4f {
                    4.0f / drawData->DisplaySize.x,
                    -4.0f / drawData->DisplaySize.y,
                    0,
                    0,
                };
                translate = Vector4f {
                    -1.0,
                    1.0,
                    0,
                    0,
                };
            }
            return RenderPassObjects::Uniforms {
                .scale = scale,
                .translate = translate,
            };
        }() );
    m_renderDevice->update( m_renderPassObjects.uniforms.get() );

    vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );

    vkCmdBindPipeline(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline->getHandle() );

    {
        VkBuffer buffers[] = { m_renderDevice->bind( m_vertices.get() ) };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers( commandBuffer, 0, 1, buffers, offsets );
    }

    vkCmdBindIndexBuffer(
        commandBuffer,
        m_renderDevice->bind( m_indices.get() ),
        0,
        vulkan::utils::getIndexType( m_indices.get() ) );

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline->getPipelineLayout(),
        0,
        1,
        &m_renderPassObjects.descriptorSets[ currentFrameIndex ],
        0,
        nullptr );

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
                auto descriptorSets = ( VkDescriptorSet * ) ( cmd->TextureId );
                auto &descriptors = descriptorSets[ currentFrameIndex ];
                vkCmdBindDescriptorSets(
                    commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    m_pipeline->getPipelineLayout(),
                    1,
                    1,
                    &descriptors,
                    0,
                    nullptr );

                vkCmdDrawIndexed( commandBuffer, cmd->ElemCount, 1, cmd->IdxOffset + indexOffset, cmd->VtxOffset + vertexOffset, 0 );
            }
        }
        indexOffset += cmds->IdxBuffer.Size;
        vertexOffset += cmds->VtxBuffer.Size;
    }

    vkCmdEndRenderPass( commandBuffer );
}

void ImGUILayer::updateDisplaySize( void ) const noexcept
{
    auto &io = ImGui::GetIO();
    // TODO: use extents from resize event
    auto width = Settings::getInstance()->get< float >( "video.width", 0 );
    auto height = Settings::getInstance()->get< float >( "video.height", 1 );
    auto framebufferScale = Settings::getInstance()->get< float >( "video.framebufferScale", 1 );
    auto displaySizeScale = 2.0f / framebufferScale;
    io.DisplaySize = ImVec2( width * displaySizeScale, height * displaySizeScale );
    io.DisplayFramebufferScale = ImVec2( displaySizeScale, displaySizeScale );
}

void ImGUILayer::init( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    m_renderArea = VkRect2D {
        .offset = {
            0,
            0,
        },
        .extent = m_renderDevice->getSwapchainExtent(),
    };

    auto attachments = std::array< VkAttachmentDescription, 1 > {
        VkAttachmentDescription {
            .format = m_renderDevice->getSwapchainFormat(),
            .samples = VK_SAMPLE_COUNT_1_BIT,
            // Don't clear input. Just load it as it is
            .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        }
    };

    auto colorReferences = std::array< VkAttachmentReference, 1 > {
        VkAttachmentReference {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        },
    };

    auto subpass = VkSubpassDescription {
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = crimild::UInt32( colorReferences.size() ),
        .pColorAttachments = colorReferences.data(),
        .pResolveAttachments = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = nullptr,
    };

    auto dependencies = std::array< VkSubpassDependency, 2 > {
        VkSubpassDependency {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        },
        VkSubpassDependency {
            .srcSubpass = 0,
            .dstSubpass = VK_SUBPASS_EXTERNAL,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        }
    };

    auto createInfo = VkRenderPassCreateInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = static_cast< crimild::UInt32 >( attachments.size() ),
        .pAttachments = attachments.data(),
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = crimild::UInt32( dependencies.size() ),
        .pDependencies = dependencies.data(),
    };

    CRIMILD_VULKAN_CHECK(
        vkCreateRenderPass(
            m_renderDevice->getHandle(),
            &createInfo,
            nullptr,
            &m_renderPass ) );

    m_framebuffers.resize( m_renderDevice->getSwapchainImageViews().size() );
    for ( uint8_t i = 0; i < m_framebuffers.size(); ++i ) {
        const auto &imageView = m_renderDevice->getSwapchainImageViews()[ i ];

        auto attachments = std::array< VkImageView, 1 > {
            imageView,
            // TODO: add depth image view if available
        };

        auto createInfo = VkFramebufferCreateInfo {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .renderPass = m_renderPass,
            .attachmentCount = uint32_t( attachments.size() ),
            .pAttachments = attachments.data(),
            .width = m_renderDevice->getSwapchainExtent().width,
            .height = m_renderDevice->getSwapchainExtent().height,
            .layers = 1,
        };

        CRIMILD_VULKAN_CHECK(
            vkCreateFramebuffer(
                m_renderDevice->getHandle(),
                &createInfo,
                nullptr,
                &m_framebuffers[ i ] ) );
    }

    createRenderPassObjects();
    createFontAtlas();

    m_pipeline = std::make_unique< vulkan::GraphicsPipeline >(
        m_renderDevice,
        m_renderPass,
        std::vector< VkDescriptorSetLayout > {
            m_renderPassObjects.descriptorSetLayout,
            m_fontAtlas.descriptorSetLayout,
        },
        m_program.get(),
        std::vector< VertexLayout > { VertexP2TC2C4::getLayout() },
        DepthStencilState {
            .depthTestEnable = false,
            .stencilTestEnable = false,
        },
        RasterizationState { .cullMode = CullMode::NONE },
        ColorBlendState {
            .enable = true,
            .srcColorBlendFactor = BlendFactor::SRC_ALPHA,
            .dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = BlendOp::ADD,
            .srcAlphaBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA,
            .dstAlphaBlendFactor = BlendFactor::ZERO,
            .alphaBlendOp = BlendOp::ADD,
        } );
}

void ImGUILayer::clean( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDeviceWaitIdle( m_renderDevice->getHandle() );

    m_pipeline = nullptr;

    destroyFontAtlas();
    destroyRenderPassObjects();

    for ( auto &fb : m_framebuffers ) {
        vkDestroyFramebuffer( m_renderDevice->getHandle(), fb, nullptr );
    }
    m_framebuffers.clear();

    vkDestroyRenderPass( m_renderDevice->getHandle(), m_renderPass, nullptr );
    m_renderPass = VK_NULL_HANDLE;
}

void ImGUILayer::createRenderPassObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    m_renderPassObjects.uniforms = [ & ] {
        auto ubo = std::make_unique< UniformBuffer >( RenderPassObjects::Uniforms {} );
        ubo->getBufferView()->setUsage( BufferView::Usage::DYNAMIC );
        m_renderDevice->bind( ubo.get() );
        return ubo;
    }();

    const auto bindings = std::array< VkDescriptorSetLayoutBinding, 1 > {
        VkDescriptorSetLayoutBinding {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = nullptr,
        },
    };

    auto layoutCreateInfo = VkDescriptorSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = uint32_t( bindings.size() ),
        .pBindings = bindings.data(),
    };

    CRIMILD_VULKAN_CHECK(
        vkCreateDescriptorSetLayout(
            m_renderDevice->getHandle(),
            &layoutCreateInfo,
            nullptr,
            &m_renderPassObjects.descriptorSetLayout ) );

    const auto poolSizes = std::array< VkDescriptorPoolSize, 1 > {
        VkDescriptorPoolSize {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = uint32_t( m_renderDevice->getSwapchainImageCount() ),
        },
    };

    auto poolCreateInfo = VkDescriptorPoolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = uint32_t( poolSizes.size() ),
        .pPoolSizes = poolSizes.data(),
        .maxSets = uint32_t( m_renderDevice->getSwapchainImageCount() ),
    };

    CRIMILD_VULKAN_CHECK(
        vkCreateDescriptorPool(
            m_renderDevice->getHandle(),
            &poolCreateInfo,
            nullptr,
            &m_renderPassObjects.descriptorPool ) );

    m_renderPassObjects.descriptorSets.resize( m_renderDevice->getSwapchainImageCount() );

    std::vector< VkDescriptorSetLayout > layouts( m_renderDevice->getSwapchainImageCount(), m_renderPassObjects.descriptorSetLayout );

    const auto allocInfo = VkDescriptorSetAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = m_renderPassObjects.descriptorPool,
        .descriptorSetCount = uint32_t( layouts.size() ),
        .pSetLayouts = layouts.data(),
    };

    CRIMILD_VULKAN_CHECK(
        vkAllocateDescriptorSets(
            m_renderDevice->getHandle(),
            &allocInfo,
            m_renderPassObjects.descriptorSets.data() ) );

    for ( size_t i = 0; i < m_renderPassObjects.descriptorSets.size(); ++i ) {
        const auto bufferInfo = VkDescriptorBufferInfo {
            .buffer = m_renderDevice->getHandle( m_renderPassObjects.uniforms.get(), i ),
            .offset = 0,
            .range = m_renderPassObjects.uniforms->getBufferView()->getLength(),
        };

        const auto writes = std::array< VkWriteDescriptorSet, 1 > {
            VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = m_renderPassObjects.descriptorSets[ i ],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .pBufferInfo = &bufferInfo,
                .pImageInfo = nullptr,
                .pTexelBufferView = nullptr,
            },
        };

        vkUpdateDescriptorSets( m_renderDevice->getHandle(), writes.size(), writes.data(), 0, nullptr );
    }
}

void ImGUILayer::destroyRenderPassObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    // no need to destroy sets
    m_renderPassObjects.descriptorSets.clear();

    vkDestroyDescriptorSetLayout( m_renderDevice->getHandle(), m_renderPassObjects.descriptorSetLayout, nullptr );
    m_renderPassObjects.descriptorSetLayout = VK_NULL_HANDLE;

    vkDestroyDescriptorPool( m_renderDevice->getHandle(), m_renderPassObjects.descriptorPool, nullptr );
    m_renderPassObjects.descriptorPool = VK_NULL_HANDLE;

    m_renderDevice->unbind( m_renderPassObjects.uniforms.get() );
    m_renderPassObjects.uniforms = nullptr;
}

void ImGUILayer::createFontAtlas( void ) noexcept
{
    auto &io = ImGui::GetIO();

    io.Fonts->AddFontDefault();

    unsigned char *pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32( &pixels, &width, &height );

    m_renderDevice->createImage(
        width,
        height,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        1,
        VK_SAMPLE_COUNT_1_BIT,
        1,
        0,
        m_fontAtlas.image,
        m_fontAtlas.memory,
        pixels );

    m_renderDevice->createImageView(
        m_fontAtlas.image,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_ASPECT_COLOR_BIT,
        m_fontAtlas.imageView );

    auto samplerInfo = VkSamplerCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .mipLodBias = 0,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy = 1,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .minLod = 0,
        .maxLod = 1,
        .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
        .unnormalizedCoordinates = VK_FALSE,
    };

    CRIMILD_VULKAN_CHECK(
        vkCreateSampler(
            m_renderDevice->getHandle(),
            &samplerInfo,
            nullptr,
            &m_fontAtlas.sampler ) );

    const auto bindings = std::array< VkDescriptorSetLayoutBinding, 1 > {
        VkDescriptorSetLayoutBinding {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
        },
    };

    auto layoutCreateInfo = VkDescriptorSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = uint32_t( bindings.size() ),
        .pBindings = bindings.data(),
    };

    CRIMILD_VULKAN_CHECK(
        vkCreateDescriptorSetLayout(
            m_renderDevice->getHandle(),
            &layoutCreateInfo,
            nullptr,
            &m_fontAtlas.descriptorSetLayout ) );

    const auto poolSizes = std::array< VkDescriptorPoolSize, 1 > {
        VkDescriptorPoolSize {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = uint32_t( m_renderDevice->getSwapchainImageCount() ),
        },
    };

    auto poolCreateInfo = VkDescriptorPoolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = uint32_t( poolSizes.size() ),
        .pPoolSizes = poolSizes.data(),
        .maxSets = uint32_t( m_renderDevice->getSwapchainImageCount() ),
    };

    CRIMILD_VULKAN_CHECK(
        vkCreateDescriptorPool(
            m_renderDevice->getHandle(),
            &poolCreateInfo,
            nullptr,
            &m_fontAtlas.descriptorPool ) );

    m_fontAtlas.descriptorSets.resize( m_renderDevice->getSwapchainImageCount() );

    std::vector< VkDescriptorSetLayout > layouts( m_renderDevice->getSwapchainImageCount(), m_fontAtlas.descriptorSetLayout );

    const auto allocInfo = VkDescriptorSetAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = m_fontAtlas.descriptorPool,
        .descriptorSetCount = uint32_t( layouts.size() ),
        .pSetLayouts = layouts.data(),
    };

    CRIMILD_VULKAN_CHECK(
        vkAllocateDescriptorSets(
            m_renderDevice->getHandle(),
            &allocInfo,
            m_fontAtlas.descriptorSets.data() ) );

    for ( size_t i = 0; i < m_fontAtlas.descriptorSets.size(); ++i ) {
        const auto imageInfo = VkDescriptorImageInfo {
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .imageView = m_fontAtlas.imageView,
            .sampler = m_fontAtlas.sampler,
        };

        const auto writes = std::array< VkWriteDescriptorSet, 1 > {
            VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = m_fontAtlas.descriptorSets[ i ],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .pBufferInfo = nullptr,
                .pImageInfo = &imageInfo,
                .pTexelBufferView = nullptr,
            },
        };

        vkUpdateDescriptorSets( m_renderDevice->getHandle(), writes.size(), writes.data(), 0, nullptr );
    }

    io.Fonts->TexID = ( ImTextureID ) m_fontAtlas.descriptorSets.data();
}

void ImGUILayer::destroyFontAtlas( void ) noexcept
{
    m_fontAtlas.descriptorSets.clear();

    vkDestroyDescriptorSetLayout( m_renderDevice->getHandle(), m_fontAtlas.descriptorSetLayout, nullptr );
    m_fontAtlas.descriptorSetLayout = VK_NULL_HANDLE;

    vkDestroyDescriptorPool( m_renderDevice->getHandle(), m_fontAtlas.descriptorPool, nullptr );
    m_fontAtlas.descriptorPool = VK_NULL_HANDLE;

    vkDestroySampler( m_renderDevice->getHandle(), m_fontAtlas.sampler, nullptr );
    m_fontAtlas.sampler = VK_NULL_HANDLE;
    vkDestroyImageView( m_renderDevice->getHandle(), m_fontAtlas.imageView, nullptr );
    m_fontAtlas.imageView = VK_NULL_HANDLE;
    vkDestroyImage( m_renderDevice->getHandle(), m_fontAtlas.image, nullptr );
    m_fontAtlas.image = VK_NULL_HANDLE;
    vkFreeMemory( m_renderDevice->getHandle(), m_fontAtlas.memory, nullptr );
    m_fontAtlas.memory = VK_NULL_HANDLE;
}
