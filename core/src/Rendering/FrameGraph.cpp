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

#include "Rendering/UniformBuffer.hpp"
#include "Rendering/CommandBuffer.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/IndexBuffer.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/PresentationMaster.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/VertexBuffer.hpp"

#include "Foundation/Log.hpp"

using namespace crimild;

void FrameGraph::remove( FrameGraphObject *obj ) noexcept
{
	if ( auto node = getNode( obj ) ) {
		auto nodePtr = crimild::retain( node );
		m_nodes.remove( nodePtr );

		// Notify that the frame graph needs rebuilding
		m_dirty = true;
	}
}

crimild::Bool FrameGraph::compile( void ) noexcept
{
    CRIMILD_LOG_TRACE( "Compiling frame graph" );

	if ( !isDirty() ) {
		// The frame graph did not change, so there's no need to recompile it
		// Returns true because this is not an error
		return true;
	}

	verifyAllConnections();

    m_reversedGraph = m_graph.reverse();

    auto sorted = m_graph.sort();

    auto presentationMaster = getPresentationMaster();
    if ( presentationMaster == nullptr ) {
        CRIMILD_LOG_ERROR( "Cannot obtain present pass node" );
        return false;
    }

    auto connected = m_reversedGraph.connected( getNode( presentationMaster ) );
    connected.insert( getNode( presentationMaster ) );

    m_sorted.clear();
    m_sortedByType.clear();
    sorted.each(
    	[ &, connected ]( auto node ) {
        	if ( !connected.contains( node ) ) {
                // Discard the node since it's not connected with the
                // final output for this render graph.
                CRIMILD_LOG_DEBUG( "Discarding (", node->type, "): ", node->getName() );
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
	// Start with presentation masters
	m_nodesByType[ Node::Type::PRESENTATION_MASTER ].each(
		[&]( auto &node ) {
			auto present = getNodeObject< PresentationMaster >( node );
			if ( auto color = crimild::get_ptr( present->colorAttachment ) ) {
				// Add an edge from the color attachment to the master node
				// since latter depends on the former.
				connect(
					color,
					present
				);
			}
		}
	);

	// Next, render passes
	m_nodesByType[ Node::Type::RENDER_PASS ].each(
		[&]( auto &node ) {
			auto renderPass = getNodeObject< RenderPass >( node );
			renderPass->attachments.each(
				[&]( auto &attachmentPtr ) {
					// Render passes create new attachments, so add edges
					// from the former to the latter.
					if ( auto attachment = crimild::get_ptr( attachmentPtr ) ) {
						connect(
							renderPass,
							attachment
						);
					}
				}
			);

			// A render pass depends on its command buffers, so add
			// an edge going from the command buffer to the render pass
			if ( auto commands = crimild::get_ptr( renderPass->commands ) ) {
				connect(
					commands,
					renderPass
				);
			}
		}
	);

	// Like command buffers
	m_nodesByType[ Node::Type::COMMAND_BUFFER ].each(
		[&]( auto &node ) {
			auto commands = getNodeObject< CommandBuffer >( node );
			commands->each(
				[&]( auto &cmd ) {
					switch ( cmd.type ) {
						case CommandBuffer::Command::Type::BIND_GRAPHICS_PIPELINE:
							connect(
								cmd.pipeline,
								commands
							);
							break;

						case CommandBuffer::Command::Type::BIND_VERTEX_BUFFER:
							connect(
								crimild::get_ptr(
									crimild::cast_ptr< VertexBuffer >( cmd.obj )
								),								
								commands
							);
							break;
							
						case CommandBuffer::Command::Type::BIND_INDEX_BUFFER:
							connect(
								crimild::get_ptr(
									crimild::cast_ptr< IndexBuffer >( cmd.obj )
								),								
								commands
							);
							break;
							
						case CommandBuffer::Command::Type::BIND_DESCRIPTOR_SET:
							connect(
								crimild::get_ptr(
									crimild::cast_ptr< DescriptorSet >( cmd.obj )
								),								
								commands
							);
							break;
							
						default:
							// ignore
							break;
					}
				}
			);
		}
	);

    m_nodesByType[ Node::Type::VERTEX_BUFFER ].each(
        [&]( auto &node ) {
            auto vbo = getNodeObject< VertexBuffer >( node );
            connect( vbo->getBufferView(), vbo );
        }
    );

    m_nodesByType[ Node::Type::UNIFORM_BUFFER ].each(
        [&]( auto &node ) {
            auto ubo = getNodeObject< UniformBuffer >( node );
            connect( ubo->getBufferView(), ubo );
        }
    );

    m_nodesByType[ Node::Type::INDEX_BUFFER ].each(
        [&]( auto &node ) {
            auto ibo = getNodeObject< IndexBuffer >( node );
            connect( ibo->getBufferView(), ibo );
        }
    );

    m_nodesByType[ Node::Type::BUFFER_VIEW ].each(
        [&]( auto &node ) {
            auto view = getNodeObject< BufferView >( node );
            connect( view->getBuffer(), view );
        }
    );

	// Link descriptor sets
	m_nodesByType[ Node::Type::DESCRIPTOR_SET ].each(
		[&]( auto &node ) {
			auto ds = getNodeObject< DescriptorSet >( node );
			ds->descriptors.each(
				[&]( Descriptor &descriptor ) {
					switch ( descriptor.descriptorType ) {
						case DescriptorType::UNIFORM_BUFFER:
							connect(
								static_cast< Buffer * >( crimild::get_ptr( descriptor.obj ) ),
								ds
							);
							break;

						case DescriptorType::TEXTURE:
							connect(
								descriptor.get< Texture >(),
								ds
							);

						default:
							// ignore
							break;
					}
				}
			);
		}
	);

	// Link attachments with images
	m_nodesByType[ Node::Type::ATTACHMENT ].each(
		[&]( auto &node ) {
			auto attachment = getNodeObject< Attachment >( node );
			if ( auto imageView = crimild::get_ptr( attachment->imageView ) ) {
				if ( auto image = crimild::get_ptr( imageView->image ) ) {
					// This seems odd, but in order to keep correct dependecies
					// an attachment must be connected directly to the image
					// Then, the image is connected with its image view
					connect(
						attachment,
						image
					);
					connect(
						image,
						imageView
					);
				}
			}
		}
	);

	// Link textures
	m_nodesByType[ Node::Type::TEXTURE ].each(
		[&]( auto &node ) {
			auto texture = getNodeObject< Texture >( node );
			if ( auto imageView = crimild::get_ptr( texture->imageView ) ) {
				connect(
					imageView,
					texture
				);
			}
		}
	);
	
}

crimild::Bool FrameGraph::isPresentation( SharedPointer< Attachment > const &attachment ) const noexcept
{
	auto master = getPresentationMaster();
	if ( master == nullptr ) {
		return false;
	}
	return master->colorAttachment == attachment;
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
					if ( auto cmds = crimild::get_ptr( renderPass->commands ) ) {
						commands->bindCommandBuffer( cmds );
					}
                	commands->endRenderPass( renderPass );
                }
            });

            commands->end();
        	return commands;
    	}()
    );

    return ret;
}

const PresentationMaster *FrameGraph::getPresentationMaster( void ) const noexcept
{
	auto &masters = m_nodesByType[ Node::Type::PRESENTATION_MASTER ];
	if ( masters.empty() ) {
		return nullptr;
	}
	return getNodeObject< PresentationMaster >( masters.first() );
}

PresentationMaster *FrameGraph::getPresentationMaster( void ) noexcept
{
	auto &masters = m_nodesByType[ Node::Type::PRESENTATION_MASTER ];
	if ( masters.empty() ) {
		return nullptr;
	}
	return getNodeObject< PresentationMaster >( masters.first() );
}

std::ostream &crimild::operator<<( std::ostream &out, FrameGraph::Node::Type type )
{
	switch ( type ) {
		case FrameGraph::Node::Type::PIPELINE:
			out << "PIPELINE";
			break;

		case FrameGraph::Node::Type::BUFFER:
			out << "BUFFER";
			break;

        case FrameGraph::Node::Type::BUFFER_VIEW:
            out << "BUFFER_VIEW";
            break;

		case FrameGraph::Node::Type::IMAGE:
			out << "IMAGE";
			break;			

		case FrameGraph::Node::Type::IMAGE_VIEW:
			out << "IMAGE_VIEW";
			break;			

        case FrameGraph::Node::Type::INDEX_BUFFER:
            out << "INDEX_BUFFER";
            break;

		case FrameGraph::Node::Type::DESCRIPTOR_SET:
			out << "DESCRIPTOR_SET";
			break;			

        case FrameGraph::Node::Type::DESCRIPTOR_SET_LAYOUT:
            out << "DESCRIPTOR_SET_LAYOUT";
            break;

        case FrameGraph::Node::Type::DESCRIPTOR_POOL:
            out << "DESCRIPTOR_POOL";
            break;

		case FrameGraph::Node::Type::TEXTURE:
			out << "TEXTURE";
			break;			

		case FrameGraph::Node::Type::COMMAND_BUFFER:
			out << "COMMAND_BUFFER";
			break;			

		case FrameGraph::Node::Type::ATTACHMENT:
			out << "ATTACHMENT";
			break;			

		case FrameGraph::Node::Type::RENDER_PASS:
			out << "RENDER_PASS";
			break;			

        case FrameGraph::Node::Type::SAMPLER:
            out << "SAMPLER";
            break;

		case FrameGraph::Node::Type::PRESENTATION_MASTER:
			out << "PRESENTATION_MASTER";
			break;			

		default:
			out << "UNKNOWN (" << int( type ) << ")";
			break;
	}

	return out;
}

