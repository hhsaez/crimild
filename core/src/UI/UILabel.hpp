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

#ifndef CRIMILD_UI_LABEL_
#define CRIMILD_UI_LABEL_

#include "Components/NodeComponent.hpp"
#include "Mathematics/Vector.hpp"

namespace crimild {

    class Text;

    namespace ui {

        class UIFrame;

        class UILabel : public NodeComponent {
            CRIMILD_IMPLEMENT_RTTI( crimild::ui::UILabel )

        public:
            enum class TextHorizontalAlignment {
                Left,
                Center,
                Right,
            };

            enum class TextVerticalAlignment {
                Top,
                Center,
                Bottom,
            };

        public:
            UILabel( void );
            UILabel( std::string text, const RGBAColorf &color = RGBAColorf::ONE );
            ~UILabel( void ) = default;

            virtual void onAttach( void ) override;
            virtual void start( void ) override;
            virtual void update( const Clock & ) override;

            void setText( std::string text );

            void setTextHorizontalAlignment( TextHorizontalAlignment value ) { _textHorizontalAlignment = value; }
            TextHorizontalAlignment getTextHorizontalAlignment( void ) const { return _textHorizontalAlignment; }

            void setTextVerticalAlignment( TextVerticalAlignment value ) { _textVerticalAlignment = value; }
            TextVerticalAlignment getTextVerticalAlignment( void ) const { return _textVerticalAlignment; }

            void setTextColor( const RGBAColorf &value ) { _textColor = value; }
            const RGBAColorf &getTextColor( void ) const { return _textColor; }

            void setTextSize( crimild::Real32 value ) { _textSize = value; }
            crimild::Real32 getTextSize( void ) const { return _textSize; }

        private:
            UIFrame *_frame = nullptr;
            SharedPointer< Text > _text = nullptr;
            TextHorizontalAlignment _textHorizontalAlignment = TextHorizontalAlignment::Left;
            TextVerticalAlignment _textVerticalAlignment = TextVerticalAlignment::Center;
            RGBAColorf _textColor;
            crimild::Real32 _textSize = 10.0f;

        public:
            void decode( coding::Decoder &decoder ) override;
        };

    }

}

#endif
