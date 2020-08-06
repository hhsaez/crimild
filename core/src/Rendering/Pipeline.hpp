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

#ifndef CRIMILD_RENDERING_PIPELINE_
#define CRIMILD_RENDERING_PIPELINE_

#include "Rendering/ColorBlendState.hpp"
#include "Rendering/DepthStencilState.hpp"
#include "Rendering/RasterizationState.hpp"
#include "Rendering/RenderResource.hpp"
#include "Rendering/VertexBuffer.hpp"
#include "Rendering/ViewportDimensions.hpp"
#include "Foundation/SharedObject.hpp"
#include "Primitives/Primitive.hpp"
#include "Mathematics/Rect.hpp"

namespace crimild {

    class AlphaState;
    class CullFaceState;
    class DescriptorSetLayout;
    class PolygonState;
    class RenderPass;
    class ShaderProgram;

    class Pipeline : public SharedObject, public RenderResourceImpl< Pipeline > {
    public:
        virtual ~Pipeline( void ) noexcept = default;

        SharedPointer< ShaderProgram > program;
        RenderPass *renderPass = nullptr;
        Primitive::Type primitiveType = Primitive::Type::TRIANGLES;
        ViewportDimensions viewport;
        ViewportDimensions scissor;
        DepthStencilState depthStencilState;
        RasterizationState rasterizationState;
        ColorBlendState colorBlendState;

        // Deprecated
        SharedPointer< CullFaceState > cullFaceState;
        SharedPointer< PolygonState > polygonState;
    };

}

#endif
