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

#ifndef CRIMILD_EDITOR_VIEWS_VIEW_
#define CRIMILD_EDITOR_VIEWS_VIEW_

#include "Coding/Codable.hpp"
#include "Foundation/Named.hpp"
#include "Foundation/RTTI.hpp"

#include <memory>

namespace crimild::editor {

    class Layout;

    class View
        : public Named,
          public coding::Codable {
    protected:
        View( std::string_view name ) noexcept;

    public:
        virtual ~View( void ) noexcept = default;

        inline void setLayout( std::shared_ptr< Layout > const &layout ) noexcept { m_layout = layout; }
        inline std::shared_ptr< Layout > getLayout( void ) noexcept { return m_layout.lock(); }

        /**
         * @brief Indicates if a view is opened and needs to be drawn
         *
         * If the view is not opened, it will be removed from the layout (and memory)
         * automatically.
         */
        [[nodiscard]] inline bool isOpen( void ) const noexcept { return m_open; }

        /**
         * @brief Draws the view
         *
         * @remarks This function is intented to be overriden by the different view
         * types (i.e. Window, Modal, etc.), which are direct subclasses of View. For
         * rendering the view's content, override the drawContent() function instead.
         */
        virtual void draw( void ) noexcept = 0;

        /**
         * @brief Draw the actual content for this view
         */
        virtual void drawContent( void ) noexcept = 0;

    protected:
        /**
         * @brief Get access to the internal view state
         *
         * Some views may need to set the open state during rendering (i.e. ImGui::Begin())
         * and this helper function makes things easier.
         */
        [[nodiscard]] inline bool &getOpenState( void ) noexcept { return m_open; }

    private:
        std::weak_ptr< Layout > m_layout;
        bool m_open = true;
    };

}

#endif
