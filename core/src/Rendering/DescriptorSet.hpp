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
    class DescriptorSet;
    class Pipeline;
    class Texture;

    enum class DescriptorType {
        TEXTURE,
		UNIFORM_BUFFER,
    };

    class DescriptorSetLayout : public SharedObject, public RenderResourceImpl< DescriptorSetLayout > {
    public:
        struct Binding {
            DescriptorType descriptorType;
            Shader::Stage stage;
        };

    public:
        virtual ~DescriptorSetLayout( void ) = default;

        containers::Array< Binding > bindings;
    };

    struct Descriptor {
        DescriptorType descriptorType;
		SharedPointer< SharedObject > obj;

		template< typename T >
		T *get( void ) noexcept
		{
			return static_cast< T * >( crimild::get_ptr( obj ) );
		}
    };

    class DescriptorPool : public SharedObject, public RenderResourceImpl< DescriptorPool > {
    public:
        SharedPointer< DescriptorSetLayout > layout;
    };

    class DescriptorSet : public SharedObject, public RenderResourceImpl< DescriptorSet > {
	private:
		using DescriptorArray = containers::Array< Descriptor >;

    public:
        virtual ~DescriptorSet( void ) = default;

        /**
           Descriptor set layout

           This is optional. If no layout is provided, we'll assign one automatically
           when resolving command buffers based on the current binding point.
         */
        SharedPointer< DescriptorSetLayout > layout;

        /**
           \brief A pool for descriptors

           This is optional. If no pool is provided, one will be created automatically
           when binding the descriptor set.
        */
        SharedPointer< DescriptorPool > pool;

		DescriptorArray descriptors;
    };


}

#endif
