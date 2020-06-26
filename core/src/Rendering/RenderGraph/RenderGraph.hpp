/*
 * Copyright (c) 2013-present, H. Hernan Saez
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

#ifndef CRIMILD_RENDERER_RENDER_GRAPH_
#define CRIMILD_RENDERER_RENDER_GRAPH_

#include "Foundation/SharedObject.hpp"
#include "Foundation/NamedObject.hpp"
#include "Foundation/Containers/Digraph.hpp"
#include "Foundation/Containers/Stack.hpp"
#include "Coding/Codable.hpp"
#include "Rendering/RenderTarget.hpp"

namespace crimild {

	class Renderer;
	class RenderQueue;
    class FrameBufferObject;

	namespace rendergraph {

		class RenderGraphPass;
		class RenderGraphAttachment;

		class RenderGraph : public coding::Codable {
			CRIMILD_IMPLEMENT_RTTI( crimild::rendergraph::RenderGraph )

		public:
			class Node : public Codable, public NamedObject {
			public:
				enum class Type {
					PASS,
					ATTACHMENT,
				};

                Node( std::string name ) : NamedObject( name ) { }
				virtual ~Node( void ) { }

				virtual Type getType( void ) const = 0;
			};
			
		public:
			RenderGraph( void );
			virtual ~RenderGraph( void );

			template< typename PASS_TYPE, typename... Args >
			PASS_TYPE *createPass( Args &&... args )
			{
				auto pass = crimild::alloc< PASS_TYPE >( this, std::forward< Args >( args )... );
				_passes.add( pass );
				_graph.addVertex( crimild::get_ptr( pass ) );
				return crimild::get_ptr( pass );
			}

			void eachPass( std::function< void( RenderGraphPass * ) > const &callback );

            RenderGraphAttachment *createAttachment( std::string name, crimild::Int64 hints );
			void eachAttachment( std::function< void( RenderGraphAttachment * ) > const &callback );

			void read( RenderGraphPass *pass, Array< RenderGraphAttachment * > const &attachments );
			void write( RenderGraphPass *pass, Array< RenderGraphAttachment * > const &attachments );			 

		private:
			Digraph< Node * > _graph;
            Digraph< Node * > _reversedGraph;
			Array< SharedPointer< RenderGraphPass >> _passes;
			Array< SharedPointer< RenderGraphAttachment >> _attachments;

		public:
			SharedPointer< FrameBufferObject > createFBO( Array< RenderGraphAttachment * > const &attachments );

		private:
            SharedPointer< RenderTarget > getRenderTarget( crimild::Int64 hints, const Vector2i &screenSize );

            void resetAttachment( RenderGraphAttachment *attachment );
            void releaseAttachment( RenderGraphAttachment *attachment );

        private:
            Map< crimild::Int64, Stack< SharedPointer< RenderTarget >>> _renderTargetCache;

		public:
			void compile( void );
			void execute( Renderer *renderer, RenderQueue *renderQueue );

		private:
			Array< RenderGraphPass * > _sortedPasses;

		public:
			void setOutput( RenderGraphAttachment *attachment );
			RenderGraphAttachment *getOutput( void ) { return crimild::get_ptr( _output ); }

		private:
			SharedPointer< RenderGraphAttachment > _output;
		};

	}

}

#endif

