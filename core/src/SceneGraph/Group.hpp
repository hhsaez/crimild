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

#ifndef CRIMILD_SCENE_GRAPH_GROUP_
#define CRIMILD_SCENE_GRAPH_GROUP_

#include "Foundation/Containers/Array.hpp"
#include "Node.hpp"

#include <functional>
#include <thread>

namespace crimild {

    class Group : public Node {
        CRIMILD_IMPLEMENT_RTTI( crimild::Group )

    public:
        explicit Group( std::string name = "" );
        virtual ~Group( void );

        bool hasNodes( void ) const { return !_nodes.empty(); }
        unsigned int getNodeCount( void ) const { return _nodes.size(); }

        void attachNode( Node *node );
        void attachNode( SharedPointer< Node > const &node );

        void detachNode( Node *node );
        void detachNode( SharedPointer< Node > const &node );

        void detachAllNodes( void );

        /**
            \brief Gets a node at a given position
        */
        Node *getNodeAt( unsigned int index );

        template< typename T >
        T *getNodeAt( unsigned int index )
        {
            return static_cast< T * >( getNodeAt( index ) );
        }

        Node *getNode( std::string name );

        template< typename T >
        T *getNode( std::string name )
        {
            return static_cast< T * >( getNode( name ) );
        }

        virtual void forEachNode( std::function< void( Node * ) > callback, bool skipDisabledNodes = true );

    protected:
        Array< SharedPointer< Node > > _nodes;

    public:
        virtual void accept( NodeVisitor &visitor ) override;

        /**
            \name Coding
         */
        //@{
    public:
        virtual void encode( coding::Encoder &encoder ) override;
        virtual void decode( coding::Decoder &decoder ) override;

        //@}
    };

}

#endif
