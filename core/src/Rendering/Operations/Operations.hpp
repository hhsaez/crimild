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

#ifndef CRIMILD_CORE_RENDERING_OPERATIONS_
#define CRIMILD_CORE_RENDERING_OPERATIONS_

#include "Foundation/Memory.hpp"
#include "Rendering/DescriptorSet.hpp"

namespace crimild {

    class FrameGraphOperation;
    class FrameGraphResource;
    class Shader;

    namespace framegraph {

        SharedPointer< FrameGraphOperation > fetchRenderables( void ) noexcept;

        SharedPointer< FrameGraphOperation > present( SharedPointer< FrameGraphOperation > op ) noexcept;
        SharedPointer< FrameGraphOperation > present( SharedPointer< FrameGraphResource > resource ) noexcept;

        SharedPointer< FrameGraphOperation > shader( std::string source ) noexcept;

        SharedPointer< FrameGraphOperation > gBufferPass( SharedPointer< FrameGraphResource > const renderables ) noexcept;

        SharedPointer< FrameGraphOperation > renderShadowAtlas( SharedPointer< FrameGraphResource > const renderables ) noexcept;

        SharedPointer< FrameGraphOperation > computeReflectionMap( SharedPointer< FrameGraphResource > const renderables ) noexcept;
        SharedPointer< FrameGraphOperation > computeIrradianceMap( SharedPointer< FrameGraphResource > const renderables ) noexcept;
        SharedPointer< FrameGraphOperation > computePrefilterMap( SharedPointer< FrameGraphResource > const renderables ) noexcept;
        SharedPointer< FrameGraphOperation > computeBRDFLUT( SharedPointer< FrameGraphResource > const renderables ) noexcept;

        SharedPointer< FrameGraphOperation > lightingPass(
            SharedPointer< FrameGraphResource > const &albedo,
            SharedPointer< FrameGraphResource > const &positions,
            SharedPointer< FrameGraphResource > const &normals,
            SharedPointer< FrameGraphResource > const &materials,
            SharedPointer< FrameGraphResource > const &depth,
            SharedPointer< FrameGraphResource > const &shadowAtlas ) noexcept;

        SharedPointer< FrameGraphOperation > iblPass(
            SharedPointer< FrameGraphResource > const &albedo,
            SharedPointer< FrameGraphResource > const &positions,
            SharedPointer< FrameGraphResource > const &normals,
            SharedPointer< FrameGraphResource > const &materials,
            SharedPointer< FrameGraphResource > const &depth,
            SharedPointer< FrameGraphResource > const &reflectionAtlas,
            SharedPointer< FrameGraphResource > const &irradianceAtlas,
            SharedPointer< FrameGraphResource > const &prefilterAtlas,
            SharedPointer< FrameGraphResource > const &brdfLUT ) noexcept;

        SharedPointer< FrameGraphOperation > forwardUnlitPass(
            SharedPointer< FrameGraphResource > const &renderables,
            SharedPointer< FrameGraphResource > const &colorAttachment = nullptr,
            SharedPointer< FrameGraphResource > const &depthAttachment = nullptr ) noexcept;

        SharedPointer< FrameGraphOperation > computeImage(
            Extent2D extent,
            SharedPointer< Shader > shader,
            Format format = Format::R8G8B8A8_UNORM,
            Array< Descriptor > descriptors = {} ) noexcept;

        SharedPointer< FrameGraphOperation > tonemapping( SharedPointer< FrameGraphResource > const &image ) noexcept;

        SharedPointer< FrameGraphOperation > blend( Array< SharedPointer< FrameGraphResource > > const &resources ) noexcept;

        SharedPointer< FrameGraphOperation > channel( std::string name, SharedPointer< FrameGraphResource > input, Size channelId ) noexcept;

        SharedPointer< FrameGraphOperation > debug( Array< SharedPointer< FrameGraphResource > > const &resources ) noexcept;
    }

}

#endif
