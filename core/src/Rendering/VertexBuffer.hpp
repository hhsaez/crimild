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

#ifndef CRIMILD_RENDERING_VERTEX_BUFFER_
#define CRIMILD_RENDERING_VERTEX_BUFFER_

#include "Rendering/Buffer.hpp"
#include "Mathematics/Vector.hpp"

namespace crimild {

    template< typename T >
    using VertexBuffer = BufferImpl< T, Buffer::Usage::VERTEX_BUFFER >;

    struct VertexInputAttributeDescription {
        enum class Format {
            R32,
            R32G32,
            R32G32B32,
            R32G32B32A32
        };

        crimild::UInt32 location;
        crimild::UInt32 binding;
        Format format;
        crimild::UInt32 offset;
    };

    struct VertexInputBindingDescription {
        crimild::UInt32 binding;
        crimild::UInt32 stride;
    };

    struct VertexP2 {
        crimild::Vector2f position;

        static std::vector< VertexInputAttributeDescription > getAttributeDescriptions( crimild::UInt32 binding ) noexcept
        {
            return {
                {
                    .location = 0,
                    .binding = binding,
                    .format = VertexInputAttributeDescription::Format::R32G32,
                    .offset = offsetof( VertexP2, position ),
                }
            };
        }

        static VertexInputBindingDescription getBindingDescription( crimild::UInt32 binding ) noexcept
        {
            return {
				.binding = binding,
                .stride = sizeof( VertexP2 ),
            };
        }
    };

    using VertexP2Buffer = VertexBuffer< VertexP2 >;

    struct VertexP2C3 {
        crimild::Vector2f position;
        crimild::RGBColorf color;

        static std::vector< VertexInputAttributeDescription > getAttributeDescriptions( crimild::UInt32 binding ) noexcept
        {
            return {
                {
                    .location = 0,
                    .binding = binding,
                    .format = VertexInputAttributeDescription::Format::R32G32,
                    .offset = offsetof( VertexP2C3, position ),
                },
                {
                    .location = 1,
                    .binding = binding,
                    .format = VertexInputAttributeDescription::Format::R32G32B32,
                    .offset = offsetof( VertexP2C3, color ),
                }
            };
        }

        static VertexInputBindingDescription getBindingDescription( crimild::UInt32 binding ) noexcept
        {
            return {
                .binding = binding,
                .stride = sizeof( VertexP2C3 ),
            };
        }
    };

    using VertexP2C3Buffer = VertexBuffer< VertexP2C3 >;

    struct VertexP3 {
        crimild::Vector3f position;

        static std::vector< VertexInputAttributeDescription > getAttributeDescriptions( crimild::UInt32 binding ) noexcept
        {
            return {
                {
                    .location = 0,
                    .binding = binding,
                    .format = VertexInputAttributeDescription::Format::R32G32B32,
                    .offset = offsetof( VertexP3, position ),
                }
            };
        }

        static VertexInputBindingDescription getBindingDescription( crimild::UInt32 binding ) noexcept
        {
            return {
                .binding = binding,
                .stride = sizeof( VertexP3 ),
            };
        }
    };

    using VertexP3Buffer = VertexBuffer< VertexP3 >;

    struct VertexP3C3 {
        crimild::Vector3f position;
        crimild::RGBColorf color;

        static std::vector< VertexInputAttributeDescription > getAttributeDescriptions( crimild::UInt32 binding ) noexcept
        {
            return {
                {
                    .location = 0,
                    .binding = binding,
                    .format = VertexInputAttributeDescription::Format::R32G32B32,
                    .offset = offsetof( VertexP3C3, position ),
                },
                {
                    .location = 1,
                    .binding = binding,
                    .format = VertexInputAttributeDescription::Format::R32G32B32,
                    .offset = offsetof( VertexP3C3, color ),
                }
            };
        }

        static VertexInputBindingDescription getBindingDescription( crimild::UInt32 binding ) noexcept
        {
            return {
                .binding = binding,
                .stride = sizeof( VertexP3C3 ),
            };
        }
    };

    using VertexP3C3Buffer = VertexBuffer< VertexP3C3 >;

}

#endif
