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

#include "RenderSystem.hpp"

#include "Rendering/FrameGraphOperation.hpp"
#include "Rendering/Operations/Operations.hpp"
#include "Rendering/RenderPass.hpp"
#include "Simulation/Simulation.hpp"

#include <queue>
#include <unordered_map>
#include <unordered_set>

namespace crimild {

    namespace utils {

        template< typename Fn >
        void traverse( SharedPointer< FrameGraphOperation > root, Fn fn ) noexcept
        {
            std::queue< SharedPointer< FrameGraphOperation > > frontier;
            std::unordered_set< FrameGraphOperation * > expanded;
            std::unordered_map< FrameGraphOperation *, size_t > blockCount;

            frontier.push( root );
            while ( !frontier.empty() ) {
                auto current = frontier.front();
                frontier.pop();
                if ( expanded.count( current.get() ) != 0 ) {
                    // already expanded
                    continue;
                }
                expanded.insert( current.get() );
                fn( current );
                current->eachBlockedBy(
                    [ & ]( auto pass ) {
                        if ( blockCount.count( pass.get() ) == 0 ) {
                            blockCount[ pass.get() ] = pass->getBlockCount();
                        }

                        --blockCount[ pass.get() ];
                        if ( blockCount[ pass.get() ] == 0 ) {
                            frontier.push( pass );
                        }
                    } );
            }
        }

    }
}

using namespace crimild;

void RenderSystem::lateStart( void ) noexcept
{
    if ( getFrameGraph() != nullptr ) {
        // Frame graph is valid. Nothing to do here
        return;
    }

    setFrameGraph(
        [] {
            using namespace crimild::framegraph;
            // return present( forwardUnlitPass( nullptr ) );
            return present( gBufferPass()->getProduct( 2 ) );
        }() );
}

#include <queue>
#include <unordered_map>
#include <unordered_set>

template< typename Fn >
void traverse( SharedPointer< FrameGraphOperation > root, Fn fn ) noexcept
{
    std::queue< SharedPointer< FrameGraphOperation > > frontier;
    std::unordered_set< FrameGraphOperation * > expanded;
    std::unordered_map< FrameGraphOperation *, size_t > blockCount;

    frontier.push( root );
    while ( !frontier.empty() ) {
        auto current = frontier.front();
        frontier.pop();
        if ( expanded.count( get_ptr( current ) ) != 0 ) {
            // already expanded
            continue;
        }
        expanded.insert( crimild::get_ptr( current ) );
        fn( current );
        current->eachBlockedBy(
            [ & ]( auto passPtr ) {
                auto pass = get_ptr( passPtr );
                if ( blockCount.count( pass ) == 0 ) {
                    blockCount[ pass ] = pass->getBlockCount();
                }

                --blockCount[ pass ];
                if ( blockCount[ pass ] == 0 ) {
                    frontier.push( passPtr );
                }
            } );
    }
}

using SortResult = std::pair<
    Array< SharedPointer< RenderPass > >,
    Array< SharedPointer< ComputePass > > >;

SortResult sort( SharedPointer< FrameGraphOperation > const &root ) noexcept
{
    auto ret = std::make_pair(
        Array< SharedPointer< RenderPass > > {},
        Array< SharedPointer< ComputePass > > {} );
    std::list< SharedPointer< FrameGraphOperation > > sorted;
    traverse(
        root,
        [ & ]( auto pass ) {
            if ( pass->getType() == FrameGraphOperation::Type::RENDER_PASS ) {
                ret.first.add( cast_ptr< RenderPass >( pass ) );
            } else if ( pass->getType() == FrameGraphOperation::Type::COMPUTE_PASS ) {
                // TODO:
                // ret.second.add( cast_ptr< ComputePass >( pass ) );
            }
            sorted.push_front( pass );
        } );

    FrameGraphOperation::Priority p = 0;
    for ( auto pass : sorted ) {
        pass->setPriority( p++ );
    }

    ret.first = ret.first.reversed();
    ret.second = ret.second.reversed();
    return ret;
}

void RenderSystem::onPreRender( void ) noexcept
{
    System::onPreRender();

    if ( m_frameGraph != nullptr && m_renderPasses.empty() ) {
        auto sorted = sort( m_frameGraph );

        m_renderPasses = sorted.first;
        m_computePasses = sorted.second;
    }
}

void RenderSystem::setFrameGraph( SharedPointer< FrameGraphOperation > const &frameGraph ) noexcept
{
    m_frameGraph = frameGraph;
    m_sortedOperations.clear();
}

RenderSystem::CommandBufferArray &RenderSystem::getGraphicsCommands( Size imageIndex, Bool includeConditionalPasses ) noexcept
{
    m_renderPasses.each( [ imageIndex ]( auto pass ) { pass->apply( imageIndex ); } );

    m_graphicsCommands = m_renderPasses.map(
        [ imageIndex ]( auto pass ) {
            return get_ptr( pass->getCommandBuffers()[ imageIndex ] );
        } );

    return m_graphicsCommands;
}

RenderSystem::CommandBufferArray &RenderSystem::getComputeCommands( Size imageIndex, Bool includeConditionalPasses ) noexcept
{
    // m_commandPasses.each( [ imageIndex ]( auto pass ) { pass->apply( inageIndex ); } );

    // TODO:
    // m_computeCommands = m_computePasses.map(
    //     [ imageIndex ]( auto pass ) {
    //         return get_ptr( pass->getCommandBuffers()[ imageIndex ] );
    //     } );

    return m_computeCommands;
}
