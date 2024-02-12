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

#include "Views/Windows/LogWindow/LogWindow.hpp"

using namespace crimild::editor;

LogWindow::LogWindow( void ) noexcept
    : Window( "Log" )
{
    // no-op
}

void LogWindow::drawContent( void ) noexcept
{
    auto logger = LogWindow::OutputHandler::getInstance();
    if ( !logger ) {
        ImGui::Text( "No output handler available" );
        return;
    }

    auto &buffer = logger->getBuffer();
    auto &lineOffsets = logger->getLineOffsets();
    auto &colors = logger->getColors();

    bool autoScroll = true;

    if ( ImGui::BeginChild( "scrolling", ImVec2( 0, 0 ), false, ImGuiWindowFlags_HorizontalScrollbar ) ) {
        ImGuiListClipper clipper;
        clipper.Begin( lineOffsets.size() );
        while ( clipper.Step() ) {
            for ( int line = clipper.DisplayStart; line < clipper.DisplayEnd; ++line ) {
                const auto color = colors[ line ];
                ImGui::PushStyleColor( ImGuiCol_Text, color );

                const char *start = buffer.begin() + lineOffsets[ line ];
                const char *end =
                    ( line + 1 < lineOffsets.size() )
                        ? ( buffer.begin() + lineOffsets[ line + 1 ] - 1 )
                        : buffer.end();
                ImGui::TextUnformatted( start, end );

                // Unfortunatelly, ImGuiListClipper does not work correct when elements have varying sizes, like
                // lines in the log. As a work around, I'm displaying a tooltip with a more readable log description
                if ( ImGui::IsItemHovered() ) {
                    ImGui::PushStyleColor( ImGuiCol_PopupBg, IM_COL32( 0, 0, 0, 255 ) );
                    ImGui::PushStyleColor( ImGuiCol_Border, IM_COL32( 255, 255, 255, 255 ) );

                    ImGui::BeginTooltip();

                    ImGui::PushTextWrapPos( ImGui::GetCursorPos().x + 1000.0f );

                    const std::string text( start, end );
                    ImGui::Text( "%s", text.c_str() );

                    ImGui::PopTextWrapPos();

                    ImGui::EndTooltip();

                    ImGui::PopStyleColor();
                    ImGui::PopStyleColor();
                }

                ImGui::PopStyleColor();
            }
        }
        clipper.End();

        // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
        // Using a scrollbar or mouse-wheel will take away from the bottom edge.
        if ( autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY() ) {
            ImGui::SetScrollHereY( 1.0f );
        }
    }
    ImGui::EndChild();
}
