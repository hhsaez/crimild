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

#include "Rendering/CommandBuffer.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/FrameGraph.hpp"
#include "Rendering/IndexBuffer.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/VertexBuffer.hpp"
#include "Rendering/UniformBuffer.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( FrameGraph, simple )
{
    auto graph = crimild::alloc< FrameGraph >();

    auto color = [&] {
        auto attachment = graph->create< Attachment >();
        attachment->usage = Attachment::Usage::COLOR_ATTACHMENT;
        attachment->format = Format::R8G8B8A8_UNORM;
        return attachment;
    }();

    auto renderPass = graph->create< RenderPass >();
    renderPass->attachments = { color };

    auto present = graph->create< PresentationMaster >();
    present->colorAttachment = color;

    EXPECT_TRUE( graph->contains( color ) );
    EXPECT_TRUE( graph->contains( renderPass ) );
    EXPECT_TRUE( graph->contains( present ) );
}

TEST( FrameGraph, compile )
{
    auto graph = crimild::alloc< FrameGraph >();

    auto color = [&] {
        auto attachment = graph->create< Attachment >();
        attachment->usage = Attachment::Usage::COLOR_ATTACHMENT;
        attachment->format = Format::R8G8B8A8_UNORM;
        return attachment;
    }();

    auto renderPass = graph->create< RenderPass >();
    renderPass->attachments = { color };

    auto present = graph->create< PresentationMaster >();
    present->colorAttachment = color;

    EXPECT_TRUE( graph->compile() );

    auto expected = containers::Array< FrameGraph::Node * > {
        graph->getNode( renderPass ),
        graph->getNode( color ),
        graph->getNode( present ),
    };
    EXPECT_EQ( expected, graph->getSorted() );
}

TEST( FrameGraph, compileFailNoPresent )
{
    auto graph = crimild::alloc< FrameGraph >();

    auto color = [&] {
        auto attachment = graph->create< Attachment >();
        attachment->usage = Attachment::Usage::COLOR_ATTACHMENT;
        attachment->format = Format::R8G8B8A8_UNORM;
        return attachment;
    }();

    auto renderPass = graph->create< RenderPass >();
    renderPass->attachments = { color };

    EXPECT_TRUE( graph->contains( color ) );
    EXPECT_TRUE( graph->contains( renderPass ) );

    // Compile fails because there's no PresentPass node in the graph
    EXPECT_FALSE( graph->compile() );
}

TEST( FrameGraph, renderPassesNotConnected )
{
    auto graph = crimild::alloc< FrameGraph >();

    auto shadowDepth = [&] {
        auto attachment = graph->create< Attachment >();
        attachment->usage = Attachment::Usage::DEPTH_STENCIL_ATTACHMENT;
        attachment->format = Format::DEPTH_STENCIL_DEVICE_OPTIMAL;
        return attachment;
    }();

    auto shadowPass = [&] {
        auto pass = graph->create< RenderPass >();
        pass->attachments = { shadowDepth };
        return pass;
    }();

    auto color = [&] {
        auto attachment = graph->create< Attachment >();
        attachment->usage = Attachment::Usage::COLOR_ATTACHMENT;
        attachment->format = Format::R8G8B8A8_UNORM;
        return attachment;
    }();

    auto depth = [&] {
        auto attachment = graph->create< Attachment >();
        attachment->usage = Attachment::Usage::DEPTH_STENCIL_ATTACHMENT;
        attachment->format = Format::DEPTH_STENCIL_DEVICE_OPTIMAL;
        return attachment;
    }();

    auto resolve = [&] {
        auto attachment = graph->create< Attachment >();
        attachment->usage = Attachment::Usage::COLOR_ATTACHMENT;
        attachment->format = Format::COLOR_SWAPCHAIN_OPTIMAL;
        return attachment;
    }();

    auto renderPass = graph->create< RenderPass >();
    renderPass->attachments = { color, depth, resolve };

    auto present = graph->create< PresentationMaster >();
    present->colorAttachment = resolve;

    EXPECT_TRUE( graph->compile() );

	// Since renderPass is not really connected with shadowPass
	// most nodes are discarded by the frame graph
    auto &sorted = graph->getSorted();
    EXPECT_EQ( 3, sorted.size() );
    EXPECT_EQ( crimild::get_ptr( renderPass ), sorted[ 0 ]->obj );
    EXPECT_EQ( crimild::get_ptr( resolve ), sorted[ 1 ]->obj );
    EXPECT_EQ( crimild::get_ptr( present ), sorted[ 2 ]->obj );
}

TEST( FrameGraph, simpleFrameGraph )
{
	auto graph = crimild::alloc< FrameGraph >();

	auto pipeline = graph->create< Pipeline >();
	auto vbo = graph->create< VertexP2C3Buffer >( 0 );
	auto ibo = graph->create< IndexUInt32Buffer >( 0 );
	auto ubo = graph->create< UniformBufferImpl< crimild::Vector4f >>();
	auto descriptorSet = [&] {
		auto ds = graph->create< DescriptorSet >();
		ds->writes = {
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.buffer = crimild::get_ptr( ubo ),
			},
		};
		return ds;
	}();

	auto color = graph->create< Attachment >();

	auto renderPass = graph->create< RenderPass >();
	renderPass->attachments = { color };
	renderPass->commands = [&] {
		auto commands = graph->create< CommandBuffer >();
		commands->bindGraphicsPipeline( crimild::get_ptr( pipeline ) );
		commands->bindVertexBuffer( crimild::get_ptr( vbo ) );
		commands->bindIndexBuffer( crimild::get_ptr( ibo ) );
		commands->bindDescriptorSet( crimild::get_ptr( descriptorSet ) );
		commands->drawIndexed( ibo->getCount() );
		return commands;
	}();

	auto present = graph->create< PresentationMaster >();
	present->colorAttachment = { color };

	EXPECT_TRUE( graph->compile() );

	auto sorted = graph->getSorted();
	EXPECT_EQ( 9, sorted.size() );

	// The first nodes contains basic resources in any order
	auto resources = containers::Array< FrameGraphObject * > {
		crimild::get_ptr( pipeline ),
		crimild::get_ptr( vbo ),
		crimild::get_ptr( ibo ),
		crimild::get_ptr( ubo ),
	};
	EXPECT_TRUE( resources.contains( sorted[ 0 ]->obj ) );
	EXPECT_TRUE( resources.contains( sorted[ 1 ]->obj ) );
	EXPECT_TRUE( resources.contains( sorted[ 2 ]->obj ) );
	EXPECT_TRUE( resources.contains( sorted[ 3 ]->obj ) );

	EXPECT_EQ( crimild::get_ptr( descriptorSet ), sorted[ 4 ]->obj );
	EXPECT_EQ( crimild::get_ptr( renderPass->commands ), sorted[ 5 ]->obj );
	EXPECT_EQ( crimild::get_ptr( renderPass ), sorted[ 6 ]->obj );
	EXPECT_EQ( crimild::get_ptr( color ), sorted[ 7 ]->obj );
	EXPECT_EQ( crimild::get_ptr( present ), sorted[ 8 ]->obj );
}

TEST( FrameGraph, simpleAutoAddNodes )
{
	auto graph = crimild::alloc< FrameGraph >();

	auto pipeline = crimild::alloc< Pipeline >();
	auto vbo = crimild::alloc< VertexP2C3Buffer >( 0 );
	auto ibo = crimild::alloc< IndexUInt32Buffer >( 0 );
	auto ubo = crimild::alloc< UniformBufferImpl< crimild::Vector4f >>();
	auto descriptorSet = [&] {
		auto ds = crimild::alloc< DescriptorSet >();
		ds->writes = {
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.buffer = crimild::get_ptr( ubo ),
			},
		};
		return ds;
	}();

	auto color = graph->create< Attachment >();

	auto renderPass = graph->create< RenderPass >();
	renderPass->attachments = { color };
	renderPass->commands = [&] {
		auto commands = crimild::alloc< CommandBuffer >();
		commands->bindGraphicsPipeline( crimild::get_ptr( pipeline ) );
		commands->bindVertexBuffer( crimild::get_ptr( vbo ) );
		commands->bindIndexBuffer( crimild::get_ptr( ibo ) );
		commands->bindDescriptorSet( crimild::get_ptr( descriptorSet ) );
		commands->drawIndexed( ibo->getCount() );
		return commands;
	}();

	auto present = graph->create< PresentationMaster >();
	present->colorAttachment = { color };

	EXPECT_TRUE( graph->compile() );

	auto sorted = graph->getSorted();
	EXPECT_EQ( 9, sorted.size() );

	// The first nodes contains basic resources in any order
	auto resources = containers::Array< FrameGraphObject * > {
		crimild::get_ptr( pipeline ),
		crimild::get_ptr( vbo ),
		crimild::get_ptr( ibo ),
		crimild::get_ptr( ubo ),
	};
	EXPECT_TRUE( resources.contains( sorted[ 0 ]->obj ) );
	EXPECT_TRUE( resources.contains( sorted[ 1 ]->obj ) );
	EXPECT_TRUE( resources.contains( sorted[ 2 ]->obj ) );
	EXPECT_TRUE( resources.contains( sorted[ 3 ]->obj ) );

	EXPECT_EQ( crimild::get_ptr( descriptorSet ), sorted[ 4 ]->obj );
	EXPECT_EQ( crimild::get_ptr( renderPass->commands ), sorted[ 5 ]->obj );
	EXPECT_EQ( crimild::get_ptr( renderPass ), sorted[ 6 ]->obj );
	EXPECT_EQ( crimild::get_ptr( color ), sorted[ 7 ]->obj );
	EXPECT_EQ( crimild::get_ptr( present ), sorted[ 8 ]->obj );
}

TEST( FrameGraph, imageUsage )
{
	auto graph = crimild::alloc< FrameGraph >();

	auto color = graph->create< Attachment >();
	color->usage = Attachment::Usage::COLOR_ATTACHMENT;
	color->format = Format::COLOR_SWAPCHAIN_OPTIMAL;
	color->imageView = [&] {
		auto imageView = crimild::alloc< ImageView >();
		imageView->image = [&] {
			auto image = crimild::alloc< Image >();
			return image;
		}();
		return imageView;
	}();

	auto depth = graph->create< Attachment >();
	depth->usage = Attachment::Usage::DEPTH_STENCIL_ATTACHMENT;
	depth->format = Format::DEPTH_STENCIL_DEVICE_OPTIMAL;
	depth->imageView = [&] {
		auto imageView = crimild::alloc< ImageView >();
		imageView->image = [&] {
			auto image = crimild::alloc< Image >();
			return image;
		}();
		return imageView;
	}();

	auto renderPass = graph->create< RenderPass >();
	renderPass->attachments = { color, depth };

	auto master = graph->create< PresentationMaster >();
	master->colorAttachment = color;

	EXPECT_TRUE( graph->compile() );

	{
		auto res = graph->connected< Attachment >( color->imageView->image );
		EXPECT_EQ( 1, res.size() );
		EXPECT_EQ( Attachment::Usage::COLOR_ATTACHMENT, res.first()->usage );
	}

	{
		auto res = graph->connected< Attachment >( depth->imageView->image );
		EXPECT_EQ( 1, res.size() );
		EXPECT_EQ( Attachment::Usage::DEPTH_STENCIL_ATTACHMENT, res.first()->usage );
	}
}

TEST( FrameGraph, offscreen )
{
	auto graph = crimild::alloc< FrameGraph >();

	auto texture = [&] {
		auto texture = graph->create< Texture >();
		texture->imageView = [&] {
			auto imageView = graph->create< ImageView >();
			imageView->image = crimild::alloc< Image >();
			return imageView;
		}();
		return texture;
	}();

	auto offPipeline = crimild::alloc< Pipeline >();
	auto offVbo = crimild::alloc< VertexP2C3Buffer >( 0 );
	auto offIbo = crimild::alloc< IndexUInt32Buffer >( 0 );
	auto offUbo = crimild::alloc< UniformBufferImpl< crimild::Vector4f >>();
	auto offDescriptorSet = [&] {
		auto ds = crimild::alloc< DescriptorSet >();
		ds->writes = {
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.buffer = crimild::get_ptr( offUbo ),
			},
		};
		return ds;
	}();
	
	auto offColor = graph->create< Attachment >();
	offColor->imageView = texture->imageView;
	
	auto offRenderPass = [&] {
		auto renderPass = graph->create< RenderPass >();
		renderPass->attachments = { offColor };
		renderPass->commands = [&] {
			auto commands = crimild::alloc< CommandBuffer >();
			commands->bindGraphicsPipeline( crimild::get_ptr( offPipeline ) );
			commands->bindVertexBuffer( crimild::get_ptr( offVbo ) );
			commands->bindIndexBuffer( crimild::get_ptr( offIbo ) );
			commands->bindDescriptorSet( crimild::get_ptr( offDescriptorSet ) );
			commands->drawIndexed( offIbo->getCount() );
			return commands;
		}();
		return renderPass;
	}();
	
    // Screen

	auto pipeline = crimild::alloc< Pipeline >();
	auto vbo = crimild::alloc< VertexP2C3Buffer >( 0 );
	auto ibo = crimild::alloc< IndexUInt32Buffer >( 0 );
	auto ubo = crimild::alloc< UniformBufferImpl< crimild::Vector4f >>();
	auto descriptorSet = [&] {
		auto ds = crimild::alloc< DescriptorSet >();
		ds->writes = {
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.buffer = crimild::get_ptr( ubo ),
			},
			{
				.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
				.texture = crimild::get_ptr( texture ),
			},
		};
		return ds;
	}();
	
	auto color = graph->create< Attachment >();
	
	auto renderPass = graph->create< RenderPass >();
	renderPass->attachments = { color };
	renderPass->commands = [&] {
		auto commands = crimild::alloc< CommandBuffer >();
		commands->bindGraphicsPipeline( crimild::get_ptr( pipeline ) );
		commands->bindVertexBuffer( crimild::get_ptr( vbo ) );
		commands->bindIndexBuffer( crimild::get_ptr( ibo ) );
		commands->bindDescriptorSet( crimild::get_ptr( descriptorSet ) );
		commands->drawIndexed( ibo->getCount() );
		return commands;
	}();
	
	auto master = graph->create< PresentationMaster >();
	master->colorAttachment = color;

	EXPECT_TRUE( graph->compile() );

	graph->getSorted().each(
		[]( auto &node ) {
			std::cout << node->type << std::endl;
		}
	);
}

