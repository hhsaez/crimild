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

#include "Editor/Panels/NodeInspectorPanel.hpp"

#include "Behaviors/Actions/Rotate.hpp"
#include "Behaviors/BehaviorController.hpp"
#include "Behaviors/Decorators/Repeat.hpp"
#include "Behaviors/withBehavior.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/PathComponent.hpp"
#include "Editor/EditorLayer.hpp"
#include "Foundation/ImGUIUtils.hpp"
#include "Foundation/ObjectFactory.hpp"
#include "Mathematics/Matrix4_inverse.hpp"
#include "Mathematics/get_ptr.hpp"
#include "Mathematics/min.hpp"
#include "Mathematics/swizzle.hpp"
#include "Mathematics/trigonometry.hpp"
#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"
#include "Rendering/Materials/UnlitMaterial.hpp"
#include "Rendering/VulkanImage.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanShadowMap.hpp"
#include "Rendering/VulkanWithRenderDevice.hpp"
#include "SceneGraph/Group.hpp"
#include "SceneGraph/Light.hpp"
#include "SceneGraph/Node.hpp"
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild;

static void pathComponentDetails( components::Path *path ) noexcept
{
    if ( ImGui::CollapsingHeader( path->getClassName(), ImGuiTreeNodeFlags_None ) ) {
        ImGui::Text( "Nodes: %i", path->getNode< Group >()->getNodeCount() );
    }
}

static void materialComponentDetails( MaterialComponent *materials )
{
    auto material = materials->first();
    if ( material == nullptr ) {
        return;
    }

    if ( ImGui::CollapsingHeader( material->getClassName(), ImGuiTreeNodeFlags_None ) ) {
        if ( auto bsdf = dynamic_cast< materials::PrincipledBSDF * >( material ) ) {
            auto albedo = bsdf->getAlbedo();
            ImGui::ColorEdit3( "Albedo", get_ptr( albedo ) );
            bsdf->setAlbedo( albedo );
        } else if ( auto unlit = dynamic_cast< UnlitMaterial * >( material ) ) {
            auto color = rgb( unlit->getColor() );
            ImGui::ColorEdit3( "Color", get_ptr( color ) );
            unlit->setColor( rgba( color ) );
        } else {
            ImGui::Text( "Unknown material class: %s", material->getClassName() );
        }
    }
}

static void behaviorControllerDetails( behaviors::BehaviorController *controller )
{
    if ( controller == nullptr ) {
        return;
    }

    auto context = controller->getContext();

    static bool showBehaviorEditor = false;
    ImGui::SetNextItemOpen( true );
    if ( ImGui::CollapsingHeader( controller->getClassName(), ImGuiTreeNodeFlags_None ) ) {
        if ( ImGui::Button( "Edit Behaviors..." ) ) {
            showBehaviorEditor = true;
        }
    }

    auto targetName = [ & ] {
        std::string name = "N/A";
        context->foreachTarget(
            [ & ]( auto target ) {
                std::stringstream ss;
                ss << ( target->getName().empty() ? target->getClassName() : target->getName() );
                ss << " (" << target->getUniqueID() << ")";
                name = ss.str();
            }
        );
        return name;
    }();

    ImGui::Text( "Target: %s", targetName.c_str() );
    if ( ImGui::BeginDragDropTarget() ) {
        if ( auto payload = ImGui::AcceptDragDropPayload( "DND_NODE" ) ) {
            size_t nodeAddr = *( ( size_t * ) payload->Data );
            Node *target = reinterpret_cast< Node * >( nodeAddr );
            if ( target ) {
                context->removeAllTargets();
                context->addTarget( target );
            }
        }
        ImGui::EndDragDropTarget();
    }

    // TODO: open behavior editor
    // behaviorEditor( showBehaviorEditor, controller );
}

static const std::set< std::string > &getComponentList( void ) noexcept
{
    static const auto components = ObjectFactory::getInstance()->filter(
        std::set< std::string > {
            "crimild::components",
        }
    );
    return components;
}

namespace crimild {

    class NodeInfoSection : public editor::NodeInspectorPanel::Section {
    public:
        virtual void render( Node *node ) noexcept override
        {
            auto enabled = node->isEnabled();
            ImGui::Checkbox( "Enable", &enabled );
            node->setEnabled( enabled );

            ImGui::Text( "Class: %s", node->getClassName() );
            ImGui::Text( "ID: %llu", node->getUniqueID() );

            char nameStr[ 256 ] = { '\0' };
            strcpy( nameStr, node->getName().c_str() );
            ImGui::InputText( "Name", nameStr, 256 );
            node->setName( nameStr );
        }
    };

    class NodeTransformationSection : public editor::NodeInspectorPanel::Section {
    public:
        virtual void render( crimild::Node *node ) noexcept override
        {
            Point3 nodeTranslation;
            Vector3 nodeRotation;
            Vector3 nodeScale;
            ImGuizmo::DecomposeMatrixToComponents( get_ptr( node->getLocal().mat ), get_ptr( nodeTranslation ), get_ptr( nodeRotation ), get_ptr( nodeScale ) );

            bool changed = false;
            changed = changed || ImGui::InputFloat3( "Tr", get_ptr( nodeTranslation ) );
            changed = changed || ImGui::InputFloat3( "Rt", get_ptr( nodeRotation ) );
            changed = changed || ImGui::InputFloat3( "Sc", get_ptr( nodeScale ) );
            if ( changed ) {
                Matrix4 mat;
                ImGuizmo::RecomposeMatrixFromComponents( get_ptr( nodeTranslation ), get_ptr( nodeRotation ), get_ptr( nodeScale ), get_ptr( mat ) );
                node->setLocal( Transformation { mat, inverse( mat ) } );
                node->perform( UpdateWorldState() );
            }
        }
    };

    class NodeComponentsSection : public editor::NodeInspectorPanel::Section {
    public:
        virtual void render( crimild::Node *node ) noexcept override
        {
            ImGui::SetNextItemOpen( true );
            if ( ImGui::CollapsingHeader( "Components", ImGuiTreeNodeFlags_None ) ) {
                if ( ImGui::BeginCombo( "##", "Add Component..." ) ) {
                    const auto &components = getComponentList();
                    for ( const auto &componentClassName : components ) {
                        if ( ImGui::Selectable( componentClassName.c_str(), false ) ) {
                            auto component = std::static_pointer_cast< NodeComponent >( ObjectFactory::getInstance()->build( componentClassName ) );
                            node->attachComponent( component );
                            component->start();
                        }
                    }
                    // TODO: Rename BehaviorController to use ::componets namespace or
                    // improve filtering for ObjectFactory
                    if ( node->getComponent< behaviors::BehaviorController >() == nullptr ) {
                        if ( ImGui::Selectable( "crimild::behaviors::BehaviorController", false ) ) {
                            withBehavior(
                                retain( node ),
                                [] {
                                    auto repeat = crimild::alloc< behaviors::decorators::Repeat >();
                                    repeat->setBehavior( behaviors::actions::rotate( Vector3 { 0, 1, 0 }, 0.5f ) );
                                    return repeat;
                                }()
                            );
                        }
                    }
                    ImGui::EndCombo();
                }

                node->forEachComponent(
                    [ & ]( NodeComponent *cmp ) {
                        if ( cmp->getClassName() == MaterialComponent::__CLASS_NAME ) {
                            ImGui::Separator();
                            materialComponentDetails( static_cast< MaterialComponent * >( cmp ) );
                        } else if ( cmp->getClassName() == behaviors::BehaviorController::__CLASS_NAME ) {
                            ImGui::Separator();
                            behaviorControllerDetails( static_cast< behaviors::BehaviorController * >( cmp ) );
                        } else if ( cmp->getClassName() == components::Path::__CLASS_NAME ) {
                            ImGui::Separator();
                            pathComponentDetails( static_cast< components::Path * >( cmp ) );
                        }
                    }
                );
            }
        }
    };

    class LightPropertiesSection
        : public editor::NodeInspectorPanel::Section,
          public vulkan::WithConstRenderDevice {
    public:
        LightPropertiesSection( const vulkan::RenderDevice *renderDevice )
            : vulkan::WithConstRenderDevice( renderDevice )
        {
            // no-op
        }

        virtual ~LightPropertiesSection( void ) noexcept
        {
            for ( auto &layer : m_shadowMapLayers ) {
                getRenderDevice()->destroyDescriptorPool( layer.descriptorPool );
                getRenderDevice()->destroyDescriptorSetLayout( layer.descriptorSetLayout );
                for ( auto &imageView : layer.imageViews ) {
                    vkDestroyImageView( getRenderDevice()->getHandle(), imageView, nullptr );
                }
            }
            m_shadowMapLayers.clear();
        }

        virtual void render( crimild::Node *node ) noexcept override
        {
            auto light = dynamic_cast< Light * >( node );
            if ( light == nullptr ) {
                return;
            }

            ImGui::SetNextItemOpen( true );
            if ( ImGui::CollapsingHeader( "Light", ImGuiTreeNodeFlags_None ) ) {
                ImGui::Text(
                    "Type: %s",
                    [ & ] {
                        switch ( light->getType() ) {
                            case Light::Type::AMBIENT:
                                return "Ambient";
                            case Light::Type::DIRECTIONAL:
                                return "Directional";
                            case Light::Type::POINT:
                                return "Point";
                            case Light::Type::SPOT:
                                return "Spot";
                        }
                    }()
                );

                auto energy = light->getEnergy();
                ImGui::InputFloat( "Energy", &energy );
                light->setEnergy( energy );

                auto radius = light->getRadius();
                ImGui::InputFloat( "Radius", &radius );
                light->setRadius( radius );

                auto color = light->getColor();
                ImGui::ColorEdit3( "Color", get_ptr( color ) );
                light->setColor( color );

                if ( light->getType() == Light::Type::SPOT ) {
                    auto innerCutoff = degrees( light->getInnerCutoff() );
                    ImGui::SliderFloat( "Inner Cutoff", &innerCutoff, 0, 180 );
                    light->setInnerCutoff( radians( innerCutoff ) );

                    auto outerCutoff = degrees( light->getOuterCutoff() );
                    ImGui::SliderFloat( "Outer Cutoff", &outerCutoff, 0, 180 );
                    light->setOuterCutoff( radians( outerCutoff ) );
                }

                auto castShadows = light->castShadows();
                ImGui::Checkbox( "Cast Shadows", &castShadows );
                light->setCastShadows( castShadows );

                if ( castShadows ) {
                    const auto *shadowMap = getRenderDevice()->getShadowMap( light );
                    if ( shadowMap != nullptr ) {
                        renderShadowMap( shadowMap );
                    } else {
                        ImGui::Text( "No shadow map available" );
                    }
                }
            }
        }

    private:
        void renderShadowMap( const vulkan::ShadowMap *shadowMap ) noexcept
        {
            if ( m_shadowMapLayers.empty() ) {
                configureShadowMapLayers( shadowMap );
            }

            // All shadow maps images will be rendered as squares.
            ImVec2 imageSize = ImGui::GetContentRegionAvail();
            imageSize.y = imageSize.x;

            for ( uint32_t i = 0; i < m_shadowMapLayers.size(); i++ ) {
                ImTextureID tex_id = ( ImTextureID ) ( void * ) m_shadowMapLayers[ i ].descriptorSets.data();
                ImVec2 uv_min = ImVec2( 0.0f, 0.0f );                 // Top-left
                ImVec2 uv_max = ImVec2( 1.0f, 1.0f );                 // Lower-right
                ImVec4 tint_col = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );   // No tint
                ImVec4 border_col = ImVec4( 1.0f, 1.0f, 1.0f, 0.0f ); // 50% opaque white
                ImGui::Image( tex_id, imageSize, uv_min, uv_max, tint_col, border_col );
            }
        }

        void configureShadowMapLayers( const vulkan::ShadowMap *shadowMap ) noexcept
        {
            m_shadowMapLayers.resize( shadowMap->imageLayerCount );

            for ( uint32_t layerId = 0; layerId < m_shadowMapLayers.size(); ++layerId ) {
                auto &imageViews = m_shadowMapLayers[ layerId ].imageViews;

                // Create image views for each layer
                imageViews.resize( getRenderDevice()->getInFlightFrameCount(), VK_NULL_HANDLE );
                for ( uint32_t viewId = 0; viewId < imageViews.size(); viewId++ ) {
                    getRenderDevice()->createImageView(
                        *shadowMap->images[ viewId ],
                        shadowMap->imageFormat,
                        shadowMap->imageAspect,
                        layerId,
                        imageViews[ viewId ]
                    );
                }

                auto &descriptorSets = m_shadowMapLayers[ layerId ].descriptorSets;
                auto &descriptorSetLayout = m_shadowMapLayers[ layerId ].descriptorSetLayout;
                auto &descriptorPool = m_shadowMapLayers[ layerId ].descriptorPool;

                descriptorSets.resize( getRenderDevice()->getInFlightFrameCount(), VK_NULL_HANDLE );

                // Take this outside?
                getRenderDevice()->createDescriptorSetLayout(
                    {
                        VkDescriptorSetLayoutBinding {
                            .binding = 0,
                            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                            .descriptorCount = 1,
                            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                            .pImmutableSamplers = nullptr,
                        },
                    },
                    descriptorSetLayout,
                    "LightProperties/ShadowMap/DescriptorSetLayout"
                );

                getRenderDevice()->createDescriptorPool(
                    {
                        VkDescriptorPoolSize {
                            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                            .descriptorCount = static_cast< uint32_t >( descriptorSets.size() ),
                        },
                    },
                    0,
                    descriptorPool,
                    "LightProperties/ShadowMap/DescriptorPool"
                );

                std::vector< VkDescriptorSetLayout > layouts( descriptorSets.size(), descriptorSetLayout );

                const auto allocInfo = VkDescriptorSetAllocateInfo {
                    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                    .descriptorPool = descriptorPool,
                    .descriptorSetCount = static_cast< uint32_t >( layouts.size() ),
                    .pSetLayouts = layouts.data(),
                };

                CRIMILD_VULKAN_CHECK(
                    vkAllocateDescriptorSets(
                        getRenderDevice()->getHandle(),
                        &allocInfo,
                        descriptorSets.data()
                    )
                );

                for ( size_t i = 0; i < descriptorSets.size(); ++i ) {
                    const auto imageInfo = VkDescriptorImageInfo {
                        .sampler = shadowMap->sampler,
                        .imageView = imageViews[ i ],
                        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    };

                    const auto writes = std::array< VkWriteDescriptorSet, 1 > {
                        VkWriteDescriptorSet {
                            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                            .dstSet = descriptorSets[ i ],
                            .dstBinding = 0,
                            .dstArrayElement = 0,
                            .descriptorCount = 1,
                            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                            .pImageInfo = &imageInfo,
                            .pBufferInfo = nullptr,
                            .pTexelBufferView = nullptr,
                        },
                    };

                    vkUpdateDescriptorSets(
                        getRenderDevice()->getHandle(),
                        writes.size(),
                        writes.data(),
                        0,
                        nullptr
                    );
                }
            }
        }

    private:
        struct ShadowMapLayer {
            VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
            VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
            std::vector< VkImageView > imageViews;
            std::vector< VkDescriptorSet > descriptorSets;
        };

        std::vector< ShadowMapLayer > m_shadowMapLayers;
    };

}

void editor::NodeInspectorPanel::render( void ) noexcept
{
    bool open = true;
    ImGui::Begin( "Inspector", &open, 0 );

    if ( open ) {
        auto editor = EditorLayer::getInstance();

        auto node = editor->getSelectedNode();
        if ( m_selectedNode != node ) {
            configure( node );
        }

        if ( !m_sections.empty() ) {
            // lightPropertiesSection( node, getRenderDevice() );
            // nodeComponentsSection( node );
            for ( auto &section : m_sections ) {
                section->render( node );
            }
        } else {
            ImGui::Text( "No node selected" );
        }
    }

    ImGui::End();

    if ( !open ) {
        configure( nullptr );
        removeFromParent();
        return;
    }
}

void editor::NodeInspectorPanel::configure( crimild::Node *node ) noexcept
{
    if ( m_selectedNode != node ) {
        m_sections.clear();
        m_selectedNode = node;
    }

    if ( m_sections.empty() ) {
        m_sections.push_back( std::make_unique< NodeInfoSection >() );
        m_sections.push_back( std::make_unique< NodeTransformationSection >() );

        if ( auto light = dynamic_cast< Light * >( node ) ) {
            m_sections.push_back( std::make_unique< LightPropertiesSection >( getRenderDevice() ) );
        }

        m_sections.push_back( std::make_unique< NodeComponentsSection >() );
    }
}
