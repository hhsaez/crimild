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

#include "Rendering/RenderPass.hpp"
#include "Rendering/FrameGraph.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( FrameGraph, simple )
{
    auto graph = crimild::alloc< FrameGraph >();

    auto color = [&] {
        auto attachment = graph->create< Attachment >();
        attachment->usage = Image::Usage::COLOR_ATTACHMENT;
        attachment->format = Format::R8G8B8A8_UNORM;
        return attachment;
    }();

    auto subpass = [&] {
        auto subpass = graph->create< RenderSubpass >();
        subpass->outputs = { color };
        return subpass;
    }();

    auto renderPass = graph->create< RenderPass >();
    renderPass->attachments = { color };
    renderPass->subpasses = { subpass };

    auto present = graph->create< PresentPass >();
    present->colorAttachment = color;

    EXPECT_TRUE( graph->contains( color ) );
    EXPECT_TRUE( graph->contains( subpass ) );
    EXPECT_TRUE( graph->contains( renderPass ) );
    EXPECT_TRUE( graph->contains( present ) );
}

TEST( FrameGraph, compile )
{
    auto graph = crimild::alloc< FrameGraph >();

    auto color = [&] {
        auto attachment = graph->create< Attachment >();
        attachment->usage = Image::Usage::COLOR_ATTACHMENT;
        attachment->format = Format::R8G8B8A8_UNORM;
        return attachment;
    }();

    auto subpass = [&] {
        auto subpass = graph->create< RenderSubpass >();
        subpass->outputs = { color };
        return subpass;
    }();

    auto renderPass = graph->create< RenderPass >();
    renderPass->attachments = { color };
    renderPass->subpasses = { subpass };

    auto present = graph->create< PresentPass >();
    present->colorAttachment = color;

    EXPECT_TRUE( graph->compile() );

    auto expected = containers::Array< FrameGraph::Node * > {
        graph->getNode( subpass ),
        graph->getNode( color ),
        graph->getNode( renderPass ),
        graph->getNode( present ),
    };
    EXPECT_EQ( expected, graph->getSorted() );
}

TEST( FrameGraph, compileFailNoPresent )
{
    auto graph = crimild::alloc< FrameGraph >();

    auto color = [&] {
        auto attachment = graph->create< Attachment >();
        attachment->usage = Image::Usage::COLOR_ATTACHMENT;
        attachment->format = Format::R8G8B8A8_UNORM;
        return attachment;
    }();

    auto subpass = [&] {
        auto subpass = graph->create< RenderSubpass >();
        subpass->outputs = { color };
        return subpass;
    }();

    auto renderPass = graph->create< RenderPass >();
    renderPass->attachments = { color };
    renderPass->subpasses = { subpass };

    EXPECT_TRUE( graph->contains( color ) );
    EXPECT_TRUE( graph->contains( subpass ) );
    EXPECT_TRUE( graph->contains( renderPass ) );

    // Compile fails because there's no PresentPass node in the graph
    EXPECT_FALSE( graph->compile() );
}

TEST( FrameGraph, inputAttachments )
{
    auto graph = crimild::alloc< FrameGraph >();

    auto color = [&] {
        auto attachment = graph->create< Attachment >();
        attachment->usage = Image::Usage::COLOR_ATTACHMENT;
        attachment->format = Format::R8G8B8A8_UNORM;
        return attachment;
    }();

    auto depth = [&] {
        auto attachment = graph->create< Attachment >();
        attachment->usage = Image::Usage::DEPTH_STENCIL_ATTACHMENT;
        attachment->format = Format::DEPTH_STENCIL_DEVICE_OPTIMAL;
        return attachment;
    }();

    auto resolve = [&] {
        auto attachment = graph->create< Attachment >();
        attachment->usage = Image::Usage::COLOR_ATTACHMENT;
        attachment->format = Format::COLOR_SWAPCHAIN_OPTIMAL;
        return attachment;
    }();

    auto gBufferSubpass = [&] {
        auto subpass = graph->create< RenderSubpass >();
        subpass->outputs = { color, depth };
        return subpass;
    }();

    auto resolveSubpass = [&] {
        auto subpass = graph->create< RenderSubpass >();
        subpass->inputs = { color, depth };
        subpass->outputs = { resolve };
        return subpass;
    }();

    auto renderPass = graph->create< RenderPass >();
    renderPass->attachments = { color, depth, resolve };
    renderPass->subpasses = { gBufferSubpass, resolveSubpass };

    auto present = graph->create< PresentPass >();
    present->colorAttachment = resolve;

    EXPECT_TRUE( graph->compile() );

    // While attachments can be in different order between subpasses,
    // all render passes, subpasses the and present pass must
    // be in a specific order.
    auto &sorted = graph->getSorted();
    EXPECT_EQ( 7, sorted.size() );
    EXPECT_EQ( gBufferSubpass, sorted[ 0 ]->obj );
    EXPECT_EQ( resolveSubpass, sorted[ 3 ]->obj );
    EXPECT_EQ( renderPass, sorted[ 5 ]->obj );
    EXPECT_EQ( present, sorted[ 6 ]->obj );
}

TEST( FrameGraph, renderPasses )
{
    auto graph = crimild::alloc< FrameGraph >();

    auto shadowDepth = [&] {
        auto attachment = graph->create< Attachment >();
        attachment->usage = Image::Usage::DEPTH_STENCIL_ATTACHMENT;
        attachment->format = Format::DEPTH_STENCIL_DEVICE_OPTIMAL;
        return attachment;
    }();

    auto shadowSubpass = [&] {
        auto subpass = graph->create< RenderSubpass >();
        subpass->outputs = { shadowDepth };
        return subpass;
    }();

    auto shadowPass = [&] {
        auto pass = graph->create< RenderPass >();
        pass->attachments = { shadowDepth };
        pass->subpasses = { shadowSubpass };
        return pass;
    }();

    auto color = [&] {
        auto attachment = graph->create< Attachment >();
        attachment->usage = Image::Usage::COLOR_ATTACHMENT;
        attachment->format = Format::R8G8B8A8_UNORM;
        return attachment;
    }();

    auto depth = [&] {
        auto attachment = graph->create< Attachment >();
        attachment->usage = Image::Usage::DEPTH_STENCIL_ATTACHMENT;
        attachment->format = Format::DEPTH_STENCIL_DEVICE_OPTIMAL;
        return attachment;
    }();

    auto resolve = [&] {
        auto attachment = graph->create< Attachment >();
        attachment->usage = Image::Usage::COLOR_ATTACHMENT;
        attachment->format = Format::COLOR_SWAPCHAIN_OPTIMAL;
        return attachment;
    }();

    auto gBufferSubpass = [&] {
        auto subpass = graph->create< RenderSubpass >();
        subpass->outputs = { color, depth };
        return subpass;
    }();

    auto resolveSubpass = [&] {
        auto subpass = graph->create< RenderSubpass >();
        subpass->inputs = { color, depth };
        subpass->outputs = { resolve };
        return subpass;
    }();

    auto renderPass = graph->create< RenderPass >();
    renderPass->attachments = { color, depth, resolve };
    renderPass->subpasses = { gBufferSubpass, resolveSubpass };

    auto present = graph->create< PresentPass >();
    present->colorAttachment = resolve;

    EXPECT_TRUE( graph->compile() );

    // While attachments can be in different order between subpasses,
    // all render passes, subpasses the and present pass must
    // be in a specific order.
    auto &sorted = graph->getSorted();
    EXPECT_EQ( 7, sorted.size() );
    EXPECT_EQ( gBufferSubpass, sorted[ 0 ]->obj );
    EXPECT_EQ( resolveSubpass, sorted[ 3 ]->obj );
    EXPECT_EQ( renderPass, sorted[ 5 ]->obj );
    EXPECT_EQ( present, sorted[ 6 ]->obj );
}

