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

#ifndef CRIMILD_EDITOR_LAYOUT_
#define CRIMILD_EDITOR_LAYOUT_

#include "Coding/Codable.hpp"
#include "Foundation/Named.hpp"
#include "Rendering/Extent.hpp"

#include <memory>
#include <string>
#include <vector>

namespace crimild::editor {

    class View;

    class Layout
        : public Named,
          public coding::Codable {
        CRIMILD_IMPLEMENT_RTTI( crimild::editor::Layout )

    public:
        Layout( void ) = default;
        Layout( std::string_view name, std::string_view imGuiLayout ) noexcept;
        ~Layout( void ) = default;

        inline const Extent2D &getExtent( void ) const noexcept { return m_extent; }

        void addView( std::shared_ptr< View > const &view ) noexcept;

        void makeCurrent( void ) noexcept;

        void draw( void ) noexcept;

    private:
        std::vector< std::shared_ptr< View > > m_views;

        Extent2D m_extent = { .width = 1024.0f, .height = 768.0f };

        /**
         * @brief Keeps track of the ImGui context, usually saved in imgui.ini
         *
         * Loading/saving ImGui Context is now done manually, instead of relying
         * on ImGui's own methods.
         */
        std::string m_imGuiLayout;

    public:
        virtual void encode( coding::Encoder &encoder ) noexcept override;
        virtual void decode( coding::Decoder &decoder ) noexcept override;
    };

}

#endif
