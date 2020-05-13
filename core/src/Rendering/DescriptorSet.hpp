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

#ifndef CRIMILD_RENDERING_DESCRIPTOR_SET_
#define CRIMILD_RENDERING_DESCRIPTOR_SET_

#include "Rendering/RenderResource.hpp"
#include "Rendering/Shader.hpp"
#include "Foundation/SharedObject.hpp"

namespace crimild {

    class Buffer;
    class Pipeline;
    class Texture;

    struct Descriptor {
        enum class Type {
            UNIFORM_BUFFER,
            TEXTURE,
            VIEW_PROJECTION_UNIFORM_BUFFER,
        };

        Type type;

        union {
            Buffer *buffer;
            Texture *texture;
        };
    };

    using DescriptorArray = containers::Array< Descriptor >;

    enum class DescriptorType {
        COMBINED_IMAGE_SAMPLER,
		UNIFORM_BUFFER,
    };

    class DescriptorSetLayout : public SharedObject, public RenderResourceImpl< DescriptorSetLayout > {
    public:
        struct Binding {
            DescriptorType descriptorType;
            crimild::Size descriptorCount = 1;
            Shader::Stage stage;
        };

    public:
        containers::Array< Binding > bindings;
    };

    class DescriptorPool : public SharedObject, public RenderResourceImpl< DescriptorPool > {
    public:
        SharedPointer< DescriptorSetLayout > descriptorSetLayout;
    };

    class DescriptorSet : public SharedObject, public RenderResourceImpl< DescriptorSet > {
    public:
        struct Write {
            DescriptorType descriptorType;

            union {
                Buffer *buffer;
                Texture *texture;
            };
        };

    public:
        SharedPointer< DescriptorPool > descriptorPool;
        SharedPointer< DescriptorSetLayout > descriptorSetLayout;
        containers::Array< Write > writes;
    };


}

#endif


