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

using namespace crimild;
using namespace crimild::coding;

MemoryEncoder::MemoryEncoder( void )
{

}

MemoryEncoder::~MemoryEncoder( void )
{

}
            
void MemoryEncoder::encode( SharedPointer< Codable > const &obj )
{
	if ( _sortedObjects.contains( obj ) ) {
		// object already register, remove it so it will be reinserted
		// again with a higher priority
		_sortedObjects.remove( obj );
	}
    
	_sortedObjects.push( obj );
    
	if ( _parent == nullptr ) {
		_roots.add( obj );
	}
    
	auto temp = _parent;
	_parent = obj;
    
	obj->encode( *this );
    
	_parent = temp;
}

void MemoryEncoder::encode( std::string key, SharedPointer< Codable > const &obj )
{
	auto parentID = _parent->getUniqueID();
    
	_links[ parentID ][ key ] = obj->getUniqueID();
    
	encode( obj );
}

void MemoryEncoder::encode( std::string key, std::string value )
{
	crimild::Size L = value.length();
	encode( key + "_length", L );
	
	auto encoded = crimild::alloc< EncodedData >( value );
	encode( key, crimild::cast_ptr< EncodedData >( encoded ) );
}

void MemoryEncoder::encode( std::string key, const Transformation &value )
{
	auto encoded = crimild::alloc< EncodedData >( value );
	encode( key, crimild::cast_ptr< EncodedData >( encoded ) );
}

void MemoryEncoder::encode( std::string key, crimild::Size value )
{
	auto encoded = crimild::alloc< EncodedData >( value );
	encode( key, crimild::cast_ptr< EncodedData >( encoded ) );
}

void MemoryEncoder::encode( std::string key, crimild::Int32 value )
{
	auto encoded = crimild::alloc< EncodedData >( value );
	encode( key, crimild::cast_ptr< EncodedData >( encoded ) );
}
            
void MemoryEncoder::encode( std::string key, crimild::Bool value )
{
	auto encoded = crimild::alloc< EncodedData >( value );
	encode( key, crimild::cast_ptr< EncodedData >( encoded ) );
}

void MemoryEncoder::encodeArrayBegin( std::string key, crimild::Size count )
{
	encode( key + "_size", count );
}

void MemoryEncoder::encodeArrayEnd( std::string key )
{
	// no-op
}

void MemoryEncoder::dump( void )
{
	std::cout << "Begin" << std::endl;
    
	std::cout << "\nObjects" << std::endl;
	auto temp = _sortedObjects;
	while ( !temp.empty() ) {
		auto obj = temp.pop();
		std::cout << obj->getUniqueID() << " " << obj->getClassName() << std::endl;
	}
    
	std::cout << "\nLinks" << std::endl;
	_links.each( []( const Codable::UniqueID &key, containers::Map< std::string, Codable::UniqueID > &ls ) {
		ls.each( [ key ]( const std::string &name, Codable::UniqueID &value ) {
			std::cout << key << " " << name << " " << value << std::endl;
		});
	});
    
	std::cout << "\nTop" << std::endl;
	_roots.each( []( const SharedPointer< Codable > &obj, crimild::Size ) {
		std::cout << obj->getUniqueID() << " " << obj->getClassName() << std::endl;
	});
    
	std::cout << "\nEnd" << std::endl;
}

