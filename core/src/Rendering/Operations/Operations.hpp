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
#include "Mathematics/Matrix3.hpp"
#include "Rendering/DescriptorSet.hpp"

namespace crimild {

    class FrameGraphOperation;
    class FrameGraphResource;
    class Shader;

    namespace framegraph {

        [[deprecated]] SharedPointer< FrameGraphOperation > fetchRenderables( void ) noexcept;

        [[deprecated]] SharedPointer< FrameGraphOperation > present( SharedPointer< FrameGraphOperation > op ) noexcept;
        [[deprecated]] SharedPointer< FrameGraphOperation > present( SharedPointer< FrameGraphResource > resource ) noexcept;

        [[deprecated]] SharedPointer< FrameGraphOperation > shader( std::string source ) noexcept;

        [[deprecated]] SharedPointer< FrameGraphOperation > gBufferPass( SharedPointer< FrameGraphResource > const renderables ) noexcept;

        [[deprecated]] SharedPointer< FrameGraphOperation > renderShadowAtlas( SharedPointer< FrameGraphResource > const renderables ) noexcept;

        [[deprecated]] SharedPointer< FrameGraphOperation > computeReflectionMap( SharedPointer< FrameGraphResource > const renderables ) noexcept;
        [[deprecated]] SharedPointer< FrameGraphOperation > computeIrradianceMap( SharedPointer< FrameGraphResource > const renderables ) noexcept;
        [[deprecated]] SharedPointer< FrameGraphOperation > computePrefilterMap( SharedPointer< FrameGraphResource > const renderables ) noexcept;
        [[deprecated]] SharedPointer< FrameGraphOperation > computeBRDFLUT( SharedPointer< FrameGraphResource > const renderables ) noexcept;

        [[deprecated]] SharedPointer< FrameGraphOperation > lightingPass(
            SharedPointer< FrameGraphResource > const &albedo,
            SharedPointer< FrameGraphResource > const &positions,
            SharedPointer< FrameGraphResource > const &normals,
            SharedPointer< FrameGraphResource > const &materials,
            SharedPointer< FrameGraphResource > const &depth,
            SharedPointer< FrameGraphResource > const &shadowAtlas
        ) noexcept;

        [[deprecated]] SharedPointer< FrameGraphOperation > iblPass(
            SharedPointer< FrameGraphResource > const &albedo,
            SharedPointer< FrameGraphResource > const &positions,
            SharedPointer< FrameGraphResource > const &normals,
            SharedPointer< FrameGraphResource > const &materials,
            SharedPointer< FrameGraphResource > const &depth,
            SharedPointer< FrameGraphResource > const &reflectionAtlas,
            SharedPointer< FrameGraphResource > const &irradianceAtlas,
            SharedPointer< FrameGraphResource > const &prefilterAtlas,
            SharedPointer< FrameGraphResource > const &brdfLUT
        ) noexcept;

        [[deprecated]] SharedPointer< FrameGraphOperation > forwardUnlitPass(
            SharedPointer< FrameGraphResource > const &renderables,
            SharedPointer< FrameGraphResource > const &colorAttachment = nullptr,
            SharedPointer< FrameGraphResource > const &depthAttachment = nullptr
        ) noexcept;

        [[deprecated]] SharedPointer< FrameGraphOperation > brightPassFilter( SharedPointer< FrameGraphResource > const &image, const Vector3f &filter ) noexcept;

        [[deprecated]] SharedPointer< FrameGraphOperation > gaussianBlur( SharedPointer< FrameGraphResource > const &image ) noexcept;

        [[deprecated]] SharedPointer< FrameGraphOperation > tonemapping( SharedPointer< FrameGraphResource > const &image ) noexcept;

        [[deprecated]] SharedPointer< FrameGraphOperation > invert( SharedPointer< FrameGraphResource > const &image ) noexcept;
        [[deprecated]] SharedPointer< FrameGraphOperation > grayscale( SharedPointer< FrameGraphResource > const &image ) noexcept;

        [[deprecated]] SharedPointer< FrameGraphOperation > convolution( std::string name, SharedPointer< FrameGraphResource > const &image, const Matrix3f &kernel ) noexcept;
        [[deprecated]] SharedPointer< FrameGraphOperation > blur( SharedPointer< FrameGraphResource > const &image ) noexcept;
        [[deprecated]] SharedPointer< FrameGraphOperation > sharpen( SharedPointer< FrameGraphResource > const &image ) noexcept;
        [[deprecated]] SharedPointer< FrameGraphOperation > edges( SharedPointer< FrameGraphResource > const &image ) noexcept;

        [[deprecated]] SharedPointer< FrameGraphOperation > blend( Array< SharedPointer< FrameGraphResource > > const &resources, std::string mode = "additive" ) noexcept;

        [[deprecated]] SharedPointer< FrameGraphOperation > channel( std::string name, SharedPointer< FrameGraphResource > input, Size channelId ) noexcept;

        [[deprecated]] SharedPointer< FrameGraphOperation > debug( Array< SharedPointer< FrameGraphResource > > const &resources ) noexcept;
    }

}

#endif
