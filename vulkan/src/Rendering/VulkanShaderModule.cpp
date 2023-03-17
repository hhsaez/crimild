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
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/VulkanRenderDevice.hpp"

using namespace crimild;
using namespace crimild::vulkan;

std::vector< std::shared_ptr< ShaderModule > > ShaderModule::createShaderModulesFromProgram(
    RenderDevice *device,
    const ShaderProgram *program
) noexcept
{
    assert( program != nullptr && "Invalid shader program instance" );
    assert( !program->getShaders().empty() && "Invalid shader program" );

    std::vector< std::shared_ptr< ShaderModule > > ret;

    program->getShaders().each(
        [ &ret, device ]( auto &shader ) {
            ret.push_back( crimild::alloc< ShaderModule >( device, shader ) );
        }
    );

    // Reset preprocessor after building shader modules to prevent garbage code to be used unintentionally
    device->getShaderCompiler().resetPreprocessor();

    return ret;
}

ShaderModule::ShaderModule( RenderDevice *device, SharedPointer< Shader > const &shader ) noexcept
    : WithRenderDevice( device )
{
    assert( shader != nullptr && "Shader instance is null" );

    auto code = shader->getData();
    if ( shader->getDataType() == Shader::DataType::INLINE ) {
        auto source = std::string( reinterpret_cast< const char * >( code.data() ), code.size() );
        if ( !device->getShaderCompiler().compile( shader->getStage(), source, code ) ) {
            CRIMILD_LOG_FATAL( "Failed to create shader module" );
            exit( EXIT_FAILURE );
        }
    }

    auto createInfo = VkShaderModuleCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = code.size(),
        .pCode = reinterpret_cast< const uint32_t * >( code.data() ),
    };

    VkShaderModule handle;
    if ( vkCreateShaderModule( device->getHandle(), &createInfo, nullptr, &handle ) != VK_SUCCESS ) {
        CRIMILD_LOG_FATAL( "Failed to create shader module" );
        exit( EXIT_FAILURE );
    }
    setHandle( handle );

    m_stage = utils::getVulkanShaderStageFlag( shader->getStage() );
    m_entryPointName = shader->getEntryPointName();
}

ShaderModule::~ShaderModule( void ) noexcept
{
    vkDestroyShaderModule( getRenderDevice()->getHandle(), getHandle(), getRenderDevice()->getAllocator() );
    setHandle( VK_NULL_HANDLE );
}
