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

#ifndef CRIMILD_RENDERING_VERTEX_ATTRIBUTE_
#define CRIMILD_RENDERING_VERTEX_ATTRIBUTE_

#include "Crimild_Foundation.hpp"
#include "Rendering/Format.hpp"

namespace crimild {

    struct VertexAttribute {
        enum Name {
            POSITION,
            NORMAL,
            TANGENT,
            COLOR,
            TEX_COORD,
            TEX_COORD_1,
            TEX_COORD_2,
            TEX_COORD_3,
            BLEND_INDICES,
            BLEND_WEIGHT,
            SIZE,

            USER_ATTRIBUTE = 100,
            USER_ATTRIBUTE_0 = USER_ATTRIBUTE + 0,
            USER_ATTRIBUTE_1 = USER_ATTRIBUTE + 1,
            USER_ATTRIBUTE_2 = USER_ATTRIBUTE + 2,
            USER_ATTRIBUTE_3 = USER_ATTRIBUTE + 3,
            USER_ATTRIBUTE_4 = USER_ATTRIBUTE + 4,
            USER_ATTRIBUTE_5 = USER_ATTRIBUTE + 5,
            USER_ATTRIBUTE_6 = USER_ATTRIBUTE + 6,
            USER_ATTRIBUTE_7 = USER_ATTRIBUTE + 7,
            USER_ATTRIBUTE_8 = USER_ATTRIBUTE + 8,
            USER_ATTRIBUTE_9 = USER_ATTRIBUTE + 9,
            USER_ATTRIBUTE_10 = USER_ATTRIBUTE + 10,
            USER_ATTRIBUTE_11 = USER_ATTRIBUTE + 11,
            USER_ATTRIBUTE_12 = USER_ATTRIBUTE + 12,
            USER_ATTRIBUTE_13 = USER_ATTRIBUTE + 13,
            USER_ATTRIBUTE_14 = USER_ATTRIBUTE + 14,
            USER_ATTRIBUTE_15 = USER_ATTRIBUTE + 15,
            USER_ATTRIBUTE_16 = USER_ATTRIBUTE + 16,
            USER_ATTRIBUTE_17 = USER_ATTRIBUTE + 17,
            USER_ATTRIBUTE_18 = USER_ATTRIBUTE + 18,
            USER_ATTRIBUTE_19 = USER_ATTRIBUTE + 19,
            USER_ATTRIBUTE_20 = USER_ATTRIBUTE + 20,
        };

        Name name;
        Format format;
        crimild::UInt32 offset = 0;

        crimild::Bool operator==( const VertexAttribute &other ) const noexcept
        {
            return name == other.name && format == other.format && offset == other.offset;
        }
    };

}

#endif
