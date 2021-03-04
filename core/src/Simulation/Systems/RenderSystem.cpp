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
#include "Rendering/Operations/OperationUtils.hpp"
#include "Rendering/Operations/Operations.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/ScenePass.hpp"
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

            auto renderables = fetchRenderables();
            auto litRenderables = renderables->getProduct( 0 );
            auto unlitRenderables = renderables->getProduct( 1 );
            auto envRenderables = renderables->getProduct( 2 );

            auto gBuffer = gBufferPass( litRenderables );
            auto albedo = gBuffer->getProduct( 0 );
            auto positions = gBuffer->getProduct( 1 );
            auto normals = gBuffer->getProduct( 2 );
            auto materials = gBuffer->getProduct( 3 );
            auto depth = gBuffer->getProduct( 4 );

            // TODO
            auto shadowAtlas = Image::ONE;
            auto reflectionAtlas = Image::ZERO;
            auto irradianceAtlas = Image::ZERO;
            auto prefilterAtlas = Image::ZERO;
            auto brdfLUT = Image::ZERO;

            auto lit = lightingPass(
                albedo,
                positions,
                normals,
                materials,
                depth,
                shadowAtlas,
                reflectionAtlas,
                irradianceAtlas,
                prefilterAtlas,
                brdfLUT );

            auto unlit = forwardUnlitPass( unlitRenderables, nullptr, depth );
            auto env = forwardUnlitPass( envRenderables, nullptr, depth );

            return blend(
                {
                    useResource( lit ),
                    useResource( unlit ),
                    useResource( env ),
                } );
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
                    // TODO: if passes are discarded because they don't really add up to the final image,
                    // this number will not be correct, since it will show more blocks than actually are.
                    blockCount[ pass ] = pass->getBlockCount();
                }

                --blockCount[ pass ];
                if ( blockCount[ pass ] == 0 ) {
                    frontier.push( passPtr );
                }
            } );
    }
}

void RenderSystem::sort( SharedPointer< FrameGraphOperation > const &root ) noexcept
{
    m_scenePasses.clear();
    m_renderPasses.clear();
    m_computePasses.clear();

    std::list< SharedPointer< FrameGraphOperation > > sorted;

    traverse(
        root,
        [ & ]( auto pass ) {
            if ( pass->getType() == FrameGraphOperation::Type::SCENE_PASS ) {
                m_scenePasses.add( cast_ptr< ScenePass >( pass ) );
            } else if ( pass->getType() == FrameGraphOperation::Type::RENDER_PASS ) {
                m_renderPasses.add( cast_ptr< RenderPass >( pass ) );
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

    m_scenePasses = m_scenePasses.reversed();
    m_renderPasses = m_renderPasses.reversed();
    m_computePasses = m_computePasses.reversed();
}

void RenderSystem::onPreRender( void ) noexcept
{
    System::onPreRender();

    if ( m_frameGraph != nullptr && m_renderPasses.empty() ) {
        sort( m_frameGraph );
    }

    m_scenePasses.each(
        []( auto pass ) {
            // Scene passes are executed every frame with no image index
            pass->apply( 0 );
        } );
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
