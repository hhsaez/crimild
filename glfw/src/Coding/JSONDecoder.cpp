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

#include "Coding/JSONDecoder.hpp"

#include "Foundation/Log.hpp"
#include "Foundation/ObjectFactory.hpp"

#include <fstream>
#include <sstream>

using namespace crimild;
using namespace crimild::coding;

crimild::Bool JSONDecoder::decode( std::string key, SharedPointer< coding::Codable > &codable )
{
    codable = m_links[ m_currentObj->getUniqueID() ][ key ];
    if ( codable == nullptr ) {
        CRIMILD_LOG_ERROR( "Cannot decode missing object with id", m_currentObj->getUniqueID(), " for key ", key );
        return false;
    }

    auto temp = m_currentObj;
    m_currentObj = codable;
    codable->decode( *this );
    m_currentObj = temp;

    return true;
}

crimild::Bool JSONDecoder::decode( std::string key, Transformation &value )
{
    return decode( key + "_mat", value.mat )
           && decode( key + "_invMat", value.invMat )
           && decode( key + "_contents", value.contents );
}

crimild::Size JSONDecoder::beginDecodingArray( std::string key )
{
    crimild::Size count = 0;
    decode( key + "_size", count );
    return count;
}

std::string JSONDecoder::beginDecodingArrayElement( std::string key, crimild::Size index )
{
    std::stringstream ss;
    ss << key << "_" << index;
    return ss.str();
}

void JSONDecoder::endDecodingArrayElement( std::string key, crimild::Size index )
{
    // no-op
}

void JSONDecoder::endDecodingArray( std::string key )
{
    // no-op
}

crimild::Bool JSONDecoder::fromJSON( const nlohmann::json &json )
{
    const auto version = json.at( "__VERSION__" ).get< std::string >();
    setVersion( Version( version ) );

    if ( json.contains( "__OBJS__" ) ) {
        for ( const auto &objJSON : json.at( "__OBJS__" ) ) {
            const auto className = objJSON.at( "__CLASS_NAME__" ).get< std::string >();
            const auto id = objJSON.at( "__ID__" ).get< Codable::UniqueID >();

            auto obj = crimild::dynamic_cast_ptr< Codable >( ObjectFactory::getInstance()->build( className ) );
            if ( obj == nullptr ) {
                CRIMILD_LOG_ERROR( "Cannot build object of class ", className );
                return false;
            }

            m_objects[ id ] = obj;
            m_encoded[ obj->getUniqueID() ] = objJSON;
        }
    }

    if ( json.contains( "__LINKS__" ) ) {
        const auto &links = json.at( "__LINKS__" );
        for ( const auto &[ parentKey, link ] : links.items() ) {
            Codable::UniqueID parentID;
            StringUtils::toValue( parentKey, parentID );
            if ( !m_objects.contains( parentID ) ) {
                CRIMILD_LOG_ERROR( "Cannot find link parent object with id \"", parentID, "\"" );
                return false;
            }

            auto parent = m_objects[ parentID ];

            for ( const auto &[ linkName, objID ] : link.items() ) {
                if ( !m_objects.contains( objID ) ) {
                    CRIMILD_LOG_ERROR( "Cannot find linked object with id \"", objID, "\"" );
                    return false;
                }

                m_links[ parent->getUniqueID() ][ linkName ] = m_objects[ objID ];
            }
        }
    }

    if ( json.contains( "__ROOTS__" ) ) {
        for ( const auto &rootJSON : json.at( "__ROOTS__" ) ) {
            const auto id = rootJSON.get< Codable::UniqueID >();

            auto obj = m_objects[ id ];
            if ( obj == nullptr ) {
                CRIMILD_LOG_ERROR( "Cannot find root object with id ", id );
                return false;
            }

            addRootObject( crimild::dynamic_cast_ptr< SharedObject >( obj ) );
        }
    }

    auto rootCount = getObjectCount();
    for ( crimild::Size i = 0; i < rootCount; i++ ) {
        m_currentObj = crimild::dynamic_cast_ptr< Codable >( getObjectAt< SharedObject >( i ) );
        if ( m_currentObj == nullptr ) {
            CRIMILD_LOG_ERROR( "Cannot obtain root node at index ", i );
            return false;
        }
        m_currentObj->decode( *this );
        m_currentObj = nullptr;
    }

    //     if ( flag == Tags::TAG_OBJECT_BEGIN ) {
    //         Codable::UniqueID objID;
    //         offset += read( bytes, objID, offset );

    //         std::string className;
    //         offset += read( bytes, className, offset );

    //         auto obj = crimild::dynamic_cast_ptr< Codable >( ObjectFactory::getInstance()->build( className ) );
    //         if ( obj == nullptr ) {
    //             Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot build object of type ", className );
    //             return false;
    //         }

    //         _objects[ objID ] = obj;

    //         if ( auto encoded = crimild::dynamic_cast_ptr< EncodedData >( obj ) ) {
    //             ByteArray data;
    //             offset += read( bytes, data, offset );
    //             encoded->setBytes( data );
    //         }

    //         offset += read( bytes, flag, offset );
    //         if ( flag != Tags::TAG_OBJECT_END ) {
    //             Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid data format. Expected ", Tags::TAG_OBJECT_END );
    //             return false;
    //         }
    //     } else if ( flag == Tags::TAG_LINK_BEGIN ) {
    //         Codable::UniqueID parentObjID;
    //         offset += read( bytes, parentObjID, offset );

    //         std::string linkName;
    //         offset += read( bytes, linkName, offset );

    //         Codable::UniqueID objID;
    //         offset += read( bytes, objID, offset );
    //         auto obj = _objects[ objID ];
    //         if ( obj == nullptr ) {
    //             Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid data format. Cannot find object with id ", objID );
    //             return false;
    //         }

    //         auto parent = _objects[ parentObjID ];
    //         if ( parent == nullptr ) {
    //             Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid data format. Cannot find object with id ", objID );
    //             return false;
    //         }
    //         _links[ parent->getUniqueID() ][ linkName ] = obj;

    //         offset += read( bytes, flag, offset );
    //         if ( flag != Tags::TAG_LINK_END ) {
    //             Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid data format. Expected ", Tags::TAG_LINK_END );
    //             return false;
    //         }
    //     } else if ( flag == Tags::TAG_ROOT_OBJECT_BEGIN ) {
    //         Codable::UniqueID objID;
    //         offset += read( bytes, objID, offset );

    //         auto obj = _objects[ objID ];
    //         if ( obj == nullptr ) {
    //             Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid data format. Cannot find object with id ", objID );
    //             return false;
    //         }

    //         addRootObject( crimild::dynamic_cast_ptr< SharedObject >( obj ) );

    //         offset += read( bytes, flag, offset );
    //         if ( flag != Tags::TAG_ROOT_OBJECT_END ) {
    //             Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid data format. Expected ", Tags::TAG_ROOT_OBJECT_END );
    //             return false;
    //         }
    //     } else {
    //         Log::error( CRIMILD_CURRENT_CLASS_NAME, "Unknown flag ", flag );
    //         return false;
    //     }
    // }

    // auto rootCount = getObjectCount();
    // for ( crimild::Size i = 0; i < rootCount; i++ ) {
    //     auto obj = crimild::dynamic_cast_ptr< Codable >( getObjectAt< SharedObject >( i ) );
    //     _currentObj = obj;
    //     obj->decode( *this );
    //     _currentObj = nullptr;
    // }

    return true;
}

Bool JSONDecoder::fromFile( std::string path )
{
    auto fs = std::ifstream( path, std::ios::in );
    nlohmann::json json;
    fs >> json;
    fs.close();

    fromJSON( json );

    return true;
}
