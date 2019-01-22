/*
 * Copyright (c) 2002-present, H. Hernán Saez
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

#include "RenderGraph.hpp"
#include "RenderGraphPass.hpp"
#include "RenderGraphAttachment.hpp"

#include "Foundation/Containers/List.hpp"
#include "Foundation/Containers/Map.hpp"
#include "Exceptions/RuntimeException.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/FrameBufferObject.hpp"

using namespace crimild;
using namespace crimild::containers;
using namespace crimild::rendergraph;

RenderGraph::RenderGraph( void )
{

}

RenderGraph::~RenderGraph( void )
{

}


void RenderGraph::eachPass( std::function< void( RenderGraphPass * ) > const &callback )
{
	_passes.each( [ callback ]( SharedPointer< RenderGraphPass > const &pass ) {
		callback( crimild::get_ptr( pass ) );
	});
}

RenderGraphAttachment *RenderGraph::createAttachment( std::string name, crimild::Int64 hints )
{
	auto r = crimild::alloc< RenderGraphAttachment >( name, hints );
	_attachments.add( r );
	return crimild::get_ptr( r );
}

void RenderGraph::eachAttachment( std::function< void( RenderGraphAttachment * ) > const &callback )
{
	_attachments.each( [ callback ]( SharedPointer< RenderGraphAttachment > const &r ) {
		callback( crimild::get_ptr( r ) );
	});
}

void RenderGraph::read( RenderGraphPass *pass, Array< RenderGraphAttachment * > const &attachments )
{
	attachments.each( [ this, pass ]( RenderGraphAttachment *att ) {
		if ( att != nullptr ) {
			_graph.addEdge( att, pass );
		}
	});
}

void RenderGraph::write( RenderGraphPass *pass, Array< RenderGraphAttachment * > const &attachments )
{
	attachments.each( [ this, pass ]( RenderGraphAttachment *att ) {
		if ( att != nullptr ) {
			_graph.addEdge( pass, att );
		}
	});
}

SharedPointer< FrameBufferObject > RenderGraph::createFBO( containers::Array< RenderGraphAttachment * > const &attachments )
{
	auto renderer = Renderer::getInstance();
	auto screenWidth = renderer->getScreenBuffer()->getWidth();
	auto screenHeight = renderer->getScreenBuffer()->getHeight();
	auto screenSize = Vector2i( screenWidth, screenHeight );
	
	crimild::Int32 fboWidth = 0;
	crimild::Int32 fboHeight = 0;

	Map< std::string, SharedPointer< RenderTarget >> targets;

	attachments.each( [ this, &fboWidth, &fboHeight, &targets, screenSize ]( RenderGraphAttachment *att, crimild::Size index ) {
		if ( att == nullptr ) {
			return;
		}

        if ( att->getRenderTarget() == nullptr ) {
            auto hints = att->getHints();
            auto target = getRenderTarget( hints, screenSize );
            att->setRenderTarget( crimild::get_ptr( target ) );
        }

        auto target = crimild::retain( att->getRenderTarget() );

        fboWidth = Numericf::max( target->getWidth(), fboWidth );
        fboHeight = Numericf::max( target->getHeight(), fboHeight );

		std::stringstream ss;
		ss << index;
		targets.insert( ss.str(), target );
	});
	
	auto fbo = crimild::alloc< FrameBufferObject >( fboWidth, fboHeight );
	fbo->getRenderTargets() = targets;
	return fbo;
}

SharedPointer< RenderTarget > RenderGraph::getRenderTarget( crimild::Int64 hints, const Vector2i &screenSize )
{
    if ( _renderTargetCache.contains( hints ) && _renderTargetCache[ hints ].size() > 0 ) {
        return _renderTargetCache[ hints ].pop();
    }

    Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Creating render target with hints ", hints );

    crimild::Bool renderOnly = hints & RenderGraphAttachment::Hint::RENDER_ONLY;
    crimild::Bool useFloatTexture = hints & RenderGraphAttachment::Hint::HDR;

    auto output = renderOnly ? RenderTarget::Output::RENDER : RenderTarget::Output::TEXTURE;

    auto type = RenderTarget::Type::COLOR_RGBA;
    if ( hints & RenderGraphAttachment::Hint::FORMAT_DEPTH ) {
        if ( hints & RenderGraphAttachment::Hint::HDR ) {
            type = RenderTarget::Type::DEPTH_32;
        } else {
            type = RenderTarget::Type::DEPTH_24;
        }
    }
    else if ( hints & RenderGraphAttachment::Hint::FORMAT_RGB ) {
        type = RenderTarget::Type::COLOR_RGB;
    }

    auto size = screenSize;
    if ( hints & RenderGraphAttachment::Hint::SIZE_SCREEN_10 ) size = screenSize / 10;
    else if ( hints & RenderGraphAttachment::Hint::SIZE_SCREEN_25 ) size = screenSize / 4;
    else if ( hints & RenderGraphAttachment::Hint::SIZE_SCREEN_50 ) size = screenSize / 2;
    else if ( hints & RenderGraphAttachment::Hint::SIZE_SCREEN_150 ) size = screenSize * 1.5;
    else if ( hints & RenderGraphAttachment::Hint::SIZE_SCREEN_200 ) size = screenSize * 2;
    else if ( hints & RenderGraphAttachment::Hint::SIZE_32 ) size = Vector2i( 32, 32 );
    else if ( hints & RenderGraphAttachment::Hint::SIZE_64 ) size = Vector2i( 64, 64 );
    else if ( hints & RenderGraphAttachment::Hint::SIZE_128 ) size = Vector2i( 128, 128 );
    else if ( hints & RenderGraphAttachment::Hint::SIZE_256 ) size = Vector2i( 256, 256 );
	else if ( hints & RenderGraphAttachment::Hint::SIZE_512 ) size = Vector2i( 512, 512 );
    else if ( hints & RenderGraphAttachment::Hint::SIZE_1024 ) size = Vector2i( 1024, 1024 );
	else if ( hints & RenderGraphAttachment::Hint::SIZE_2048 ) size = Vector2i( 2048, 2048 );
	else if ( hints & RenderGraphAttachment::Hint::SIZE_4096 ) size = Vector2i( 4096, 4096 );

    auto target = crimild::alloc< RenderTarget >( type, output, size.x(), size.y(), useFloatTexture );

    if ( hints & RenderGraphAttachment::Hint::BORDER_ONE ) {
        target->getTexture()->setBorderColor( RGBAColorf::ONE );
    }

	if ( hints & RenderGraphAttachment::Hint::WRAP_REPEAT ) {
		target->getTexture()->setWrapMode( Texture::WrapMode::REPEAT );
	}

    return target;
}

void RenderGraph::compile( void )
{
    auto output = getOutput();
    if ( output == nullptr ) {
        CRIMILD_LOG_ERROR( "No output attachment provided for render graph" );
        return;
    }

	// make sure all connections are set
	_passes.each( [ this ]( SharedPointer< RenderGraphPass > const &pass ) {
		pass->setup( this );
	});

    _reversedGraph = _graph.reverse();
	auto sorted = _graph.sort();
    auto connected = _reversedGraph.connected( getOutput() );
    connected.insert( output );

	_sortedPasses.clear();
	sorted.each( [ this, &connected ]( RenderGraph::Node *node ) {
        if ( !connected.contains( node ) ) {
            // Discard the node since it's not connected with the
            // final output for this render graph.
			CRIMILD_LOG_DEBUG( "Discarding ", node->getName() );
            return;
        }
		if ( node->getType() == RenderGraph::Node::Type::PASS ) {
			_sortedPasses.add( static_cast< RenderGraphPass * >( node ) );
		}
	});

    std::stringstream ss;

    _sortedPasses.each( [ this, &ss ]( RenderGraphPass *pass ) {
        ss << "Pass: " << pass->getName() << "\n";

        ss << "\tInputs: ";
        _reversedGraph.eachEdge( pass, [ &ss ]( Node *attachment ) {
            ss << attachment->getName() << ", ";
        });
        ss << "\n";

        ss << "\tOutputs: ";
        _graph.eachEdge( pass, [ &ss ]( Node *attachment ) {
            ss << attachment->getName() << ", ";
        });
        ss << "\n";
    });

    Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Render Graph compiled:\n", ss.str() );
}

void RenderGraph::execute( Renderer *renderer, RenderQueue *renderQueue )
{
	if ( _sortedPasses.size() == 0 ) {
		compile();
	}
	
	_sortedPasses.each( [ this, renderer, renderQueue ]( RenderGraphPass *pass ) {
		pass->execute( this, renderer, renderQueue );

        _graph.eachEdge( pass, [ this ]( Node *output ) {
            resetAttachment( static_cast< RenderGraphAttachment * >( output ) );
        });

        _reversedGraph.eachEdge( pass, [ this ]( Node *input ) {
            releaseAttachment( static_cast< RenderGraphAttachment * >( input ) );
        });
	});
}

void RenderGraph::resetAttachment( RenderGraphAttachment *attachment )
{
    if ( getOutput() == attachment ) {
        return;
    }

    attachment->setReaderCount( _graph.getEdgeCount( attachment ) );
}

void RenderGraph::releaseAttachment( RenderGraphAttachment *attachment )
{
    if ( getOutput() == attachment ) {
        return;
    }

    attachment->setReaderCount( attachment->getReaderCount() - 1 );

	auto hints = attachment->getHints();
	if ( hints & RenderGraphAttachment::Hint::PERSISTENT ) {
		return;
	}
	
    if ( attachment->getReaderCount() <= 0 ) {
        auto target = crimild::retain( attachment->getRenderTarget() );
        _renderTargetCache[ hints ].push( target );
        attachment->setRenderTarget( nullptr );
    }
}

void RenderGraph::setOutput( RenderGraphAttachment *output )
{
	_output = crimild::retain( output );
}

