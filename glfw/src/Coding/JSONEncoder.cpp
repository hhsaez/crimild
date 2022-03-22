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

#include "JSONEncoder.hpp"

#include "Coding/Codable.hpp"
#include "Foundation/StringUtils.hpp"

#include <fstream>

using namespace crimild;
using namespace crimild::coding;

crimild::Bool JSONEncoder::encode( SharedPointer< Codable > const &codable )
{
    if ( codable == nullptr ) {
        return false;
    }

    if ( m_sortedObjects.contains( codable ) ) {
        // object already register, remove it so it will be reinserted
        // again with a higher priority
        m_sortedObjects.remove( codable );
        m_sortedObjects.push( codable );
        return true;
    }

    m_sortedObjects.push( codable );

    if ( m_parent == nullptr ) {
        m_roots.add( codable );
    }

    auto temp = m_parent;
    m_parent = codable;

    m_encoded[ codable->getUniqueID() ] = {
        { "__CLASS_NAME__", codable->getClassName() },
        { "__ID__", codable->getUniqueID() },
    };

    codable->encode( *this );

    m_parent = temp;

    return true;
}

crimild::Bool JSONEncoder::encode( std::string key, SharedPointer< Codable > const &codable )
{
    if ( codable == nullptr ) {
        return false;
    }

    auto parentID = m_parent->getUniqueID();

    m_links[ parentID ][ key ] = codable->getUniqueID();

    return encode( codable );
}

crimild::Bool JSONEncoder::encode( std::string key, const Transformation &value )
{
    encode( key + "_mat", value.mat );
    encode( key + "_invMat", value.invMat );
    encode( key + "_contents", value.contents );
    return true;
}

void JSONEncoder::encodeArrayBegin( std::string key, crimild::Size count )
{
    std::cout << "encodeArrayBegin " << key << " " << count << std::endl;
    encode( key + "_size", count );
}

std::string JSONEncoder::beginEncodingArrayElement( std::string key, crimild::Size index )
{
    std::stringstream ss;
    ss << key << "_" << index;
    std::cout << "beginEncodingArrayElement " << ss.str() << std::endl;
    return ss.str();
}

void JSONEncoder::endEncodingArrayElement( std::string key, crimild::Size index )
{
    // no-op
}

void JSONEncoder::encodeArrayEnd( std::string key )
{
    // no-op
}

nlohmann::json JSONEncoder::getResult( void ) const
{
    // return m_json;

    nlohmann::json json;

    json[ "__VERSION__" ] = getVersion().getDescription();

    auto temp = m_sortedObjects;
    while ( !temp.empty() ) {
        auto obj = temp.pop();
        auto &objJSON = m_encoded[ obj->getUniqueID() ];
        if ( objJSON != nullptr ) {
            json[ "__OBJS__" ].push_back( objJSON );
        }
    }

    m_links.each(
        [ & ]( const Codable::UniqueID &uniqueID, const Map< std::string, Codable::UniqueID > &ls ) {
            const auto key = StringUtils::toString( uniqueID );
            ls.each(
                [ & ]( const std::string &name, const Codable::UniqueID &value ) {
                    json[ "__LINKS__" ][ key ][ name ] = value;
                } );
        } );

    m_roots.each(
        [ & ]( const SharedPointer< Codable > &obj ) {
            json[ "__ROOTS__" ].push_back( obj->getUniqueID() );
        } );

    return json;
}

void JSONEncoder::write( std::string path ) const
{
    const auto json = getResult();

    auto fs = std::ofstream( path, std::ios::out );
    fs << json.dump( 2 );
    fs.close();
}
