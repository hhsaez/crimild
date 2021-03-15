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

#ifndef CRIMILD_CORE_RENDERING_FRAME_GRAPH_OPERATION_
#define CRIMILD_CORE_RENDERING_FRAME_GRAPH_OPERATION_

#include "Foundation/Containers/Array.hpp"
#include "Foundation/Memory.hpp"
#include "Foundation/NamedObject.hpp"
#include "Foundation/SharedObject.hpp"

namespace crimild {

    class FrameGraphResource;

    /**
     * \brief Executes a single frame operation
     */
    class FrameGraphOperation
        : public SharedObject,
          public NamedObject {
    public:
        enum class Type {
            SCENE_PASS,
            RENDER_PASS,
            COMPUTE_PASS,
        };

    public:
        virtual ~FrameGraphOperation( void ) = default;

        virtual Type getType( void ) const noexcept = 0;

        /**
            \brief Executes the operation
         */
        std::function< Bool( Size imageIndex, Bool force ) > apply = []( auto, auto ) {
            return true;
        };

        void reads( Array< SharedPointer< FrameGraphResource > > const &resources ) noexcept;

        template< typename Fn >
        void eachRead( Fn fn ) noexcept
        {
            m_reads.each( fn );
        }

        void writes( Array< SharedPointer< FrameGraphResource > > const &resources ) noexcept;

        template< typename Fn >
        void eachWritten( Fn fn ) noexcept
        {
            m_writes.each( fn );
        }

        void produces( Array< SharedPointer< FrameGraphResource > > const &resources ) noexcept;

        template< typename Fn >
        void eachProduct( Fn fn ) noexcept
        {
            m_products.each( fn );
        }

        inline SharedPointer< FrameGraphResource > getProduct( Size index ) noexcept { return !m_products.empty() ? m_products[ index ] : nullptr; }

        inline SharedPointer< FrameGraphResource > getMainProduct( void ) noexcept { return !m_products.empty() ? m_products.first() : nullptr; }

        inline Size getBlockCount( void ) const noexcept { return m_blocks.size(); }

        template< typename Fn >
        void eachBlocks( Fn fn ) noexcept
        {
            m_blocks.each( fn );
        }

        template< typename Fn >
        void eachBlockedBy( Fn fn ) noexcept
        {
            m_blockedBy.each( fn );
        }

        using Priority = Size;
        inline void setPriority( Priority priority ) noexcept { m_priority = priority; }
        inline Priority getPriority( void ) const noexcept { return m_priority; }

    private:
        Array< SharedPointer< FrameGraphResource > > m_reads;
        Array< SharedPointer< FrameGraphResource > > m_writes;
        Array< SharedPointer< FrameGraphResource > > m_products;

        Array< FrameGraphOperation * > m_blocks;
        Array< SharedPointer< FrameGraphOperation > > m_blockedBy;

        Priority m_priority = 0;
    };

}

#endif
