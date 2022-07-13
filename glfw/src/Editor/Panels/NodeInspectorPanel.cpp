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
#include "Editor/EditorLayer.hpp"
#include "Foundation/ImGUIUtils.hpp"
#include "Mathematics/Matrix4_inverse.hpp"
#include "Mathematics/get_ptr.hpp"
#include "Mathematics/swizzle.hpp"
#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"
#include "Rendering/Materials/UnlitMaterial.hpp"
#include "SceneGraph/Light.hpp"
#include "SceneGraph/Node.hpp"
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild;

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

static void nodeComponentsSection( Node *node )
{
    ImGui::SetNextItemOpen( true );
    if ( ImGui::CollapsingHeader( "Components", ImGuiTreeNodeFlags_None ) ) {
        node->forEachComponent(
            [ & ]( NodeComponent *cmp ) {
                if ( cmp->getClassName() == MaterialComponent::__CLASS_NAME ) {
                    materialComponentDetails( static_cast< MaterialComponent * >( cmp ) );
                } else if ( cmp->getClassName() == behaviors::BehaviorController::__CLASS_NAME ) {
                    behaviorControllerDetails( static_cast< behaviors::BehaviorController * >( cmp ) );
                }
            }
        );

        if ( node->getComponent< behaviors::BehaviorController >() == nullptr ) {
            if ( ImGui::Button( "Add Behaviors..." ) ) {
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
    }
}

static void lightPropertiesSection( Node *node )
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

        auto color = rgb( light->getColor() );
        ImGui::ColorEdit3( "Color", get_ptr( color ) );
        light->setColor( rgba( color ) );

        auto castShadows = light->castShadows();
        ImGui::Checkbox( "Cast Shadows", &castShadows );
        light->setCastShadows( castShadows );
    }
}

void NodeInspectorPanel::render( void ) noexcept
{
    // Allow opening multiple panels with the same name
    std::stringstream ss;
    ss << "Node Inspector##" << ( size_t ) this;

    bool open = true;
    ImGui::SetNextWindowPos( ImVec2( m_position.x, m_position.y ), ImGuiCond_FirstUseEver );
    ImGui::SetNextWindowSize( ImVec2( m_extent.width, m_extent.height ), ImGuiCond_FirstUseEver );

    if ( ImGui::Begin( ss.str().c_str(), &open, ImGuiWindowFlags_NoCollapse ) ) {
        auto editor = EditorLayer::getInstance();

        auto node = editor->getSelectedNode();
        if ( node != nullptr ) {
            ImGui::Text( "Class: %s", node->getClassName() );
            ImGui::Text( "ID: %llu", node->getUniqueID() );

            char nameStr[ 256 ] = { '\0' };
            strcpy( nameStr, node->getName().c_str() );
            ImGui::InputText( "Name", nameStr, 256 );
            node->setName( nameStr );

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

            lightPropertiesSection( node );
            nodeComponentsSection( node );
        } else {
            ImGui::Text( "No node selected" );
        }
    }
    ImGui::End();

    if ( !open ) {
        detachFromParent();
        return;
    }

    Layer::render();
}
