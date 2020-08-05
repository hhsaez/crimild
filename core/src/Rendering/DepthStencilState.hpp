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
 *     * Neither the name of the copyright holders nor the
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

#ifndef CRIMILD_CORE_RENDERING_DEPTH_STENCIL_STATE_
#define CRIMILD_CORE_RENDERING_DEPTH_STENCIL_STATE_

#include "Foundation/Types.hpp"
#include "Rendering/CompareOp.hpp"

namespace crimild {

    enum class StencilOp {
        KEEP = 0,
        ZERO = 1,
        REPLACE = 2,
        INCREMENT_AND_CLAMP = 3,
        DECREMENT_AND_CLAMP = 4,
        INVERT = 5,
        INCREMENT_AND_WRAP = 6,
        DECREMENT_AND_WRAP = 7,
    };

    struct StencilOpState {
        StencilOp failOp = StencilOp::KEEP; // action to take if stencil test fails
        StencilOp passOp = StencilOp::KEEP; // action to take if both depth and stencil tests pass
        StencilOp depthFailOp = StencilOp::KEEP; // action to take if stencil test passes but depth test fails
        CompareOp compareOp; // comparison operator for stencil test
        UInt32 compareMask;
        UInt32 writeMask;
        UInt32 reference;
    };

    struct DepthStencilState {
        Bool depthTestEnable = true;
        Bool depthWriteEnable = true;
        CompareOp depthCompareOp = CompareOp::LESS_OR_EQUAL;
        Bool depthBoundsTestEnable = false;
        Bool stencilTestEnable = false;
        StencilOpState front = { };
        StencilOpState back = { .compareOp = CompareOp::ALWAYS };
        Real32 minDepthBounds = 0.0f;
        Real32 maxDepthBounds = 1.0f;
    };

}

#endif
