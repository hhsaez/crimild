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

#include "Node.hpp"

#include <list>
#include <functional>
#include <thread>

namespace crimild {

	class Group : public Node {
            
	public:
		explicit Group( std::string name = "" );
		virtual ~Group( void );

		bool hasNodes( void ) const { return ( getNodeCount() > 0 ); }
		unsigned int getNodeCount( void ) const { return _nodes.size(); }

		void attachNode( NodePtr const &node );
		void detachNode( NodePtr const &node );
		void detachAllNodes( void );

		/**
			\brief Gets a node at a given position

			\warning This method is inefficient. Use it carefully
		*/
		NodePtr getNodeAt( unsigned int index );

		template< typename T >
        SharedPointer< T > getNodeAt( unsigned int index )
		{
            return std::static_pointer_cast< T >( getNodeAt( index ) );
		}

		virtual void foreachNode( std::function< void( NodePtr const & ) > callback );

	protected:
		std::list< NodePtr > _nodes;

	public:
		virtual void accept( NodeVisitor &visitor ) override;

	};
    
    using GroupPtr = SharedPointer< Group >;

}

#endif

