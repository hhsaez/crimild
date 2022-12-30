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

#include "Rendering/VulkanShaderModule.hpp"

#include "Rendering/Shader.hpp"
#include "Rendering/VulkanRenderDeviceOLD.hpp"

using namespace crimild;
using namespace crimild::vulkan;

ShaderModuleOLD::~ShaderModuleOLD( void )
{
    if ( manager != nullptr ) {
        manager->destroy( this );
    }
}

SharedPointer< ShaderModuleOLD > ShaderModuleManager::create( ShaderModuleOLD::Descriptor const &descriptor ) noexcept
{
    CRIMILD_LOG_DEBUG( "Creating shader module for stage ", descriptor.shader->getStageDescription() );

    auto renderDevice = m_renderDevice;
    if ( renderDevice == nullptr ) {
        renderDevice = descriptor.renderDevice;
    }

    auto shader = descriptor.shader;
    assert( shader != nullptr && "Shader instance is null" );

    auto code = shader->getData();
    if ( shader->getDataType() == Shader::DataType::INLINE ) {
        auto source = std::string( reinterpret_cast< const char * >( code.data() ), code.size() );
        if ( !getShaderCompiler().compile( shader->getStage(), source, code ) ) {
            CRIMILD_LOG_ERROR( "Failed to create shader module" );
            return nullptr;
        }
    }

    auto createInfo = VkShaderModuleCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = code.size(),
        .pCode = reinterpret_cast< const uint32_t * >( code.data() ),
    };

    VkShaderModule shaderModuleHandler;
    if ( vkCreateShaderModule( renderDevice->handler, &createInfo, nullptr, &shaderModuleHandler ) != VK_SUCCESS ) {
        CRIMILD_LOG_ERROR( "Failed to create shader module" );
        return nullptr;
    }

    auto shaderModule = crimild::alloc< ShaderModuleOLD >();
    shaderModule->renderDevice = renderDevice;
    shaderModule->handler = shaderModuleHandler;
    shaderModule->manager = this;
    shaderModule->stage = utils::getVulkanShaderStageFlag( descriptor.shader->getStage() );
    shaderModule->entryPointName = descriptor.shader->getEntryPointName();
    insert( crimild::get_ptr( shaderModule ) );
    return shaderModule;
}

void ShaderModuleManager::destroy( ShaderModuleOLD *shaderModule ) noexcept
{
    if ( shaderModule->renderDevice != nullptr && shaderModule->handler != VK_NULL_HANDLE ) {
        vkDestroyShaderModule( shaderModule->renderDevice->handler, shaderModule->handler, nullptr );
    }

    shaderModule->renderDevice = nullptr;
    shaderModule->handler = VK_NULL_HANDLE;
    shaderModule->manager = nullptr;
    erase( shaderModule );
}
