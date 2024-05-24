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

#ifndef CRIMILD_CORE_RENDERING_RENDER_PASS_
#define CRIMILD_CORE_RENDERING_RENDER_PASS_

#include "Crimild_Foundation.hpp"
#include "Crimild_Mathematics.hpp"
#include "Rendering/CommandBuffer.hpp"
#include "Rendering/Format.hpp"
#include "Rendering/FrameGraphOperation.hpp"
#include "Rendering/FrameGraphResource.hpp"
#include "Rendering/Image.hpp"
#include "Rendering/ImageView.hpp"
#include "Rendering/RenderResource.hpp"
#include "Rendering/Swapchain.hpp"
#include "Rendering/ViewportDimensions.hpp"

namespace crimild {

    class DescriptorSet;
    class GraphicsPipeline;

    class [[deprecated]] Attachment
        : public SharedObject,
          public NamedObject,
          public FrameGraphResource {
    public:
        enum class Usage {
            UNDEFINED,
            COLOR_ATTACHMENT,
            DEPTH_STENCIL_ATTACHMENT,
        };

        enum class LoadOp {
            LOAD,
            CLEAR,
            DONT_CARE,
        };

        enum class StoreOp {
            STORE,
            DONT_CARE,
        };

    public:
        inline FrameGraphResource::Type getType( void ) const noexcept override { return FrameGraphResource::Type::ATTACHMENT; }

        Format format = Format::UNDEFINED;
        Usage usage = Usage::UNDEFINED;
        LoadOp loadOp = LoadOp::DONT_CARE;
        StoreOp storeOp = StoreOp::DONT_CARE;
        LoadOp stencilLoadOp = LoadOp::DONT_CARE;
        StoreOp stencilStoreOp = StoreOp::DONT_CARE;

        SharedPointer< ImageView > imageView = [] {
            // Create an empty image that might be used to store the attachment's data
            // Note: If the attachment is for presentation (see below), this image is not
            // used. Instead, we use the one from the Swapchain.
            auto imageView = crimild::alloc< ImageView >();
            imageView->image = crimild::alloc< Image >();
            return imageView;
        }();

        inline Bool isPresentation( void ) const noexcept
        {
            // This attachment is for presentation if and only if the format
            // is the expected one and no operation reads its data. This usually
            // happens only in the case of the root operation.
            return !isRead() && format == Format::COLOR_SWAPCHAIN_OPTIMAL;
        }

        virtual void setWrittenBy( FrameGraphOperation *op ) noexcept override
        {
            FrameGraphResource::setWrittenBy( op );
            if ( imageView != nullptr ) {
                imageView->setWrittenBy( op );
            }
        }

        virtual void setReadBy( FrameGraphOperation *op ) noexcept override
        {
            FrameGraphResource::setReadBy( op );
            if ( imageView != nullptr ) {
                imageView->setReadBy( op );
            }
        }
    };

    class [[deprecated]] RenderPass
        : public RenderResourceImpl< RenderPass >,
          public FrameGraphOperation {

    public:
        virtual ~RenderPass( void ) = default;

        inline FrameGraphOperation::Type getType( void ) const noexcept override { return FrameGraphOperation::Type::RENDER_PASS; }

        Array< SharedPointer< Attachment > > attachments;
        Extent2D extent = Extent2D {
            .scalingMode = ScalingMode::SWAPCHAIN_RELATIVE,
        };

        struct ClearValue {
            ColorRGBA color = ColorRGBA::Constants::CLEAR;
            Vector2f depthStencil = Vector2f::Constants::UNIT_X;
        };
        ClearValue clearValue;

        inline void setCommandBuffers( Array< SharedPointer< CommandBuffer > > const &commands ) noexcept { m_commands = commands; }
        inline Array< SharedPointer< CommandBuffer > > &getCommandBuffers( void ) noexcept { return m_commands; }

    private:
        Array< SharedPointer< CommandBuffer > > m_commands;

    public:
        inline void setCommandRecorder( CommandRecorder commandRecorder ) noexcept { m_commandRecorder = commandRecorder; }

        template< typename BuilderFunction >
        void createCommandRecorder( BuilderFunction builder ) noexcept
        {
            auto swapchain = Swapchain::getInstance();
            if ( swapchain == nullptr ) {
                CRIMILD_LOG_ERROR( "Failed to obtain swapchain" );
                return;
            }

            // This is a helper function to create a basic command recorder
            // for static rendering commands.
            auto commandBuffers = swapchain->getImages().map(
                [ &, imageIndex = 0 ]( auto unused ) mutable {
                    auto commandBuffer = builder();
                    commandBuffer->setFrameIndex( imageIndex++ );
                    return commandBuffer;
                }
            );

            setCommandRecorder(
                [ commandBuffers ]( Size imageIndex ) {
                    return commandBuffers[ imageIndex ];
                }
            );
        }

        inline CommandBuffer *execute( Size imageIndex ) noexcept
        {
            return m_commandRecorder != nullptr ? m_commandRecorder( imageIndex ) : nullptr;
        }

    private:
        CommandRecorder m_commandRecorder;

        /**
           \name Pipeline
        */
        //@{

    public:
        void setGraphicsPipeline( GraphicsPipeline *graphicsPipeline ) noexcept;
        inline void setGraphicsPipeline( SharedPointer< GraphicsPipeline > const &graphicsPipeline ) noexcept { m_graphicsPipeline = graphicsPipeline; }
        inline GraphicsPipeline *getGraphicsPipeline( void ) noexcept { return get_ptr( m_graphicsPipeline ); }

    private:
        /**
           \brief GraphicsPipeline associated with this render pass, if any.

           This is optional.
         */
        SharedPointer< GraphicsPipeline > m_graphicsPipeline;

        //@}

        /**
           \name Descriptor Set
        */
        //@{

    public:
        inline void setDescriptors( DescriptorSet *descriptors ) noexcept;
        inline void setDescriptors( SharedPointer< DescriptorSet > const &descriptors ) noexcept { m_descriptors = descriptors; }
        inline DescriptorSet *getDescriptors( void ) noexcept { return get_ptr( m_descriptors ); }

    private:
        /**
           \brief Descriptors associated with this render pass, if any.

           This is optional.
         */
        SharedPointer< DescriptorSet > m_descriptors;

        //@}

    public:
        void setConditional( Bool conditional ) noexcept { m_conditional = conditional; }
        Bool isConditional( void ) const noexcept { return m_conditional; }

    private:
        Bool m_conditional = false;
    };

}

#endif
