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

#include "MemoryEncoder.hpp"

#include "Tags.hpp"

#include <Crimild_Foundation.hpp>

using namespace crimild;
using namespace crimild::coding;

MemoryEncoder::MemoryEncoder( void )
{
}

MemoryEncoder::~MemoryEncoder( void )
{
}

crimild::Bool MemoryEncoder::encode( SharedPointer< Codable > const &obj )
{
   if ( obj == nullptr ) {
      return false;
   }

   if ( !ObjectFactory::getInstance()->hasBuilder( obj->getClassName() ) ) {
      CRIMILD_LOG_WARNING( "Ignoring object with unkown type: ", obj->getClassName() );
      return false;
   }

   if ( _sortedObjects.contains( obj ) ) {
      // object already register, remove it so it will be reinserted
      // again with a higher priority
      _sortedObjects.remove( obj );
      _sortedObjects.push( obj );
      return true;
   }

   _sortedObjects.push( obj );

   if ( _parent == nullptr ) {
      _roots.add( obj );
   }

   auto temp = _parent;
   _parent = obj;

   obj->encode( *this );

   _parent = temp;

   return true;
}

crimild::Bool MemoryEncoder::encode( std::string key, SharedPointer< Codable > const &obj )
{
   if ( obj == nullptr ) {
      return false;
   }

   if ( !ObjectFactory::getInstance()->hasBuilder( obj->getClassName() ) ) {
      CRIMILD_LOG_WARNING( "Ignoring object with unkown type: ", obj->getClassName(), " (", key, ")" );
      return false;
   }

   auto parentID = _parent->getUniqueID();

   _links[ parentID ][ key ] = obj->getUniqueID();

   return encode( obj );
}

crimild::Bool MemoryEncoder::encode( std::string key, std::string value )
{
   crimild::Size L = value.length();
   encode( key + "_length", L );

   return encodeData( key, value );
}

void MemoryEncoder::encodeArrayBegin( std::string key, crimild::Size count )
{
   encode( key + "_size", count );
}

std::string MemoryEncoder::beginEncodingArrayElement( std::string key, crimild::Size index )
{
   std::stringstream ss;
   ss << key << "_" << index;
   return ss.str();
}

void MemoryEncoder::endEncodingArrayElement( std::string key, crimild::Size index )
{
   // no-op
}

void MemoryEncoder::encodeArrayEnd( std::string key )
{
   // no-op
}

ByteArray MemoryEncoder::getBytes( void ) const
{
   ByteArray result;

   // TODO: reserve memory space beforehand in order to
   // avoid resizing the resulting array
   // result.reserve( XXX );

   append( result, Tags::TAG_DATA_START );

   append( result, Tags::TAG_DATA_VERSION );
   append( result, getVersion().getDescription() );

   auto temp = _sortedObjects;
   while ( !temp.empty() ) {
      auto obj = temp.pop();
      if ( !ObjectFactory::getInstance()->hasBuilder( obj->getClassName() ) ) {
         // Ignore objects with unknown types
         continue;
      }
      append( result, Tags::TAG_OBJECT_BEGIN );
      append( result, obj->getUniqueID() );
      append( result, obj->getClassName() );
      if ( auto data = crimild::dynamic_cast_ptr< EncodedData >( obj ) ) {
         append( result, data->getBytes() );
      }
      append( result, Tags::TAG_OBJECT_END );
   }

   _links.each( [ &result ]( const Codable::UniqueID &key, const Map< std::string, Codable::UniqueID > &ls ) {
      ls.each( [ &result, key ]( const std::string &name, const Codable::UniqueID &value ) {
         append( result, Tags::TAG_LINK_BEGIN );
         append( result, key );
         append( result, name );
         append( result, value );
         append( result, Tags::TAG_LINK_END );
      } );
   } );

   _roots.each( [ &result ]( const SharedPointer< Codable > &obj ) {
      append( result, Tags::TAG_ROOT_OBJECT_BEGIN );
      append( result, obj->getUniqueID() );
      append( result, Tags::TAG_ROOT_OBJECT_END );
   } );

   append( result, Tags::TAG_DATA_END );

   return result;
}

void MemoryEncoder::append( ByteArray &out, crimild::Int8 value )
{
   appendRawBytes( out, sizeof( crimild::Int8 ), &value );
}

void MemoryEncoder::append( ByteArray &out, std::string value )
{
   ByteArray bytes( value.length() + 1 );
   memcpy( &bytes.getData()[ 0 ], value.c_str(), sizeof( crimild::Char ) * value.length() );
   bytes[ value.length() ] = '\0';
   append( out, bytes );
}

void MemoryEncoder::append( ByteArray &out, Codable::UniqueID value )
{
   appendRawBytes( out, sizeof( Codable::UniqueID ), &value );
}

void MemoryEncoder::append( ByteArray &out, const ByteArray &data )
{
   crimild::Size count = data.size();
   appendRawBytes( out, sizeof( crimild::Size ), &count );
   appendRawBytes( out, data.size(), data.getData() );
}

void MemoryEncoder::appendRawBytes( ByteArray &out, crimild::Size count, const void *data )
{
   for ( crimild::Size i = 0; i < count; i++ ) {
      out.add( static_cast< const crimild::Byte * >( data )[ i ] );
   }
}

std::string MemoryEncoder::dump( void )
{
   std::stringstream ss;

   ss << "Sorted Objects:\n";
   _sortedObjects.each( [ &ss ]( SharedPointer< Codable > &codable ) {
      ss << "\t" << codable->getUniqueID() << " " << codable->getClassName() << "\n";
   } );

   ss << "Links:\n";
   _links.each( [ &ss ]( const Codable::UniqueID &key, const Map< std::string, Codable::UniqueID > &ls ) {
      ls.each( [ &ss, key ]( const std::string &name, const Codable::UniqueID &value ) {
         ss << "\t" << key << " " << name << " " << value << "\n";
      } );
   } );

   ss << "Roots:\n";
   _roots.each( [ &ss ]( const SharedPointer< Codable > &obj ) {
      ss << "\t" << obj->getUniqueID() << "\n";
   } );

   return ss.str();
}
