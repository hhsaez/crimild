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

#ifndef CRIMILD_CORE_STREAMING_STREAM_
#define CRIMILD_CORE_STREAMING_STREAM_

#include "Foundation/Macros.hpp"
#include "Foundation/SharedObject.hpp"
#include "Foundation/Memory.hpp"
#include "Foundation/RTTI.hpp"
#include "Foundation/Log.hpp"

#include "Mathematics/Transformation.hpp"

#include <cassert>
#include <string>
#include <list>
#include <vector>
#include <map>

namespace crimild {
    
    class Stream;
    class VertexFormat;

    /**
        \brief An object that can be inserted into a stream
    */
    class StreamObject : public SharedObject, public RTTI {
    public:
        /**
            \brief Alias for a stream object unique identifier
        */
        using StreamObjectId = size_t;

    protected:
        /**
            \brief Default constructor
        */
        StreamObject( void ) { }

    public:
        /**
            \brief Destructor
        */
        virtual ~StreamObject( void ) { }

        /*
            \brief Retrieves the object unique identifier

            The unique identifier for a live object is defined by
            its memory address. This could lead to duplicated values
            in history, but at the time of streaming an object, no 
            two objects can have the same memory address
        */
        StreamObjectId getUniqueIdentifier( void ) const { return ( StreamObjectId ) this; }

        /**
            \brief Register an object in the stream

            \remarks Subclasses must invoke this method at the very beggining like this:

            \code
                class Foo : public StreamObject {
                public:
                    bool Foo::registerInStream( Stream &s )
                    {
                        if ( StreamObject::registerInStream( s ) ) {
                            return false;
                        }

                        // do something else

                        return true;
                    }
                };
            \endcode
        */
        virtual bool registerInStream( Stream &s );

        /**
            \briefs Writes an object into the stream

            \remarks Subclases must invoke this method
        */
        virtual void save( Stream &s );

        /**
            \brief Reads an object's properties from the stream
        */
        virtual void load( Stream &s );
    };

    /**
        \brief Handles object building
    */
    class StreamObjectFactory : public StaticSingleton< StreamObjectFactory > {
    public:
        using StreamObjectBuilderFunction = std::function< SharedPointer< StreamObject > ( void ) >;

    public:
        StreamObjectFactory( void ) { }
        virtual ~StreamObjectFactory( void ) { }

        template< class T >
        void registerBuilder( std::string className )
        {
            _builders[ className ] = []( void ) -> SharedPointer< StreamObject > {
                return crimild::alloc< T >();
            };
        }

        SharedPointer< StreamObject > buildObject( std::string className );

    private:
        std::map< std::string, StreamObjectBuilderFunction > _builders;

    public:
        template< class T >
        class Builder {
        public:
            Builder( const char *className )
            {
                StreamObjectFactory::getInstance()->registerBuilder< T >( className );
            }

            ~Builder( void )
            {

            }
        };
    };

    /**
        \brief Useful macro for registering builders for classes
    */
#define CRIMILD_REGISTER_STREAM_OBJECT_BUILDER( X ) \
    static crimild::StreamObjectFactory::Builder< X > CRIMILD_RANDOM_VARIABLE_NAME( __streamObjectBuilder__ )( #X );
      
    /**
        \brief Base class for streams
    */  
    class Stream : public NonCopyable {
    public:
        static constexpr const char *FLAG_STREAM_START = "___CRIMILD_STREAM_START___";
        static constexpr const char *FLAG_STREAM_END = "___CRIMILD_STREAM_END___";
        static constexpr const char *FLAG_TOP_LEVEL_OBJECT = "___CRIMILD_TOP_LEVEL_OBJECT___";
        static constexpr const char *FLAG_INNER_OBJECT = "___CRIMILD_INNER_OBJECT___";
        static constexpr const char *FLAG_OBJECT_START = "___CRIMILD_OBJECT_START___";
        static constexpr const char *FLAG_OBJECT_END = "___CRIMILD_OBJECT_END___";

    public:
        /**
            \brief Default constructor
        */
        Stream( void );

        /**
            \brief Destructor
        */
        virtual ~Stream( void );

        /**
            \name Saving
        */
        //@{
    public:

        void addObject( StreamObject *obj ) 
        { 
            addObject( crimild::retain( obj ) ); 
        }

        void addObject( SharedPointer< StreamObject > const &obj );

        virtual bool flush( void );

        //@}

        /**
            \name Loading
        */
        //@{
    public:
        virtual bool load( void );

        size_t getObjectCount( void ) const { return _topLevelObjects.size(); }

        template< class T >
        SharedPointer< T > getObjectAt( unsigned int index ) 
        { 
            return crimild::cast_ptr< T >( _topLevelObjects[ index ] ); 
        }

        //@}

    private:
        bool isTopLevel( SharedPointer< StreamObject > const &obj ) const;

    private:
        std::vector< SharedPointer< StreamObject >> _topLevelObjects;

    public:
        bool registerObject( StreamObject *obj );
        bool registerObject( StreamObject::StreamObjectId, SharedPointer< StreamObject > const &obj );

    private:
        // TODO: replace this for crimild::Map<>
        std::map< StreamObject::StreamObjectId, SharedPointer< StreamObject >> _objects;
        std::list< SharedPointer< StreamObject >> _orderedObjects;

        /**
            \name Writing properites
        */
        //@{
    public:
        void write( const char *str );
        void write( const std::string &str );
        void write( const VertexFormat &vf );

        template< size_t SIZE, typename PRECISION >
        void write( const Vector< SIZE, PRECISION > &v )
        {
            writeRawBytes( ( const PRECISION * ) v, SIZE * sizeof( PRECISION ) );
        }

        template< size_t SIZE, typename PRECISION >
        void write( const Matrix< SIZE, PRECISION > &m )
        {
            writeRawBytes( ( const PRECISION * ) m, SIZE * sizeof( PRECISION ) );
        }

        void write( const Quaternion4f &q );
        void write( const Transformation &t );

        template< class T >
        void write( SharedPointer< T > &obj )
        {
            StreamObject::StreamObjectId objId = ( obj != nullptr ? obj->getUniqueIdentifier() : 0 );
            write( objId );
        }

        template< class T >
        void write( std::vector< SharedPointer< T >> &os )
        {
            size_t count = os.size();
            write( count );
            for ( auto &o : os ) {
                write( o->getUniqueIdentifier() );
            }
        }

        template< typename T >
        void write( const T &value )
        {
            writeRawBytes( &value, sizeof( T ) );
        }

        virtual void writeRawBytes( const void *bytes, size_t size ) = 0;

        //@}

        /**
            \brief Reading properties
        */
        //@{

    public:
        void read( std::string &str );
        void read( VertexFormat &vf );

        template< size_t SIZE, typename PRECISION >
        void read( Vector< SIZE, PRECISION > &v )
        {
            PRECISION data[ SIZE ];
            readRawBytes( &data[ 0 ], SIZE * sizeof( PRECISION ) );
            v = Vector< SIZE, PRECISION >( data );
        }

        template< size_t SIZE, typename PRECISION >
        void read( Matrix< SIZE, PRECISION > &m )
        {
            PRECISION data[ SIZE ];
            readRawBytes( &data[ 0 ], SIZE * sizeof( PRECISION ) );
            m = Matrix< SIZE, PRECISION >( data );
        }

        void read( Quaternion4f &q );
        void read( Transformation &t );

        template< class T >
        void read( SharedPointer< T > &ptr )
        {
            StreamObject::StreamObjectId objId;
            read( objId );

            if ( objId == 0 ) {
                return;
            }

            auto obj = _objects[ objId ];
            if ( obj == nullptr ) {
                Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot find object with id ", objId );
                return;
            }

            ptr = crimild::cast_ptr< T >( obj );
        }

        template< class T >
        void read( std::vector< SharedPointer< T >> &objs )
        {
            size_t count;
            read( count );

            for ( int i = 0; i < count; i++ ) {
                StreamObject::StreamObjectId objId;
                read( objId );
                auto obj = _objects[ objId ];
                if ( obj == nullptr ) {
                    Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot find object with id ", objId );
                    continue;
                }
                objs.push_back( crimild::cast_ptr< T >( obj ) );
            }
        }

        template< typename T >
        void read( T &value )
        {
            readRawBytes( &value, sizeof( T ) );
        }

        virtual void readRawBytes( void *bytes, size_t size ) = 0;

        //@}
    };

}

#endif

