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

#ifndef CRIMILD_CORE_RENDERING_FRAME_COMPOSITION_
#define CRIMILD_CORE_RENDERING_FRAME_COMPOSITION_

#include "Foundation/Containers/List.hpp"
#include "Foundation/Containers/Map.hpp"
#include "Foundation/SharedObject.hpp"

namespace crimild {

    class Attachment;

    namespace compositions {

        class Composition {
        public:
            Composition( void ) noexcept = default;
            Composition( const Composition &cmp1, const Composition &cmp2 ) noexcept;
            Composition( const Composition & ) noexcept;
            Composition( Composition && ) noexcept;
            ~Composition( void ) noexcept = default;

            Composition &operator=( const Composition & ) noexcept;
            Composition &operator=( Composition && ) noexcept;

            template< typename T >
            T *create( void ) noexcept
            {
                auto obj = crimild::alloc< T >();
                m_objects.add( obj );
                return crimild::get_ptr( obj );
            }

            Attachment *createAttachment( std::string name ) noexcept;

            template< typename Fn >
            void eachAttachment( Fn fn ) noexcept
            {
                m_attachments.eachValue( fn );
            }

            inline Composition &setOutput( Attachment *att ) noexcept
            {
                m_output = att;
                return *this;
            }

            inline Attachment *getOutput( void ) noexcept { return m_output; }

        private:
            Attachment *m_output = nullptr;
            Map< std::string, Attachment * > m_attachments;
            List< SharedPointer< SharedObject > > m_objects;
        };

    }

}

#endif
