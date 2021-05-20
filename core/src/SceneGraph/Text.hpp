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

#ifndef CRIMILD_CORE_SCENE_GRAPH_TEXT_
#define CRIMILD_CORE_SCENE_GRAPH_TEXT_

#include "Geometry.hpp"
#include "Group.hpp"
#include "Mathematics/ColorRGBA.hpp"
#include "Rendering/Image.hpp"
#include "Rendering/Material.hpp"

namespace crimild {

    class Font;

    class Text : public Group {
        CRIMILD_IMPLEMENT_RTTI( crimild::Text )

    public:
        enum class HorizontalAlignment {
            LEFT,
            RIGHT,
            CENTER
        };

    public:
        Text( void );
        virtual ~Text( void );

        std::string getText( void ) const { return _text; }
        void setText( std::string text );

        float getSize( void ) const { return _size; }
        void setSize( float size );

        Font *getFont( void ) { return crimild::get_ptr( _font ); }
        void setFont( Font *font );
        void setFont( SharedPointer< Font > const &font );

        const ColorRGBA &getTextColor( void ) const { return _material->getDiffuse(); }
        void setTextColor( const ColorRGBA &color ) { _material->setDiffuse( color ); }

        void setHorizontalAlignment( HorizontalAlignment alignment );
        HorizontalAlignment getHorizontalAlignment( void ) const { return _horizontalAlignment; }

        //bool isDepthTestEnabled( void ) const { return _material->getDepthState()->isEnabled(); }
        //void setDepthTestEnabled( bool enabled ) { _material->getDepthState()->setEnabled( enabled ); }

        // internal use only
        Geometry *getGeometry( void ) { return crimild::get_ptr( _geometry ); }

    public:
        virtual void accept( NodeVisitor &visitor ) override;

    private:
        void updatePrimitive( void );

        std::string _text;
        float _size;
        SharedPointer< Font > _font;
        SharedPointer< Geometry > _geometry;
        SharedPointer< Primitive > _primitive;
        SharedPointer< Material > _material;
        HorizontalAlignment _horizontalAlignment = HorizontalAlignment::LEFT;

        /**
		   \name Coding support
		*/
        //@{

    public:
        virtual void encode( coding::Encoder &encoder ) override;
        virtual void decode( coding::Decoder &decoder ) override;

        //@}
    };

}

#endif
