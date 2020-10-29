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

#include "Coding/Codable.hpp"
#include "Foundation/Containers/Array.hpp"
#include "Foundation/Containers/Digraph.hpp"
#include "Foundation/Containers/Map.hpp"
#include "Foundation/Containers/Set.hpp"
#include "Foundation/NamedObject.hpp"
#include "Foundation/SharedObject.hpp"
#include "Foundation/Singleton.hpp"
#include "Rendering/FrameGraphObject.hpp"

namespace crimild {

    class Attachment;
    class Buffer;
    class BufferView;
    class CommandBuffer;
    class ComputePass;
    class ComputePipeline;
    class DescriptorPool;
    class DescriptorSet;
    class DescriptorSetLayout;
    class GraphicsPipeline;
    class Image;
    class ImageView;
    class IndexBuffer;
    class PresentationMaster;
    class RenderPass;
    class Sampler;
    class Texture;
    class UniformBuffer;
    class VertexBuffer;

    class CustomOperation : public SharedObject {
    public:
        std::function< void( void ) > callback;
    };

    class FrameGraph;

    /**
	   \brief Frame Graph

	   [Hernan] I'm not sure if making this a singleton is a good idea. It might
	   be possible that we create a new frame graph for a different scene and then
	   switch them. What we might need is something more like an 'active' singleton.

	   \todo This class is not thread safe (yet);
	   \todo merge render passes.
	   \todo reuse image resources.
	   \todo update uniform buffers only when needed.
	*/
    class FrameGraph : public coding::Codable,
                       public DynamicSingleton< FrameGraph > {
        CRIMILD_IMPLEMENT_RTTI( crimild::FrameGraph )

    public:
        class Node : public coding::Codable, public NamedObject {
            CRIMILD_IMPLEMENT_RTTI( crimild::FrameGraph::Node )

        public:
            enum class Type {
                GRAPHICS_PIPELINE,
                BUFFER,
                BUFFER_VIEW,
                IMAGE,
                IMAGE_VIEW,
                INDEX_BUFFER,
                DESCRIPTOR_POOL,
                DESCRIPTOR_SET,
                DESCRIPTOR_SET_LAYOUT,
                COMMAND_BUFFER,
                ATTACHMENT,
                RENDER_PASS,
                SAMPLER,
                TEXTURE,
                UNIFORM_BUFFER,
                VERTEX_BUFFER,

                COMPUTE_PASS,
                COMPUTE_PIPELINE,

                PRESENTATION_MASTER,

                CUSTOM,
                CUSTOM_OPERATION,
                CUSTOM_RESOURCE,
            };

            Type type;

            // obj should inherit from FrameGraphObject
            FrameGraphObject *obj = nullptr;
        };

        using CommandBufferArray = Array< SharedPointer< CommandBuffer > >;

    public:
        virtual ~FrameGraph( void ) = default;

        Node::Type getNodeType( GraphicsPipeline * ) const { return Node::Type::GRAPHICS_PIPELINE; }
        Node::Type getNodeType( Buffer * ) const { return Node::Type::BUFFER; }
        Node::Type getNodeType( BufferView * ) const { return Node::Type::BUFFER_VIEW; }
        Node::Type getNodeType( Image * ) const { return Node::Type::IMAGE; }
        Node::Type getNodeType( ImageView * ) const { return Node::Type::IMAGE_VIEW; }
        Node::Type getNodeType( IndexBuffer * ) const { return Node::Type::INDEX_BUFFER; }
        Node::Type getNodeType( Texture * ) const { return Node::Type::TEXTURE; }
        Node::Type getNodeType( DescriptorSet * ) const { return Node::Type::DESCRIPTOR_SET; }
        Node::Type getNodeType( DescriptorSetLayout * ) const { return Node::Type::DESCRIPTOR_SET_LAYOUT; }
        Node::Type getNodeType( DescriptorPool * ) const { return Node::Type::DESCRIPTOR_POOL; }
        Node::Type getNodeType( CommandBuffer * ) const { return Node::Type::COMMAND_BUFFER; }
        Node::Type getNodeType( RenderPass * ) const { return Node::Type::RENDER_PASS; }
        Node::Type getNodeType( Attachment * ) const { return Node::Type::ATTACHMENT; }
        Node::Type getNodeType( Sampler * ) const { return Node::Type::SAMPLER; }
        Node::Type getNodeType( PresentationMaster * ) const { return Node::Type::PRESENTATION_MASTER; }
        Node::Type getNodeType( FrameGraphObject * ) const { return Node::Type::CUSTOM; }
        Node::Type getNodeType( UniformBuffer * ) const { return Node::Type::UNIFORM_BUFFER; }
        Node::Type getNodeType( VertexBuffer * ) const { return Node::Type::VERTEX_BUFFER; }

        Node::Type getNodeType( ComputePass * ) const { return Node::Type::COMPUTE_PASS; }
        Node::Type getNodeType( ComputePipeline * ) const { return Node::Type::COMPUTE_PIPELINE; }

        template< typename NodeObjectType >
        void add( SharedPointer< NodeObjectType > const objPtr ) noexcept
        {
            add( crimild::get_ptr( objPtr ) );
        }

        template< typename NodeObjectType >
        void add( NodeObjectType *obj ) noexcept
        {
            if ( contains( obj ) ) {
                return;
            }

            auto node = crimild::alloc< Node >();
            node->type = getNodeType( obj );
            node->obj = obj;
            auto weakNode = crimild::get_ptr( node );

            m_graph.addVertex( weakNode );
            m_invertedIndex[ obj ] = weakNode;
            m_nodesByType[ node->type ].add( weakNode );

            // Retain the new node
            m_nodes.insert( node );

            // Notify that the frame graph needs rebuilding
            m_dirty = true;
        }

        /**
		   \brief Removes an object from the graph

		   \todo This should trigger a frame graph compilation. Maybe I should use
		   a kind of dirty flag or similar.
		 */
        void remove( FrameGraphObject *obj ) noexcept;

        template< typename T >
        inline crimild::Bool contains( SharedPointer< T > const &ptr ) noexcept
        {
            return contains( crimild::get_ptr( ptr ) );
        }

        template< typename T >
        inline crimild::Bool contains( T *obj ) noexcept
        {
            return m_invertedIndex[ obj ] != nullptr;
        }

        template< typename T >
        inline Node *getNode( T *obj ) noexcept
        {
            return m_invertedIndex[ obj ];
        }

        template< typename T >
        inline Node *getNode( SharedPointer< T > const &obj ) noexcept
        {
            return getNode( crimild::get_ptr( obj ) );
        }

        inline const Array< Node * > &getSorted( void ) const noexcept
        {
            return m_sorted;
        }

        crimild::Bool hasNodes( void ) const noexcept
        {
            return !m_nodes.empty();
        }

        template< typename T, typename Fn >
        void each( Fn fn )
        {
            T *dummy = nullptr;
            m_nodes[ getNodeType( dummy ) ].each( [ & ]( auto &node ) {
                fn( static_cast< T * >( crimild::get_ptr( node->obj ) ) );
            } );
        }

        crimild::Bool isPresentation( SharedPointer< Attachment > const &attachment ) const noexcept;

        crimild::Bool isDirty( void ) const noexcept { return m_dirty; }

        crimild::Bool compile( void ) noexcept;

        CommandBufferArray recordCommands( void ) noexcept;
        CommandBufferArray recordComputeCommands( void ) noexcept;

        /**
		   \brief Get all nodes connected to the current one matching the given type

		   \warning Requires frame graph to be compiled first
		   \todo Should this be const'd?
		 */
        template< typename T, typename U >
        Set< T * > connected( U const &obj ) noexcept
        {
            auto node = getNode( obj );
            if ( node == nullptr ) {
                return {};
            }

            auto retType = getNodeType( static_cast< T * >( nullptr ) );
            auto conn = m_reversedGraph.connected( node );
            return m_reversedGraph
                .connected( node )
                .filter(
                    [ & ]( auto &node ) {
                        return node->type == retType;
                    } )
                .map(
                    [ & ]( auto &node ) {
                        return static_cast< T * >( node->obj );
                    } );
        }

    private:
        void verifyAllConnections( void ) noexcept;

        template< typename T >
        T *getNodeObject( Node *node ) noexcept
        {
            T *ret = nullptr;
            if ( node != nullptr && getNodeType( ret ) == node->type ) {
                ret = static_cast< T * >( node->obj );
            }
            return ret;
        }

        template< typename T >
        const T *getNodeObject( Node *node ) const noexcept
        {
            T *ret = nullptr;
            if ( node != nullptr && getNodeType( ret ) == node->type ) {
                ret = static_cast< T * >( node->obj );
            }
            return ret;
        }

        /**
           \brief Connect two nodes with a directed edge from src to dst

           Connecting nodes this way means that src preceeds dst or that dst
           has a dependency on src.
         */
        template< typename T, typename U >
        void connect( T *src, U *dst ) noexcept
        {
            auto srcNode = getNode( src );
            if ( srcNode == nullptr ) {
                add( src );
                srcNode = getNode( src );
            }

            auto dstNode = getNode( dst );
            if ( dstNode == nullptr ) {
                add( dst );
                dstNode = getNode( dst );
            }

            m_graph.addEdge( srcNode, dstNode );
        }

        PresentationMaster *getPresentationMaster( void ) noexcept;
        const PresentationMaster *getPresentationMaster( void ) const noexcept;

    private:
        Digraph< Node * > m_graph;
        Digraph< Node * > m_reversedGraph;
        Map< FrameGraphObject *, Node * > m_invertedIndex;
        Array< Node * > m_sorted;
        Map< Node::Type, Array< Node * > > m_sortedByType;
        Set< SharedPointer< Node > > m_nodes;
        Map< Node::Type, Array< Node * > > m_nodesByType;

        /**
		   \brief Keep tracks of changes in the graph

		   Whenever the frame graph is modified (i.e. adding or removing objects),
		   we might need to recompile it.

		   \todo Instead of a flag, we might keep track of what objects actually changed.
		   For example, new objects that were added or objects that were connected with
		   other ones that were removed. That way we know for sure which connections
		   have to be made and which passes need to recreate their command buffers.
		 */
        crimild::Bool m_dirty = false;
    };

    std::ostream &operator<<( std::ostream &out, FrameGraph::Node::Type node );

}

#endif
