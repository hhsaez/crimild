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

#ifndef CRIMILD_RENDERING_UNIFORM_BUFFER_
#define CRIMILD_RENDERING_UNIFORM_BUFFER_

#include "Rendering/Buffer.hpp"
#include "Mathematics/Matrix.hpp"

namespace crimild {

    class UniformBuffer : public Buffer {
    public:
        virtual ~UniformBuffer( void ) noexcept = default;
    };

    template< typename T >
    class UniformBufferImpl : public UniformBuffer {
    public:
        explicit UniformBufferImpl( T const &data = T() ) noexcept { setData( data ); }
        virtual ~UniformBufferImpl( void ) noexcept = default;

        Usage getUsage( void ) const noexcept override { return Buffer::Usage::UNIFORM_BUFFER; }

        crimild::Size getSize( void ) const noexcept override { return sizeof( T ); }
        crimild::Size getStride( void ) const noexcept override { return sizeof( T ); }

        void *getRawData( void ) noexcept override { return static_cast< void * >( &m_data ); }
        const void *getRawData( void ) const noexcept override { return static_cast< const void * >( &m_data ); }

        T &getData( void ) noexcept { return m_data; }
        const T &getData( void ) const noexcept { return m_data; }
        void setData( T const &data ) noexcept { m_data = data; }

    private:
        T m_data;
    };

    struct ModelViewProjUniform {
        Matrix4f model;
        Matrix4f view;
        Matrix4f proj;
    };

    using ModelViewProjUniformBuffer = UniformBufferImpl< ModelViewProjUniform >;

}

#endif


