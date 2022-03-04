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

#include "Editor/Menus/sceneMenu.hpp"

#include "imgui.h"

void crimild::editor::sceneMenu( void ) noexcept
{
    if ( ImGui::BeginMenu( "Scene" ) ) {
        if ( ImGui::MenuItem( "Add Empty" ) ) {
            // addEmptyNode();
        }

        ImGui::Separator();

        if ( ImGui::BeginMenu( "Geometry" ) ) {
            if ( ImGui::MenuItem( "Plane" ) ) {
                // addGeometry( BoxPrimitive::UNIT_BOX );
            }

            if ( ImGui::MenuItem( "Box" ) ) {
                // addGeometry( BoxPrimitive::UNIT_BOX );
            }

            if ( ImGui::MenuItem( "Sphere" ) ) {
                // addGeometry( SpherePrimitive::UNIT_SPHERE );
            }

            ImGui::EndMenu();
        }

        if ( ImGui::BeginMenu( "Light" ) ) {
            if ( ImGui::MenuItem( "Directional" ) ) {
                // TODO
            }
            if ( ImGui::MenuItem( "Point" ) ) {
                // TODO
            }
            if ( ImGui::MenuItem( "Spot" ) ) {
                // TODO
            }

            ImGui::EndMenu();
        }

        ImGui::Separator();

        if ( ImGui::MenuItem( "Add Camera" ) ) {
            // TODO
        }

        ImGui::EndMenu();
    }
}
