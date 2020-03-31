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

#ifndef CRIMILD_CORE_RENDERING_RENDER_PASS_
#define CRIMILD_CORE_RENDERING_RENDER_PASS_

#include "Foundation/SharedObject.hpp"
#include "Rendering/Format.hpp"
#include "Rendering/Image.hpp"
#include "Rendering/RenderResource.hpp"

namespace crimild {

    class CommandBuffer;

    class Attachment : public SharedObject {
	public:
		enum class LoadOp {
			LOAD,
			CLEAR,
			DONT_CARE,
		};

		enum class StoreOp {
			STORE,
			DONT_CARE,
		};
		
    public:
        Format format;
        Image::Usage usage = Image::Usage::PRESENTATION;
		LoadOp loadOp = LoadOp::DONT_CARE;
		StoreOp storeOp = StoreOp::DONT_CARE;
		LoadOp stencilLoadOp = LoadOp::DONT_CARE;
		StoreOp stencilStoreOp = StoreOp::DONT_CARE;
		//Image::Layout initialLayout = Image::Layout::UNDEFINED;
		//Image::Layout finalLayout = Image::Layout::UNDEFINED;
    };

    class RenderSubpass : public SharedObject {
    public:
        containers::Array< SharedPointer< Attachment >> colorAttachments;
        SharedPointer< Attachment > depthStencilAttachment;

        SharedPointer< CommandBuffer > commands;
    };

    class RenderPass :
    	public SharedObject,
    	public RenderResourceImpl< RenderPass > {

    public:
		containers::Array< SharedPointer< Attachment >> attachments;
		containers::Array< SharedPointer< RenderSubpass >> subpasses;
    };

}

#endif

