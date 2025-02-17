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

#ifndef CRIMILD_CORE_RENDERING_IMAGE_VIEW_
#define CRIMILD_CORE_RENDERING_IMAGE_VIEW_

#include "Crimild_Coding.hpp"
#include "Crimild_Foundation.hpp"
#include "Rendering/Format.hpp"
#include "Rendering/FrameGraphResource.hpp"
#include "Rendering/RenderResource.hpp"

namespace crimild {

    class Image;

    class ImageView
        : public RenderResourceImpl< ImageView >,
          public NamedObject,
          public coding::Codable,
          public FrameGraphResource {
        CRIMILD_IMPLEMENT_RTTI( crimild::ImageView )

    public:
        enum class Type {
            UNDEFINED, //< Default.
            IMAGE_VIEW_1D,
            IMAGE_VIEW_2D,
            IMAGE_VIEW_3D,
            IMAGE_VIEW_CUBE,
            IMAGE_VIEW_SWAPCHAIN, //< An image from the swapchain
        };

    public:
        inline FrameGraphResource::Type getType( void ) const noexcept override { return FrameGraphResource::Type::IMAGE_VIEW; }

        Type type = Type::UNDEFINED; //< Default to same type as image
        SharedPointer< Image > image;
        Format format = Format::UNDEFINED; //< Fallbacks to same format as image
        crimild::UInt32 mipLevels = 0;     //< Fallbacks to same mipLevels as image
        crimild::UInt32 layerCount = 0;    //< Fallbacks to same layer count as image

        virtual void setWrittenBy( FrameGraphOperation *op ) noexcept override;
        virtual void setReadBy( FrameGraphOperation *op ) noexcept override;

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
