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

#ifndef CRIMILD_CORE_RENDERING_FRAME_GRAPH_
#define CRIMILD_CORE_RENDERING_FRAME_GRAPH_

#include "Foundation/NamedObject.hpp"
#include "Foundation/SharedObject.hpp"
#include "Foundation/Containers/Array.hpp"
#include "Foundation/Containers/Digraph.hpp"
#include "Foundation/Containers/Map.hpp"
#include "Foundation/Containers/Set.hpp"
#include "Coding/Codable.hpp"

namespace crimild {

	class Attachment;
    class CommandBuffer;
    class PresentPass;
	class RenderPass;	
	class RenderSubpass;

    class CustomOperation : public SharedObject {
    public:
        std::function< void( void ) > callback;
    };

	class FrameGraph : public coding::Codable {
		CRIMILD_IMPLEMENT_RTTI( crimild::FrameGraph )
		
	public:
		class Node : public coding::Codable, public NamedObject {
            CRIMILD_IMPLEMENT_RTTI( crimild::FrameGraph::Node )

        public:
			enum class Type {
				ATTACHMENT,
				RENDER_PASS,
				RENDER_SUBPASS,
                PRESENT_PASS,

                CUSTOM_OPERATION,
                CUSTOM_RESOURCE,
			};

			Type type;
			SharedPointer< SharedObject > obj;
		};

        using CommandBufferArray = containers::Array< SharedPointer< CommandBuffer >>;

	public:
		virtual ~FrameGraph( void ) = default;

		Node::Type getNodeType( RenderPass * ) { return Node::Type::RENDER_PASS; }
		Node::Type getNodeType( RenderSubpass * ) { return Node::Type::RENDER_SUBPASS; }		
		Node::Type getNodeType( Attachment * ) { return Node::Type::ATTACHMENT; }
        Node::Type getNodeType( PresentPass * ) { return Node::Type::PRESENT_PASS; }

        template< typename NodeObjectType >
        void add( SharedPointer< NodeObjectType > const objPtr ) noexcept
        {
            auto obj = crimild::get_ptr( objPtr );
            if ( contains( obj ) ) {
                return;
            }

            auto node = crimild::alloc< Node >();
            node->type = getNodeType( obj );
            node->obj = objPtr;
            auto weakNode = crimild::get_ptr( node );

            m_graph.addVertex( weakNode );
            m_invertedIndex[ obj ] = weakNode;
            m_nodesByType[ node->type ].add( weakNode );

            // Retain the new node
            m_nodes.insert( node );
        }
		
		template< typename NodeObjectType >
		SharedPointer< NodeObjectType > create( void ) noexcept
		{
			auto obj = crimild::alloc< NodeObjectType >();
            add( obj );
            return obj;
		}

        template< typename T >
        inline crimild::Bool contains( SharedPointer< T > const &ptr ) noexcept
        {
            return contains( crimild::get_ptr( ptr ) );
        }

        inline crimild::Bool contains( SharedObject *obj ) noexcept
        {
            return m_invertedIndex[ obj ] != nullptr;
        }

        inline Node *getNode( SharedObject *obj ) noexcept
        {
            return m_invertedIndex[ obj ];
        }

		inline Node *getNode( SharedPointer< SharedObject > const &obj ) noexcept
		{
			return getNode( crimild::get_ptr( obj ) );
		}

        inline const containers::Array< Node * > &getSorted( void ) const
        {
            return m_sorted;
        }

        template< typename T, typename Fn >
        void each( Fn fn )
        {
            T *dummy = nullptr;
            m_nodes[ getNodeType( dummy )].each( [&]( auto &node ) {
                fn( static_cast< T * >( crimild::get_ptr( node->obj ) ) );
            });
        }

        crimild::Bool compile( void ) noexcept;

        CommandBufferArray recordCommands( void ) noexcept;

	private:
		void verifyAllConnections( void ) noexcept;

        template< typename T >
        T *getNodeObject( Node *node ) noexcept
        {
            T *ret = nullptr;
            if ( node != nullptr && getNodeType( ret ) == node->type ) {
                ret = static_cast< T * >( crimild::get_ptr( node->obj ) );
            }
            return ret;
        }

		void connect( Node *src, Node *dst ) noexcept;

	private:
		containers::Digraph< Node * > m_graph;
		containers::Digraph< Node * > m_reversedGraph;
		containers::Map< SharedObject *, Node * > m_invertedIndex;
        containers::Array< Node * > m_sorted;
		containers::Map< Node::Type, containers::Array< Node * >> m_sortedByType;
        containers::Set< SharedPointer< Node >> m_nodes;
        containers::Map< Node::Type, containers::Array< Node * >> m_nodesByType;
	};

    class PresentPass : public SharedObject {
    public:
        virtual ~PresentPass( void ) = default;

        SharedPointer< Attachment > colorAttachment;
    };

}

#endif
