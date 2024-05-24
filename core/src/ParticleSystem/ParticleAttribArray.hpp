/*
 * Copyright (c) 2013, Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CRIMILD_PARTICLE_SYSTEM_ATTRIB_ARRAY_
#define CRIMILD_PARTICLE_SYSTEM_ATTRIB_ARRAY_

#include "Crimild_Foundation.hpp"
#include "Crimild_Mathematics.hpp"

namespace crimild {

    using ParticleId = crimild::Size;

    /**
           \brief Default attrib types

           This is kept as a standar enum so it can be easily
           custumized by adding new types
         */
    class ParticleAttrib {
    public:
        enum {
            POSITION,
            VELOCITY,
            ACCELERATION,
            START_COLOR,
            END_COLOR,
            COLOR,
            UNIFORM_SCALE,
            UNIFORM_SCALE_START,
            UNIFORM_SCALE_END,
            EULER_ANGLES,
            TIME,
            LIFE_TIME,
            SORT_REFERENCE,
            USER_DEFINED = 1000,
            USER_DEFINED_MAX,
        };
    };

    using ParticleAttribType = crimild::UInt16;

    /**
           \brief Interface for all particle attributes
         */
    class ParticleAttribArray : public SharedObject {
    public:
        virtual ~ParticleAttribArray( void ) { }

        /**
                   \brief Resizes the array
                 */
        virtual void reset( crimild::Size count ) = 0;

        /**
                   \brief Swaps one attribute for another

                   Usually invoked when particles are killed
                 */
        virtual void swap( ParticleId a, ParticleId b ) = 0;

        /**
                   \brief Gets the number of elements in the array
                 */
        virtual crimild::Size getCount( void ) const = 0;

        /**
                   \brief Data accessor
                 */
        template< typename T >
        T &operator[]( crimild::Size idx )
        {
            return static_cast< T * >( getRawData() )[ idx ];
        }

        /**
                   \brief Data accessor (const version)
                 */
        template< typename T >
        const T &operator[]( crimild::Size idx ) const
        {
            return static_cast< T * >( getRawData() )[ idx ];
        }

        /**
                   \brief Useful templated function to get the data
                 */
        template< typename T >
        T *getData( void )
        {
            return static_cast< T * >( getRawData() );
        }

        /**
                   \brief Useful templated function to get the data (const version)
                 */
        template< typename T >
        const T *getData( void ) const
        {
            return static_cast< T * >( getRawData() );
        }

        /**
                   \brief Gets the raw data for the array
                 */
        virtual void *getRawData( void ) = 0;

        /**
                   \brief Gets the raw data for the array (const version)
                 */
        virtual const void *getRawData( void ) const = 0;
    };

    using ParticleAttribArrayPtr = SharedPointer< ParticleAttribArray >;

    /**
           \brief Basic implementation for an attribute array

           \remarks Attribute data is stored such as active elements are at
           the very begining of the array.
         */
    template< typename T >
    class ParticleAttribArrayImpl : public ParticleAttribArray {
    public:
        ParticleAttribArrayImpl( void )
        {
        }

        virtual ~ParticleAttribArrayImpl( void )
        {
        }

        virtual crimild::Size getCount( void ) const override
        {
            return _data.size();
        }

        virtual void *getRawData( void ) override
        {
            return _data.getData();
        }

        virtual const void *getRawData( void ) const override
        {
            return _data.getData();
        }

        virtual void reset( crimild::Size count ) override
        {
            _data.resize( count );
        }

        virtual void swap( ParticleId a, ParticleId b ) override
        {
            auto temp = _data[ a ];
            _data[ a ] = _data[ b ];
            _data[ b ] = temp;
        }

    private:
        /**
                   \brief Holds the data for the attributes

                   \remarks This member is NOT thread safe.
                 */
        Array< T > _data;
    };

    using Vector3fParticleAttribArray = ParticleAttribArrayImpl< Vector3f >;
    using RGBAColorfParticleAttribArray = ParticleAttribArrayImpl< ColorRGBA >;
    using Real32ParticleAttribArray = ParticleAttribArrayImpl< crimild::Real32 >;

}

#endif
