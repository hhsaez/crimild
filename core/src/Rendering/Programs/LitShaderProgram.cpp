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

#include "Rendering/Programs/LitShaderProgram.hpp"

#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Vertex.hpp"

using namespace crimild;

LitShaderProgram::LitShaderProgram( void ) noexcept
{
    setShaders(
        {
            crimild::alloc< Shader >(
                Shader::Stage::VERTEX,
#include "LitShaderProgram.vert"
                ),
            crimild::alloc< Shader >(
                Shader::Stage::FRAGMENT,
#include "LitShaderProgram.frag"
                ),
        } );

    vertexLayouts = { VertexP3N3TC2::getLayout() };

    descriptorSetLayouts = {
        [] {
            auto layout = crimild::alloc< DescriptorSetLayout >();
            layout->bindings = {
                {
                    .descriptorType = DescriptorType::UNIFORM_BUFFER,
                    .stage = Shader::Stage::VERTEX,
                },
                {
                    .descriptorType = DescriptorType::UNIFORM_BUFFER,
                    .stage = Shader::Stage::ALL,
                },
                {
                    // Shadow atlas
                    .descriptorType = DescriptorType::SHADOW_ATLAS,
                    .stage = Shader::Stage::FRAGMENT,
                },
                {
                    // Reflection atlas
                    .descriptorType = DescriptorType::REFLECTION_ATLAS,
                    .stage = Shader::Stage::FRAGMENT,
                },
                {
                    // Irradiance atlas
                    .descriptorType = DescriptorType::IRRADIANCE_ATLAS,
                    .stage = Shader::Stage::FRAGMENT,
                },
                {
                    // Prefilter atlas
                    .descriptorType = DescriptorType::PREFILTER_ATLAS,
                    .stage = Shader::Stage::FRAGMENT,
                },
                {
                    // BRDF LUT
                    .descriptorType = DescriptorType::BRDF_LUT,
                    .stage = Shader::Stage::FRAGMENT,
                },
            };
            return layout;
        }(),
        [] {
            auto layout = crimild::alloc< DescriptorSetLayout >();
            layout->bindings = {
                {
                    // Material properties
                    .descriptorType = DescriptorType::UNIFORM_BUFFER,
                    .stage = Shader::Stage::FRAGMENT,
                },
                {
                    // Albedo Map
                    .descriptorType = DescriptorType::ALBEDO_MAP,
                    .stage = Shader::Stage::FRAGMENT,
                },
                {
                    // Metallic Map
                    .descriptorType = DescriptorType::METALLIC_MAP,
                    .stage = Shader::Stage::FRAGMENT,
                },
                {
                    // Roughness Map
                    .descriptorType = DescriptorType::ROUGHNESS_MAP,
                    .stage = Shader::Stage::FRAGMENT,
                },
                {
                    // Ambient Occlusion Map
                    .descriptorType = DescriptorType::AMBIENT_OCCLUSION_MAP,
                    .stage = Shader::Stage::FRAGMENT,
                },
                {
                    // Normal Map
                    .descriptorType = DescriptorType::NORMAL_MAP,
                    .stage = Shader::Stage::FRAGMENT,
                },
            };
            return layout;
        }(),
        [] {
            auto layout = crimild::alloc< DescriptorSetLayout >();
            layout->bindings = {
                {
                    .descriptorType = DescriptorType::UNIFORM_BUFFER,
                    .stage = Shader::Stage::VERTEX,
                },
            };
            return layout;
        }(),
    };
}
