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

#ifndef CRIMILD_CORE_VARIANT_
#define CRIMILD_CORE_VARIANT_

#include "Coding/Codable.hpp"

namespace crimild {

    /**
     * \brief A type that can hold any value
     *
     * This is a helper class that can be used to store any kind of value. It's
     * specially useful for settings or other context values that are usually
     * stored in a map and retrieved by key.
     *
     * \warning This is an unsafe type and you should be careful when working with it
     * since it can lead to some degenarate cases (see unit tests).
     *
     * \code {.cpp}
     *  Variant var( 10 ); // this is an int by default
     *  assert( var.get< float >() == 10 ); // fails
     * \endcode
     *
     * \warning Use only with native types and trivial types. Do not use with
     * containers (std::vector, std::string, etc.) or complex types (i.e. Node).
     */
    class Variant : public coding::Codable {
        CRIMILD_IMPLEMENT_RTTI( crimild::Variant );

    public:
        Variant( void ) = default;

        template< typename T >
        explicit Variant( const T &value ) noexcept
            : m_data( sizeof( value ) )
        {
            if ( m_data.size() > 0 ) {
                memcpy( m_data.data(), &value, m_data.size() );
            }
        }

        Variant( const Variant &other ) noexcept
            : m_data( other.m_data )
        {
        }

        Variant( Variant &&other ) noexcept
            : m_data( std::move( other.m_data ) )
        {
        }

        ~Variant( void ) = default;

        Variant &operator=( const Variant &other ) noexcept
        {
            m_data = other.m_data;
            return *this;
        }

        Variant &operator=( Variant &&other ) noexcept
        {
            m_data = std::move( other.m_data );
            return *this;
        }

        inline bool isValid( void ) const noexcept { return m_data.size() > 0; }

        template< typename T >
        T &get( void ) noexcept
        {
            assert( m_data.size() == sizeof( T ) );
            return *reinterpret_cast< T * >( m_data.data() );
        }

        template< typename T >
        const T &get( void ) const noexcept
        {
            assert( m_data.size() == sizeof( T ) );
            return *reinterpret_cast< const T * >( m_data.data() );
        }

    private:
        /**
         * \brief Raw bytes
         *
         * Probably not the most efficient way to store the data, but variants
         * are supposed to be helpful, not efficient.
         */
        std::vector< std::byte > m_data;

        /**
           \name Coding support
        */
        //@{

    public:
        virtual void encode( coding::Encoder &encoder ) override;
        virtual void decode( coding::Decoder &decoder ) override;

        //@}
    };

}

#endif
