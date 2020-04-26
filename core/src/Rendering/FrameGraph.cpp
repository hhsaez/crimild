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

	verifyAllConnections();

    m_reversedGraph = m_graph.reverse();

    auto sorted = m_graph.sort();
    sorted.each( []( auto &node ) { std::cout << int( node->type ) << std::endl; });

    auto presentPass = getNodeObject< PresentPass >( sorted.last() );
    if ( presentPass == nullptr ) {
        CRIMILD_LOG_ERROR( "Cannot obtain present pass node" );
        return false;
    }

    auto connected = m_reversedGraph.connected( getNode( presentPass ) );
    connected.insert( getNode( presentPass ) );

    m_sorted.clear();
    m_sortedByType.clear();
    sorted.each(
    	[ &, connected ]( auto node ) {
        	if ( !connected.contains( node ) ) {
                // Discard the node since it's not connected with the
                // final output for this render graph.
                CRIMILD_LOG_DEBUG( "Discarding (", int( node->type ), "): ", node->getName() );
                return;
        	}
            m_sorted.add( node );
        	m_sortedByType[ node->type ].add( node );
    	}
    );

	return true;
}

void FrameGraph::verifyAllConnections( void ) noexcept
{
    // Start with render passes since they hold all the information already
    m_nodesByType[ Node::Type::RENDER_PASS ].each(
       	[&]( auto &node ) {
            auto renderPass = getNodeObject< RenderPass >( node );
        	// A render pass depends on all of its subpasses and attachments
            renderPass->attachments.each( [&]( auto &att ) {
                connect( getNode( att ), getNode( renderPass ) );
            });
            renderPass->subpasses.each( [&]( auto &subpass ) {
                connect( getNode( subpass ), getNode( renderPass ) );
            });
    	}
   	);

    // Next, subpasses
    m_nodesByType[ Node::Type::RENDER_SUBPASS ].each(
        [&]( auto &node ) {
            auto subpass = getNodeObject< RenderSubpass >( node );
            subpass->inputs.each( [&]( auto &attachment ) {
                // An input attachment is read by a subpass
                connect( getNode( attachment ), getNode( subpass ) );
//                Node *origin = nullptr;
//                m_graph.eachEdge(
//                	getNode( attachment ),
//                    [&]( auto &node ) {
//                        if ( node->type == Node::Type::RENDER_PASS ) {
//                            origin = node;
//                        }
//                	}
//                );
//                if ( origin != nullptr ) {
//                    connect( origin, getNode( subpass ) );
//                }
            });
            subpass->outputs.each( [&]( auto &attachment ) {
                // A subpass writes into an output attachment
                connect( getNode( subpass ), getNode( attachment ) );
            });
    	}
    );

    // Finally, presentation pass
    m_nodesByType[ Node::Type::PRESENT_PASS ].each(
        [&]( auto &node ) {
            auto presentPass = getNodeObject< PresentPass >( node );
        	if ( auto color = crimild::get_ptr( presentPass->colorAttachment ) ) {
                // A color attachment is presented by a present pass
                connect( getNode( color ), getNode( presentPass ) );
                Node *origin = nullptr;
                m_graph.eachEdge(
                    getNode( color ),
                    [&]( auto &node ) {
                        if ( node->type == Node::Type::RENDER_PASS ) {
                            origin = node;
                        }
                    }
                );
                if ( origin != nullptr ) {
                    connect( origin, getNode( presentPass ) );
                }
                else {
                    CRIMILD_LOG_ERROR( "Color attachment has no origin" );
                }
        	}
        }
    );
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

