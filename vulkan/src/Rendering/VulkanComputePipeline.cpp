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

#include "VulkanComputePipeline.hpp"

#include "Foundation/Log.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "VulkanPhysicalDevice.hpp"
#include "VulkanRenderDeviceOLD.hpp"

using namespace crimild;
using namespace crimild::vulkan;

crimild::Bool ComputePipelineManager::bind( ComputePipeline *pipeline ) noexcept
{
    if ( validate( pipeline ) ) {
        // Pipeline already bound
        return true;
    }

    CRIMILD_LOG_TRACE();

    auto renderDevice = getRenderDevice();
    if ( renderDevice == nullptr ) {
        return false;
    }

    // WARNING: all of these config params are used when creating the pipeline and
    // they must be alive when vkCreatePipeline is called. Beware of scopes!
    auto shaderModules = createShaderModules( renderDevice, pipeline->getProgram() );
    auto shaderStages = createShaderStages( shaderModules );

    auto pipelineLayout = renderDevice->create(
        PipelineLayout::Descriptor {
            .setLayouts = pipeline->getProgram()->descriptorSetLayouts.map(
                []( auto &layout ) {
                    return crimild::get_ptr( layout );
                } ),
        } );

    auto createInfo = VkComputePipelineCreateInfo {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = shaderStages[ 0 ],
        .layout = pipelineLayout->handler,
    };

    VkPipeline pipelineHander;
    CRIMILD_VULKAN_CHECK(
        vkCreateComputePipelines(
            renderDevice->handler,
            VK_NULL_HANDLE,
            1,
            &createInfo,
            nullptr,
            &pipelineHander ) );

    setBindInfo(
        pipeline,
        {
            .pipelineHandler = pipelineHander,
            .pipelineLayout = pipelineLayout,
        } );

    utils::setObjectName(
        renderDevice->handler,
        UInt64( pipelineHander ),
        VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT,
        pipeline->getName().c_str() );

    return ManagerImpl::bind( pipeline );
}

crimild::Bool ComputePipelineManager::unbind( ComputePipeline *pipeline ) noexcept
{
    if ( !validate( pipeline ) ) {
        return false;
    }

    CRIMILD_LOG_TRACE();

    auto renderDevice = getRenderDevice();
    if ( renderDevice == nullptr ) {
        CRIMILD_LOG_ERROR( "No valid render device instance" );
        return false;
    }

    auto bindInfo = getBindInfo( pipeline );

    vkDestroyPipeline(
        renderDevice->handler,
        bindInfo.pipelineHandler,
        nullptr );

    removeBindInfo( pipeline );

    return ManagerImpl::unbind( pipeline );
}

ComputePipelineManager::ShaderModuleArray ComputePipelineManager::createShaderModules( RenderDeviceOLD *renderDevice, ShaderProgram *program ) const noexcept
{
    CRIMILD_LOG_TRACE();

    ShaderModuleArray modules;
    assert( program != nullptr && "Invalid shader program instance" );
    assert( !program->getShaders().empty() && "Invalid shader program" );
    program->getShaders().each( [ &modules, renderDevice ]( SharedPointer< Shader > &shader ) {
        auto module = renderDevice->create(
            ShaderModule::Descriptor {
                .shader = crimild::get_ptr( shader ) } );
        if ( module != nullptr ) {
            modules.push_back( module );
        }
    } );

    return modules;
}

ComputePipelineManager::ShaderStageArray ComputePipelineManager::createShaderStages( const ShaderModuleArray &modules ) const noexcept
{
    CRIMILD_LOG_TRACE();

    ShaderStageArray shaderStages;
    for ( const auto &module : modules ) {
        auto stage = VkPipelineShaderStageCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = module->stage,
            .module = module->handler,
            .pName = module->entryPointName.c_str(),
        };
        shaderStages.push_back( stage );
    }
    return shaderStages;
}
