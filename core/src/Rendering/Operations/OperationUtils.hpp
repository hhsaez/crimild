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
 *     * Neither the name of the copyright holder nor the
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

#ifndef CRIMILD_CORE_RENDERING_OPERATIONS_UTILS_
#define CRIMILD_CORE_RENDERING_OPERATIONS_UTILS_

#include "Rendering/CommandBuffer.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Swapchain.hpp"

namespace crimild {

    namespace framegraph {

        SharedPointer< Attachment > useColorAttachment( std::string name = "", Format format = Format::COLOR_SWAPCHAIN_OPTIMAL ) noexcept;
        SharedPointer< Attachment > useDepthAttachment( std::string name = "" ) noexcept;

        SharedPointer< FrameGraphResource > useResource( SharedPointer< FrameGraphOperation > const &op, Size index = 0 ) noexcept;
        SharedPointer< Texture > withResource( SharedPointer< Texture > const &texture, SharedPointer< FrameGraphResource > const &resource ) noexcept;

        template< typename CommandRecorder >
        static SharedPointer< RenderPass > withGraphicsCommands( SharedPointer< RenderPass > const &renderPass, CommandRecorder recorder ) noexcept
        {
            renderPass->getCommandBuffers()
                .resize( Swapchain::getInstance()->getImages().size() )
                .fill(
                    [ recorder,
                      renderPass = crimild::get_ptr( renderPass ) ]( auto frameIndex ) {
                        auto commandBuffer = crimild::alloc< CommandBuffer >();
                        commandBuffer->setFrameIndex( frameIndex );
                        commandBuffer->begin( CommandBuffer::Usage::SIMULTANEOUS_USE );
                        commandBuffer->beginRenderPass( crimild::get_ptr( renderPass ), nullptr );

                        recorder( commandBuffer );

                        commandBuffer->endRenderPass( crimild::get_ptr( renderPass ) );
                        commandBuffer->end();
                        return commandBuffer;
                    } );

            return renderPass;
        }

        template< typename CommandRecorder >
        static SharedPointer< RenderPass > withDynamicGraphicsCommands( SharedPointer< RenderPass > const &renderPass, CommandRecorder recorder ) noexcept
        {
            renderPass->getCommandBuffers()
                .resize( Swapchain::getInstance()->getImages().size() )
                .fill(
                    [ baseName = renderPass->getName() ]( auto frameIndex ) {
                        std::stringstream ss;
                        ss << baseName;
                        ss << ( baseName != "" ? "/commandBuffer" : "commandBuffer" );
                        ss << "/" << frameIndex;
                        auto commandBuffer = crimild::alloc< CommandBuffer >();
                        commandBuffer->setName( ss.str() );
                        commandBuffer->setFrameIndex( frameIndex );
                        return commandBuffer;
                    } );

            renderPass->apply =
                [ recorder,
                  renderPass = crimild::get_ptr( renderPass ) ]( auto imageIndex, auto unused ) {
                    auto commandBuffer = renderPass->getCommandBuffers()[ imageIndex ];

                    commandBuffer->clear();

                    commandBuffer->begin( CommandBuffer::Usage::SIMULTANEOUS_USE );
                    commandBuffer->beginRenderPass( crimild::get_ptr( renderPass ), nullptr );

                    recorder( commandBuffer );

                    commandBuffer->endRenderPass( crimild::get_ptr( renderPass ) );
                    commandBuffer->end();
                    return true;
                };

            return renderPass;
        }

        template< typename CommandRecorder, typename Predicate >
        static SharedPointer< RenderPass > withConditionalGraphicsCommands( SharedPointer< RenderPass > const &renderPass, Predicate predicate, CommandRecorder recorder ) noexcept
        {
            renderPass->getCommandBuffers()
                .resize( Swapchain::getInstance()->getImages().size() )
                .fill(
                    [ baseName = renderPass->getName() ]( auto frameIndex ) {
                        std::stringstream ss;
                        ss << baseName;
                        ss << ( baseName != "" ? "/commandBuffer" : "commandBuffer" );
                        ss << "/" << frameIndex;
                        auto commandBuffer = crimild::alloc< CommandBuffer >();
                        commandBuffer->setName( ss.str() );
                        commandBuffer->setFrameIndex( frameIndex );
                        return commandBuffer;
                    } );

            renderPass->apply =
                [ recorder,
                  predicate,
                  renderPass = crimild::get_ptr( renderPass ) ]( auto imageIndex, auto force ) {
                    if ( !force && !predicate() ) {
                        return false;
                    }

                    auto commandBuffer = renderPass->getCommandBuffers()[ imageIndex ];

                    commandBuffer->clear();

                    commandBuffer->begin( CommandBuffer::Usage::SIMULTANEOUS_USE );
                    commandBuffer->beginRenderPass( crimild::get_ptr( renderPass ), nullptr );

                    recorder( commandBuffer );

                    commandBuffer->endRenderPass( crimild::get_ptr( renderPass ) );
                    commandBuffer->end();
                    return true;
                };

            return renderPass;
        }

    }
}

#endif
