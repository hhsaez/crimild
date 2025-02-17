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

#ifndef CRIMILD_CORE_RENDERING_RASTERIZATION_STATE_
#define CRIMILD_CORE_RENDERING_RASTERIZATION_STATE_

#include "Crimild_Foundation.hpp"
#include "Rendering/CompareOp.hpp"

namespace crimild {

    enum class PolygonMode {
        FILL = 0,
        LINE = 1,
        POINT = 2,
    };

    enum class FrontFace {
        COUNTER_CLOCKWISE = 0,
        CLOCKWISE = 1,
    };

    enum class CullMode {
        NONE = 0,
        FRONT = 0x00000001,
        BACK = 0x00000002,
        FRONT_AND_BACK = 0x00000003,
    };

    struct RasterizationState {
        Bool depthClampEnable = false;        // 'true' might be required for shadow maps
        Bool rasterizerDiscardEnable = false; // 'true' disables output to the framebuffer
        PolygonMode polygonMode = PolygonMode::FILL;
        CullMode cullMode = CullMode::BACK;
        FrontFace frontFace = FrontFace::COUNTER_CLOCKWISE;
        Bool depthBiasEnable = false;
        Real32 depthBiasConstantFactor = 0.0f;
        Real32 depthBiasClamp = 0.0f;
        Real32 depthBiasSlopeFactor = 0.0f;
        Real32 lineWidth = 1.0f;
    };

}

#endif
