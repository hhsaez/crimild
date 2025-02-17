/*
 * Copyright (c) 2002-present, H. Hernán Saez
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

#ifndef CRIMILD_UI_CANVAS_
#define CRIMILD_UI_CANVAS_

#include "Components/NodeComponent.hpp"
#include "Crimild_Mathematics.hpp"

namespace crimild {

    namespace ui {

        class UICanvas : public NodeComponent {
            CRIMILD_IMPLEMENT_RTTI( crimild::ui::UICanvas )

        public:
            enum class RenderSpace {
                WORLD,
                CAMERA,
            };

        public:
            UICanvas( void ) = default;
            UICanvas( crimild::Int32 width, crimild::Int32 height );
            ~UICanvas( void ) = default;

            virtual void onAttach( void ) override;

            virtual void update( const Clock & ) override;

            void setRenderSpace( RenderSpace value ) { _renderSpace = value; }
            RenderSpace getRenderSpace( void ) const { return _renderSpace; }

        private:
            Vector2i _size;
            Vector2i _safeArea;
            RenderSpace _renderSpace = RenderSpace::CAMERA;

        public:
            void decode( coding::Decoder &decoder ) override;
        };

    }

}

#endif
