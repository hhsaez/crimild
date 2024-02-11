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

#ifndef CRIMILD_EDITOR_VIEWS_WINDOWS_WINDOW_
#define CRIMILD_EDITOR_VIEWS_WINDOWS_WINDOW_

#include "Foundation/ImGuiUtils.hpp"
#include "Views/View.hpp"

namespace crimild::editor {

    class Window : public View {
    protected:
        Window( std::string_view name ) noexcept;

    public:
        virtual ~Window( void ) noexcept = default;

        void setActive( bool active ) noexcept { m_isOpen = active; }
        virtual bool isActive( void ) const noexcept override { return isVisible() && m_isOpen; }

        void draw( void ) noexcept final;

    protected:
        inline ImGuiWindowFlags getFlags( void ) const noexcept { return m_flags; }
        inline void setFlags( ImGuiWindowFlags flags ) noexcept { m_flags = flags; }

        inline ImVec2 getMinSize( void ) const noexcept { return m_minSize; }
        inline ImVec2 getMaxSize( void ) const noexcept { return m_maxSize; }

    private:
        ImGuiWindowFlags m_flags = ImGuiWindowFlags_None;
        bool m_isOpen = true;
        ImVec2 m_minSize = { 300, 400 };
        ImVec2 m_maxSize = { FLT_MAX, FLT_MAX };
        std::string m_windowName;

    public:
        virtual void encode( coding::Encoder &encoder ) noexcept override;
        virtual void decode( coding::Decoder &decoder ) noexcept override;
    };

}

#endif
