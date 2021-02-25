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

#ifndef CRIMILD_CORE_RENDERING_FRAME_GRAPH_RESOURCE_
#define CRIMILD_CORE_RENDERING_FRAME_GRAPH_RESOURCE_

#include "Foundation/Containers/Array.hpp"

namespace crimild {

    class FrameGraphOperation;

    /**
     * \brief A resource to be used by frame graph operations
     */
    class FrameGraphResource {
    public:
        enum class Type {
            BUFFER,
            BUFFER_VIEW,
            IMAGE,
            IMAGE_VIEW,
            TEXTURE,
            ATTACHMENT,
        };

    public:
        virtual ~FrameGraphResource( void ) = default;

        virtual Type getType( void ) const noexcept = 0;

        virtual void setReadBy( FrameGraphOperation *op ) noexcept { m_readBy.add( op ); }

        inline Bool isRead( void ) const noexcept { return !m_readBy.empty(); }

        template< typename Fn >
        void eachReadBy( Fn fn ) noexcept
        {
            m_readBy.each( fn );
        }

        virtual void setWrittenBy( FrameGraphOperation *op ) noexcept { m_writtenBy.add( op ); }

        inline Bool isWritten( void ) const noexcept { return !m_writtenBy.empty(); }

        template< typename Fn >
        void eachWrittenBy( Fn fn ) noexcept
        {
            m_writtenBy.each( fn );
        }

    private:
        Array< FrameGraphOperation * > m_readBy;
        Array< FrameGraphOperation * > m_writtenBy;
    };

}

#endif
