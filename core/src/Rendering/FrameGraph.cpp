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

#include "Rendering/FrameGraph.hpp"

#include "Rendering/CommandBuffer.hpp"
#include "Rendering/RenderPass.hpp"

#include "Foundation/Log.hpp"

using namespace crimild;

crimild::Bool FrameGraph::compile( void ) noexcept
{
    CRIMILD_LOG_TRACE( "Compiling frame graph" );

	auto output = getOutput();
	if ( output == nullptr ) {
		CRIMILD_LOG_ERROR( "Cannot compile frame graph: No output provided" );
		return false;
	}

	verifyAllConnections();

    m_reversedGraph = m_graph.reverse();

    auto sorted = m_graph.sort();
//    auto connected = m_reversedGraph.connected( output );
//    connected.insert( output );

    m_sorted.clear();
    m_sortedByType.clear();
    sorted.each(
    	[ &/*, connected*/ ]( auto node ) {
//        	if ( connected.contains( node ) ) {
//                // Discard the node since it's not connected with the
//                // final output for this render graph.
//                CRIMILD_LOG_DEBUG( "Discarding ", node->getName() );
//                return;
//        	}
            m_sorted.add( node );
        	m_sortedByType[ node->type ].add( node );
    	}
    );

	return true;
}

void FrameGraph::verifyAllConnections( void ) noexcept
{
	m_graph.eachVertex(
		[&]( auto node ) {
			switch ( node->type ) {
				case Node::Type::ATTACHMENT:
					verifyConnections< Attachment >( node );
					break;

				case Node::Type::RENDER_PASS:
					verifyConnections< RenderPass >( node );
					break;

				case Node::Type::RENDER_SUBPASS:
					verifyConnections< RenderSubpass >( node );
					break;

				default:
					break;
			}
		}
	);
}

void FrameGraph::verifyConnections( Attachment *attachment ) noexcept
{
	// An attachment has no dependencies?
}

void FrameGraph::verifyConnections( RenderPass *renderPass ) noexcept
{
	renderPass->attachments.each( [&]( auto &attachment ) {
		// The render pass depends on the attachment
		connect( getNode( attachment ), getNode( renderPass ) );
	});
	
	renderPass->subpasses.each( [&]( auto &subpass ) {
		// The render pass depends on each subpass
		connect( getNode( subpass ), getNode( renderPass ) );
	});
}

void FrameGraph::verifyConnections( RenderSubpass *subpass ) noexcept
{
	subpass->colorAttachments.each( [&]( auto &ref ) {
		// A subpass writes into color attachments
        connect( getNode( subpass ), getNode( ref.attachment ) );
	});
}

void FrameGraph::connect( Node *src, Node *dst ) noexcept
{
	m_graph.addEdge( src, dst );
}

FrameGraph::CommandBufferArray FrameGraph::recordCommands( void ) noexcept
{
    CommandBufferArray ret;

    ret.add(
        [&] {
        	auto commands = crimild::alloc< CommandBuffer >();
            commands->begin( CommandBuffer::Usage::SIMULTANEOUS_USE );

//        	auto getNextRenderPass = [ &, idx = 0 ]() mutable {
//                return getNodeObject< RenderPass >( m_sortedByType[ Node::Type::RENDER_PASS ][ idx++ ] );
//        	};
//
//            auto getNextRenderSubpass = [ &, idx = 0 ]() mutable {
//                return getNodeObject< RenderSubpass >( m_sortedByType[ Node::Type::RENDER_SUBPASS ][ idx++ ] );
//            };
//
//            auto currentRenderPass = getNextRenderPass();
//            auto currentRenderSubpass = getNextRenderSubpass();
//
//            commands->beginRenderPass( currentRenderPass, nullptr );
//            commands->beginRenderSubpass( currentRenderSubpass );
//
//        	m_sorted.each( [&]( auto &node ) {
//                switch ( node->type ) {
//                    case Node::Type::RENDER_PASS: {
//                        if ( currentRenderPass != nullptr ) {
//                            commands->endRenderPass( currentRenderPass );
//                        }
//                        currentRenderPass = getNextRenderPass();
//                        if ( currentRenderPass != nullptr ) {
//                            commands->beginRenderPass( currentRenderPass, nullptr );
//                        }
//                        break;
//                    }
//
//                    case Node::Type::RENDER_SUBPASS: {
//                        if ( currentRenderSubpass != nullptr ) {
//                            commands->endRenderSubpass( currentRenderSubpass );
//                        }
//                        currentRenderSubpass = getNextRenderSubpass();
//                        if ( currentRenderSubpass != nullptr ) {
//                            commands->beginRenderSubpass( currentRenderSubpass );
//                        }
////                        auto subpass = getNodeObject< RenderSubpass >( node );
////                        commands->beginRenderSubpass( subpass );
////                        if ( auto cmds = crimild::get_ptr( subpass->commands ) ) {
////                            commands->bindCommandBuffer( cmds );
////                        }
////                        commands->endRenderSubpass( subpass );
//                        break;
//                    }
//
//                    default:
//                        // ignored
//                        break;
//                }
//            });
//
//        	if ( renderPass != nullptr ) {
//                commands->endRenderPass( renderPass );
//                renderPass = nullptr;
//        	}

            m_sortedByType[ Node::Type::RENDER_PASS ].each( [&]( auto node ) {
                if ( auto renderPass = getNodeObject< RenderPass >( node ) ) {
                	commands->beginRenderPass( renderPass, nullptr );

                    renderPass->subpasses.each( [&]( auto &ptr ) {
                        if ( auto subpass = crimild::get_ptr( ptr ) ) {
                            commands->beginRenderSubpass( subpass );
                            if ( auto cmds = crimild::get_ptr( subpass->commands ) ) {
                                commands->bindCommandBuffer( cmds );
                            }
                            commands->endRenderSubpass( subpass );
                        }
                    });

                	commands->endRenderPass( renderPass );
                }
            });

            commands->end();
        	return commands;
    	}()
    );

    return ret;
}
