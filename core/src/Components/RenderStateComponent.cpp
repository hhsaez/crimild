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

#include "RenderStateComponent.hpp"
#include "Rendering/Buffer.hpp"
#include "Rendering/CommandBuffer.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/VertexBuffer.hpp"

CRIMILD_REGISTER_STREAM_OBJECT_BUILDER( crimild::RenderStateComponent )

using namespace crimild;

RenderStateComponent::RenderStateComponent( void )
    : _renderOnScreen( false )
{
    commandRecorder = [ this ]( CommandBuffer *commandBuffer ) {
        prepare();

        commandBuffer->bindGraphicsPipeline(
            crimild::get_ptr( pipeline )
        );
        commandBuffer->bindVertexBuffer(
            crimild::get_ptr( vbo )
        );
        commandBuffer->bindIndexBuffer(
            crimild::get_ptr( ibo )
        );
        commandBuffer->bindDescriptorSet(
            crimild::get_ptr( descriptorSet )
        );
        crimild::UInt32 indexCount = ibo->getSize() /  ibo->getStride();
        commandBuffer->drawIndexed(
            indexCount
        );
    };
}

RenderStateComponent::~RenderStateComponent( void )
{
	detachAllMaterials();
	detachAllLights();
}

void RenderStateComponent::prepare( void ) noexcept
{
    if ( pipeline->descriptorSetLayout == nullptr ) {
        pipeline->descriptorSetLayout = pipeline->program->descriptorSetLayout;
    }

    if ( pipeline->attributeDescriptions.empty() ) {
    	pipeline->attributeDescriptions = pipeline->program->attributeDescriptions;
        pipeline->bindingDescription = pipeline->program->bindingDescription;
    }

    prepare( crimild::get_ptr( pipeline->descriptorSetLayout ) );
}

void RenderStateComponent::prepare( DescriptorSetLayout *layout ) noexcept
{
    if ( descriptorSetLayout == nullptr ) {
        descriptorSetLayout = crimild::retain( layout );
    }

    if ( descriptorPool == nullptr ) {
        descriptorPool = crimild::alloc< DescriptorPool >();
        descriptorPool->descriptorSetLayout = descriptorSetLayout;
    }

    if ( descriptorSet == nullptr ) {
        descriptorSet = crimild::alloc< DescriptorSet >();
        descriptorSet->descriptorSetLayout = descriptorSetLayout;
        descriptorSet->descriptorPool = descriptorPool;

        size_t uniformIdx = 0;
        size_t textureIdx = 0;

        descriptorSetLayout->bindings.each( [&]( DescriptorSetLayout::Binding &binding ) {
            auto write = DescriptorSet::Write {
                .descriptorType = binding.descriptorType,
            };

            switch ( write.descriptorType ) {
                case DescriptorType::UNIFORM_BUFFER:
                    write.buffer = crimild::get_ptr( uniforms[ uniformIdx++ ] );
                    break;
                case DescriptorType::COMBINED_IMAGE_SAMPLER:
                    write.texture = crimild::get_ptr( textures[ textureIdx++ ] );
                    break;
                default:
                    CRIMILD_LOG_FATAL( "Invalid descriptor type" );
                    exit( -1 );
                    break;
            };

            descriptorSet->writes.add( write );
        });
    }
}

void RenderStateComponent::forEachMaterial( std::function< void( Material * ) > callback )
{
	_materials.each( [ callback ]( SharedPointer< Material > &m ) {
		callback( crimild::get_ptr( m ) );
	});
}

void RenderStateComponent::forEachLight( std::function< void( Light * ) > callback )
{
	_lights.each( [ callback ]( SharedPointer< Light > &l ) {
		callback( crimild::get_ptr( l ) );
	});
}

