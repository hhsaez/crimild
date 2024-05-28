/*
 * Copyright (c) 2002-present, H. Hernan Saez
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

#include "SceneGraph/PrefabNode.hpp"

#include "Coding/FileDecoder.hpp"
#include "Coding/FileEncoder.hpp"
#include "Crimild_Foundation.hpp"
#include "Visitors/ShallowCopy.hpp"
#include "Visitors/StartComponents.hpp"
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild;

std::unordered_map< std::string, std::unordered_set< PrefabNode * > > PrefabNode::s_prefabs;
std::unordered_map< std::string, std::shared_ptr< Node > > PrefabNode::s_instances;

PrefabNode::PrefabNode( const std::filesystem::path &path ) noexcept
    : m_path( path )
{
    load();
}

PrefabNode::~PrefabNode( void ) noexcept
{
    s_prefabs[ m_path.string() ].erase( this );
}

void PrefabNode::overrideInstance( void ) noexcept
{
    const auto key = m_path.string();

    // Update file
    coding::FileEncoder encoder;
    encoder.encode( retain( getNodeAt( 0 ) ) );
    encoder.write( m_path );

    // Update shared instance
    s_instances[ key ] = getNodeAt( 0 )->perform< ShallowCopy >();

    // Update all other prefabs using this instance
    auto others = s_prefabs[ key ];
    for ( auto other : others ) {
        if ( other != this ) {
            if ( other->isLinked() && !other->isEditable() ) {
                other->reloadInstance();
            }
        }
    }

    m_editable = false;
}

void PrefabNode::reloadInstance( void ) noexcept
{
    load();

    if ( hasNodes() ) {
        auto node = getNodeAt( 0 );
        node->perform( StartComponents() );
        node->perform( UpdateWorldState() );
    }
}

void PrefabNode::encode( coding::Encoder &encoder )
{
    Node::encode( encoder );

    encoder.encode( "path", m_path.string() );
    encoder.encode( "linked", m_linked );

    if ( !m_linked ) {
        Array< SharedPointer< Node > > nodes;
        for ( int i = 0; i < getNodeCount(); ++i ) {
            nodes.add( retain( getNodeAt( i ) ) );
        }
        encoder.encode( "nodes", nodes );
    }
}

void PrefabNode::decode( coding::Decoder &decoder )
{
    Node::decode( decoder );

    std::string pathStr;
    decoder.decode( "path", pathStr );
    m_path = { pathStr };
    decoder.decode( "linked", m_linked );

    if ( m_linked ) {
        load();
    } else {
        Array< SharedPointer< Node > > nodes;
        decoder.decode( "nodes", nodes );
        nodes.each(
            [ this ]( auto &node ) {
                attachNode( node );
            }
        );
    }
}

void PrefabNode::load( void ) noexcept
{
    const auto key = m_path.string();

    s_prefabs[ key ].erase( this );

    detachAllNodes();

    if ( !s_instances.contains( key ) ) {
        coding::FileDecoder decoder;
        decoder.read( key );
        if ( decoder.getObjectCount() == 0 ) {
            CRIMILD_LOG_ERROR( "Decoded file is empty: ", key );
            return;
        }
        if ( std::shared_ptr< Node > instance = decoder.getObjectAt< Node >( 0 ) ) {
            s_instances[ key ] = instance;
        } else {
            CRIMILD_LOG_ERROR( "File does not contains nodes: ", key );
        }
    }

    auto instance = s_instances[ key ];
    auto node = instance->perform< ShallowCopy >();
    attachNode( node );

    s_prefabs[ key ].insert( this );

    setName( m_path.filename().stem().string() );

    m_linked = true;
    m_editable = false;
}
