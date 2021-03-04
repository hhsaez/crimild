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

#include "Rendering/Operations/OperationUtils.hpp"

#include "Rendering/Sampler.hpp"
#include "Rendering/Texture.hpp"

using namespace crimild;

SharedPointer< Attachment > crimild::framegraph::useColorAttachment( std::string name, Format format ) noexcept
{
    name = name != "" ? name : "color";

    auto att = crimild::alloc< Attachment >();
    att->setName( name + "/attachment" );
    att->usage = Attachment::Usage::COLOR_ATTACHMENT;
    att->format = format;
    att->imageView->setName( att->getName() + "/imageView" );
    att->imageView->image->setName( att->getName() + "/image" );
    return att;
}

SharedPointer< Attachment > crimild::framegraph::useDepthAttachment( std::string name ) noexcept
{
    name = name != "" ? name : "color";

    auto att = crimild::alloc< Attachment >();
    att->setName( name + "/attachment" );
    att->usage = Attachment::Usage::DEPTH_STENCIL_ATTACHMENT;
    att->format = Format::DEPTH_STENCIL_DEVICE_OPTIMAL;
    att->imageView->setName( att->getName() + "/imageView" );
    att->imageView->image->setName( att->getName() + "/image" );
    return att;
}

SharedPointer< FrameGraphResource > crimild::framegraph::useResource( SharedPointer< FrameGraphOperation > const &op, Size index ) noexcept
{
    return op->getProduct( index );
}

SharedPointer< Texture > crimild::framegraph::withResource( SharedPointer< Texture > const &texture, SharedPointer< FrameGraphResource > const &resource ) noexcept
{
    switch ( resource->getType() ) {
        case FrameGraphResource::Type::IMAGE_VIEW: {
            texture->imageView = crimild::cast_ptr< ImageView >( resource );
            break;
        }
        case FrameGraphResource::Type::IMAGE: {
            texture->imageView = crimild::alloc< ImageView >();
            texture->imageView->image = crimild::cast_ptr< Image >( resource );
            break;
        }
        case FrameGraphResource::Type::ATTACHMENT: {
            auto att = crimild::cast_ptr< Attachment >( resource );
            texture->imageView = att->imageView;
            break;
        }
        default: {
            CRIMILD_LOG_FATAL( "Invalid resource type" );
            exit( -1 );
        }
    }
    texture->sampler = [] {
        auto sampler = crimild::alloc< Sampler >();
        sampler->setMinFilter( Sampler::Filter::NEAREST );
        sampler->setMagFilter( Sampler::Filter::NEAREST );
        return sampler;
    }();

    return texture;
}
