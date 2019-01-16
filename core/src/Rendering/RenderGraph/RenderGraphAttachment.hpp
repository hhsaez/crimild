/*
 * Copyright (c) 2013-2018, H. Hernan Saez
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

#ifndef CRIMILD_RENDERER_RENDER_GRAPH_ATTACHMENT_
#define CRIMILD_RENDERER_RENDER_GRAPH_ATTACHMENT_

#include "RenderGraph.hpp"

namespace crimild {

	class Texture;
	class RenderTarget;

	namespace rendergraph {

		/**
		   \brief A descriptor for a attachment used for a render pass
		 */
		class RenderGraphAttachment : public RenderGraph::Node {
			CRIMILD_IMPLEMENT_RTTI( crimild::rendergraph::RenderGraphAttachment )

		public:
			class Hint {
			public:
				enum {
					HDR                     = 1 << 0,
                    RENDER_ONLY             = 1 << 1,
					PERSISTENT              = 1 << 2, 
					
					FORMAT_DEPTH            = 1 << 10,
					FORMAT_RGB              = 1 << 11,
					FORMAT_RGBA             = 1 << 12,
                    FORMAT_DEPTH_HDR        = FORMAT_DEPTH | HDR,
                    FORMAT_RGB_HDR          = FORMAT_RGB | HDR,
					FORMAT_RGBA_HDR         = FORMAT_RGBA | HDR,

					WRAP_CLAMP_TO_EDGES     = 1 << 18,
					WRAP_REPEAT             = 1 << 19,
					
					SIZE_32                 = 1 << 20,
					SIZE_64                 = 1 << 21,
					SIZE_128                = 1 << 22,
					SIZE_256                = 1 << 23,
					SIZE_512                = 1 << 24,
					SIZE_1024               = 1 << 25,
					SIZE_2048               = 1 << 26,
					SIZE_4096               = 1 << 27,
					SIZE_SCREEN_10          = 1 << 28,
					SIZE_SCREEN_25          = 1 << 29,
					SIZE_SCREEN_50          = 1 << 30,
					SIZE_SCREEN_100         = 1 << 31,
					SIZE_SCREEN_150         = 1 << 32,
					SIZE_SCREEN_200         = 1 << 33,
                    SIZE_FULLSCREEN = SIZE_SCREEN_100,
				};
			};
			
		public:
            RenderGraphAttachment( std::string name, crimild::Int64 hints = Hint::FORMAT_RGBA | Hint::SIZE_FULLSCREEN );
			virtual ~RenderGraphAttachment ( void );

			RenderGraph::Node::Type getType( void ) const override { return RenderGraph::Node::Type::ATTACHMENT; }

			void setHints( crimild::Int64 hints ) { _hints = hints; }
			crimild::Int64 getHints( void ) const { return _hints; }

		private:
			crimild::Int64 _hints = 0;

		public:
			void setTexture( Texture *texture );
            Texture *getTexture( void ) { return _renderTarget != nullptr ? _renderTarget->getTexture() : crimild::get_ptr( _texture ); }

			void setRenderTarget( RenderTarget *target );
			RenderTarget *getRenderTarget( void ) { return crimild::get_ptr( _renderTarget ); }

            void setReaderCount( crimild::Int32 readerCount ) { _readerCount = readerCount; }
            crimild::Int32 getReaderCount( void ) const { return _readerCount; }

		private:
			SharedPointer< Texture > _texture;
			SharedPointer< RenderTarget > _renderTarget;
            crimild::Int32 _readerCount;
		};

	}

}

#endif



