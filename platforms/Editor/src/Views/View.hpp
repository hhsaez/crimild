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

namespace crimild::editor {

    class View
        : public Named,
          public coding::Codable {
    protected:
        View( std::string_view name ) noexcept;

    public:
        virtual ~View( void ) noexcept = default;

        /**
         * @brief Indicates if this view needs to be processed
         *
         * Views may be alive, but they are not actually active unless the current layout
         * does requires it explicitely. If the view is not active, it will not be executed nor
         * rendered.
         */
        [[nodiscard]] virtual bool isActive( void ) const noexcept;

        /**
         * @brief Indicates if the view is visible and needs to be rendered
         *
         * Even if the view is active, it might be possible for it to not be visible. If
         * so, there is no need to render it.
         */
        [[nodiscard]] virtual bool isVisible( void ) const noexcept;

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
    };

    class Modal : public View {
    public:
        void draw( void ) noexcept final
        {
            // TODO
        }
    };

}

#endif
