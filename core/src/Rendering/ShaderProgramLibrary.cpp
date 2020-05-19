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

#include "Rendering/ShaderProgramLibrary.hpp"

#include "Rendering/DescriptorSet.hpp"
#include "Rendering/ShaderLibrary.hpp"
#include "Rendering/ShaderProgram.hpp"

using namespace crimild;

template<>
RenderResourceLibrary< ShaderProgram >::RenderResourceLibrary( void ) noexcept
{
    add(
        constants::SHADER_PROGRAM_UNLIT_P2C3_COLOR,
        [] {
            auto program = crimild::alloc< ShaderProgram >(
                containers::Array< SharedPointer< Shader >> {
                    crimild::retain( ShaderLibrary::getInstance()->get( constants::SHADER_UNLIT_VERTEX_P2C3 ) ),
                    crimild::retain( ShaderLibrary::getInstance()->get( constants::SHADER_UNLIT_FRAGMENT_COLOR ) ),
                }
            );

            program->attributeDescriptions = VertexP2C3::getAttributeDescriptions( 0 );
            program->bindingDescription = VertexP2C3::getBindingDescription( 0 );
            program->descriptorSetLayouts = {
				[] {
					auto layout = crimild::alloc< DescriptorSetLayout >();
					layout->bindings = {
						{
							.descriptorType = DescriptorType::UNIFORM_BUFFER,
							.descriptorCount = 1,
							.stage = Shader::Stage::VERTEX,
						},
					};
					return layout;
				}(),
			};

            return program;
    	}
    );

    add(
        constants::SHADER_PROGRAM_UNLIT_P2C3TC2_TEXTURE_COLOR,
        [] {
            auto program = crimild::alloc< ShaderProgram >(
                containers::Array< SharedPointer< Shader >> {
                    crimild::retain( ShaderLibrary::getInstance()->get( constants::SHADER_UNLIT_VERTEX_P2C3TC2 ) ),
                    crimild::retain( ShaderLibrary::getInstance()->get( constants::SHADER_UNLIT_FRAGMENT_TEXTURE_COLOR ) ),
                }
            );

            program->attributeDescriptions = VertexP2C3TC2::getAttributeDescriptions( 0 );
            program->bindingDescription = VertexP2C3TC2::getBindingDescription( 0 );
            program->descriptorSetLayouts = {
				[] {
					auto layout = crimild::alloc< DescriptorSetLayout >();
					layout->bindings = {
						{
							.descriptorType = DescriptorType::UNIFORM_BUFFER,
							.descriptorCount = 1,
							.stage = Shader::Stage::VERTEX,
						},
						{
							.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
							.descriptorCount = 1,
							.stage = Shader::Stage::FRAGMENT,
						}
					};
					return layout;
				}(),
			};

            return program;
        }
    );

    add(
        constants::SHADER_PROGRAM_UNLIT_SKYBOX_P3,
        [] {
            auto program = crimild::alloc< ShaderProgram >(
                containers::Array< SharedPointer< Shader >> {
                    crimild::retain( ShaderLibrary::getInstance()->get( constants::SHADER_UNLIT_SKYBOX_P3_VERT ) ),
                    crimild::retain( ShaderLibrary::getInstance()->get( constants::SHADER_UNLIT_SKYBOX_P3_FRAG ) ),
                }
            );

            program->attributeDescriptions = VertexP3::getAttributeDescriptions( 0 );
            program->bindingDescription = VertexP3::getBindingDescription( 0 );
            program->descriptorSetLayouts = {
				[] {
					auto layout = crimild::alloc< DescriptorSetLayout >();
					layout->bindings = {
						{
							.descriptorType = DescriptorType::UNIFORM_BUFFER,
							.descriptorCount = 1,
							.stage = Shader::Stage::VERTEX,
						},
						{
							.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
							.descriptorCount = 1,
							.stage = Shader::Stage::FRAGMENT,
						}
					};
					return layout;
				}(),
			};

            return program;
        }
    );

    add(
        constants::SHADER_PROGRAM_UNLIT_TEXTURE_P3N3TC2,
        [] {
            auto program = crimild::alloc< ShaderProgram >(
                containers::Array< SharedPointer< Shader >> {
                    crimild::retain( ShaderLibrary::getInstance()->get( constants::SHADER_UNLIT_TEXTURE_P3N3TC2_VERT ) ),
                    crimild::retain( ShaderLibrary::getInstance()->get( constants::SHADER_UNLIT_TEXTURE_P3N3TC2_FRAG ) ),
                }
            );

            program->attributeDescriptions = VertexP3N3TC2::getAttributeDescriptions( 0 );
            program->bindingDescription = VertexP3N3TC2::getBindingDescription( 0 );
            program->descriptorSetLayouts = {
				[] {
					auto layout = crimild::alloc< DescriptorSetLayout >();
					layout->bindings = {
						{
							.descriptorType = DescriptorType::UNIFORM_BUFFER,
							.descriptorCount = 1,
							.stage = Shader::Stage::VERTEX,
						},
						{
							.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
							.descriptorCount = 1,
							.stage = Shader::Stage::FRAGMENT,
						}
					};
					return layout;
				}(),
			};

            return program;
        }
    );

    add(
        constants::SHADER_PROGRAM_DEBUG_POSITION_P3,
        [] {
            auto program = crimild::alloc< ShaderProgram >(
                containers::Array< SharedPointer< Shader >> {
                    crimild::retain( ShaderLibrary::getInstance()->get( constants::SHADER_DEBUG_POSITION_P3_VERT ) ),
                    crimild::retain( ShaderLibrary::getInstance()->get( constants::SHADER_DEBUG_POSITION_P3_FRAG ) ),
                }
            );

            program->attributeDescriptions = VertexP3::getAttributeDescriptions( 0 );
            program->bindingDescription = VertexP3::getBindingDescription( 0 );
            program->descriptorSetLayouts = {
				[] {
					auto layout = crimild::alloc< DescriptorSetLayout >();
					layout->bindings = {
						{
							.descriptorType = DescriptorType::UNIFORM_BUFFER,
							.descriptorCount = 1,
							.stage = Shader::Stage::VERTEX,
						},
					};
					return layout;
				}(),
			};

            return program;
        }
    );

    add(
        constants::SHADER_PROGRAM_DEBUG_POSITION_P3N3TC2,
        [] {
            auto program = crimild::alloc< ShaderProgram >(
                containers::Array< SharedPointer< Shader >> {
                    crimild::retain( ShaderLibrary::getInstance()->get( constants::SHADER_DEBUG_POSITION_P3N3TC2_VERT ) ),
                    crimild::retain( ShaderLibrary::getInstance()->get( constants::SHADER_DEBUG_POSITION_P3N3TC2_FRAG ) ),
                }
            );

            program->attributeDescriptions = VertexP3N3TC2::getAttributeDescriptions( 0 );
            program->bindingDescription = VertexP3N3TC2::getBindingDescription( 0 );
            program->descriptorSetLayouts = {
				[] {
					auto layout = crimild::alloc< DescriptorSetLayout >();
					layout->bindings = {
						{
							.descriptorType = DescriptorType::UNIFORM_BUFFER,
							.descriptorCount = 1,
							.stage = Shader::Stage::VERTEX,
						},
					};
					return layout;
				}(),
			};

            return program;
        }
    );
}

