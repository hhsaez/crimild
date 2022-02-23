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

#include "Editor/EditorLayer.hpp"

#include "Foundation/Log.hpp"
#include "Mathematics/Vector4_constants.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/Vertex.hpp"
#include "Rendering/VulkanGraphicsPipeline.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Simulation/Event.hpp"
#include "Simulation/Settings.hpp"
#include "imgui.h"

#include <array>

using namespace crimild;
using namespace crimild::vulkan;

EditorLayer::EditorLayer( RenderDevice *renderDevice ) noexcept
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

                                layout ( set = 0, binding = 1 ) uniform sampler2D uTexture;

                                layout ( location = 0 ) out vec4 FragColor;

                                void main()
                                {
                                    vec2 uv = vTexCoord;
                                    FragColor = vColor * texture( uTexture, uv );
                                }
                        )" ) } );
              return program;
          }() )
{
    CRIMILD_LOG_TRACE();

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    auto &io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    updateDisplaySize();
    init();
}

EditorLayer::~EditorLayer( void ) noexcept
{
    clean();
    ImGui::DestroyContext();
}

Event EditorLayer::handle( const Event &e ) noexcept
{
    switch ( e.type ) {
        case Event::Type::WINDOW_RESIZE: {
            clean();
            updateDisplaySize();
            init();
            break;
        }

        default:
            break;
    }

    return e;
}

void EditorLayer::render( void ) noexcept
{
    // TODO: move this to event handling?
    renderUI();

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

    vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );

    // TODO: render

    vkCmdEndRenderPass( commandBuffer );
}

void EditorLayer::renderUI( void ) noexcept
{
    auto &io = ImGui::GetIO();

    // auto clock = Simulation::getInstance()->getSimulationClock();
    // io.DeltaTime = Numericf::max( 1.0f / 60.0f, clock.getDeltaTime() );

    if ( !io.Fonts->IsBuilt() ) {
        CRIMILD_LOG_ERROR( "Font atlas is not built!" );
        return;
    }

    // This should not be necessary
    updateDisplaySize();

    ImGui::NewFrame();

    // if ( m_frameCallback != nullptr ) {
    //     m_frameCallback();
    // }

    static bool open = true;
    ImGui::ShowDemoWindow( &open );

    ImGui::Render();
}

void EditorLayer::updateDisplaySize( void ) const noexcept
{
    auto &io = ImGui::GetIO();
    auto width = Settings::getInstance()->get< float >( "video.width", 0 );
    auto height = Settings::getInstance()->get< float >( "video.height", 1 );
    auto framebufferScale = Settings::getInstance()->get< float >( "video.framebufferScale", 1 );
    auto displaySizeScale = 2.0f / framebufferScale;
    io.DisplaySize = ImVec2( width * displaySizeScale, height * displaySizeScale );
    io.DisplayFramebufferScale = ImVec2( 1, 1 );
}

void EditorLayer::init( void ) noexcept
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

    createFontAtlas();
    createRenderPassObjects();

    m_pipeline = std::make_unique< vulkan::GraphicsPipeline >(
        m_renderDevice,
        m_renderPass,
        std::vector< VkDescriptorSetLayout > {
            m_renderPassObjects.descriptorSetLayout,
        },
        m_program.get(),
        std::vector< VertexLayout > { VertexP2TC2C4::getLayout() } );
}

void EditorLayer::clean( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDeviceWaitIdle( m_renderDevice->getHandle() );

    m_pipeline = nullptr;

    destroyRenderPassObjects();
    destroyFontAtlas();

    for ( auto &fb : m_framebuffers ) {
        vkDestroyFramebuffer( m_renderDevice->getHandle(), fb, nullptr );
    }
    m_framebuffers.clear();

    vkDestroyRenderPass( m_renderDevice->getHandle(), m_renderPass, nullptr );
    m_renderPass = VK_NULL_HANDLE;
}

void EditorLayer::createRenderPassObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    m_renderPassObjects.uniforms = [ & ] {
        struct RenderPassUniforms {
            Vector4 scale = Vector4::Constants::ONE;
            Vector4 translate = Vector4::Constants::ZERO;
        };

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

        auto ubo = std::make_unique< UniformBuffer >(
            RenderPassUniforms {
                .scale = scale,
                .translate = translate,
            } );
        // ubo->getBufferView()->setUsage( BufferView::Usage::DYNAMIC );
        m_renderDevice->bind( ubo.get() );
        return ubo;
    }();

    const auto poolSizes = std::array< VkDescriptorPoolSize, 2 > {
        VkDescriptorPoolSize {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = uint32_t( m_renderDevice->getSwapchainImageCount() ),
        },
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

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorPool( m_renderDevice->getHandle(), &poolCreateInfo, nullptr, &m_renderPassObjects.descriptorPool ) );

    const auto bindings = std::array< VkDescriptorSetLayoutBinding, 2 > {
        VkDescriptorSetLayoutBinding {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = nullptr,
        },
        VkDescriptorSetLayoutBinding {
            .binding = 1,
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

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorSetLayout( m_renderDevice->getHandle(), &layoutCreateInfo, nullptr, &m_renderPassObjects.descriptorSetLayout ) );

    std::vector< VkDescriptorSetLayout > layouts( m_renderDevice->getSwapchainImageCount(), m_renderPassObjects.descriptorSetLayout );

    const auto allocInfo = VkDescriptorSetAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = m_renderPassObjects.descriptorPool,
        .descriptorSetCount = uint32_t( layouts.size() ),
        .pSetLayouts = layouts.data(),
    };

    m_renderPassObjects.descriptorSets.resize( m_renderDevice->getSwapchainImageCount() );
    CRIMILD_VULKAN_CHECK( vkAllocateDescriptorSets( m_renderDevice->getHandle(), &allocInfo, m_renderPassObjects.descriptorSets.data() ) );

    auto imageView = m_renderDevice->bind( m_fontAtlas->imageView.get() );
    auto sampler = m_renderDevice->bind( m_fontAtlas->sampler.get() );

    for ( size_t i = 0; i < m_renderPassObjects.descriptorSets.size(); ++i ) {
        const auto bufferInfo = VkDescriptorBufferInfo {
            .buffer = m_renderDevice->getHandle( m_renderPassObjects.uniforms.get(), i ),
            .offset = 0,
            .range = m_renderPassObjects.uniforms->getBufferView()->getLength(),
        };

        const auto imageInfo = VkDescriptorImageInfo {
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .imageView = imageView,
            .sampler = sampler,
        };

        const auto writes = std::array< VkWriteDescriptorSet, 2 > {
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
            VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = m_renderPassObjects.descriptorSets[ i ],
                .dstBinding = 1,
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
}

void EditorLayer::destroyRenderPassObjects( void ) noexcept
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

void EditorLayer::createFontAtlas( void ) noexcept
{
    auto &io = ImGui::GetIO();

    io.Fonts->AddFontDefault();

    unsigned char *pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32( &pixels, &width, &height );

    auto texture = std::make_unique< Texture >();
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

    m_fontAtlas = std::move( texture );

    m_renderDevice->bind( m_fontAtlas->imageView.get() );
    m_renderDevice->bind( m_fontAtlas->sampler.get() );
}

void EditorLayer::destroyFontAtlas( void ) noexcept
{
    m_renderDevice->unbind( m_fontAtlas->imageView.get() );
    m_renderDevice->unbind( m_fontAtlas->sampler.get() );

    m_fontAtlas = nullptr;
}
