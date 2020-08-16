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

#include "Rendering/VulkanShaderCompiler.hpp"

#include "Rendering/Shader.hpp"
#include "Foundation/Log.hpp"

// TODO (hernan): In order for the Vulkan shader compiler to work correctly, make sure there
// is no `glslang` directory in VULKAN_SDK/macOS/include
// That way, the compiler uses the header files that are in `third-party` instead.
// I definitely need to fix this

#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace crimild;

std::string getSuffix( const std::string &path ) noexcept
{
    auto pos = path.rfind( '.' );
    return ( pos == std::string::npos ) ? "" : path.substr( pos + 1 );
}

EShLanguage getShaderStage( const Shader::Stage &stage ) noexcept
{
    switch ( stage ) {
        case Shader::Stage::VERTEX:
            return EShLangVertex;
        case Shader::Stage::TESSELLATION_CONTROL:
            return EShLangTessControl;
        case Shader::Stage::TESSELLATION_EVALUATION:
            return EShLangTessEvaluation;
        case Shader::Stage::GEOMETRY:
            return EShLangGeometry;
        case Shader::Stage::FRAGMENT:
            return EShLangFragment;
        case Shader::Stage::COMPUTE:
            return EShLangCompute;
        default:
            CRIMILD_LOG_FATAL( "Invalid shader stage: ", int( stage ) );
            return EShLangCount;
    }
}

const TBuiltInResource DefaultTBuiltInResource = {
    .maxLights = 32,
    .maxClipPlanes = 6,
    .maxTextureUnits = 32,
    .maxTextureCoords = 32,
    .maxVertexAttribs = 64,
    .maxVertexUniformComponents = 4096,
    .maxVaryingFloats = 64,
    .maxVertexTextureImageUnits = 32,
    .maxCombinedTextureImageUnits = 80,
    .maxTextureImageUnits = 32,
    .maxFragmentUniformComponents = 4096,
    .maxDrawBuffers = 32,
    .maxVertexUniformVectors = 128,
    .maxVaryingVectors = 8,
    .maxFragmentUniformVectors = 16,
    .maxVertexOutputVectors = 16,
    .maxFragmentInputVectors = 15,
    .minProgramTexelOffset = -8,
    .maxProgramTexelOffset = 7,
    .maxClipDistances = 8,
    .maxComputeWorkGroupCountX = 65535,
    .maxComputeWorkGroupCountY = 65535,
    .maxComputeWorkGroupCountZ = 65535,
    .maxComputeWorkGroupSizeX = 1024,
    .maxComputeWorkGroupSizeY = 1024,
    .maxComputeWorkGroupSizeZ = 64,
    .maxComputeUniformComponents = 1024,
    .maxComputeTextureImageUnits = 16,
    .maxComputeImageUniforms = 8,
    .maxComputeAtomicCounters = 8,
    .maxComputeAtomicCounterBuffers = 1,
    .maxVaryingComponents = 60,
    .maxVertexOutputComponents = 64,
    .maxGeometryInputComponents = 64,
    .maxGeometryOutputComponents = 128,
    .maxFragmentInputComponents = 128,
    .maxImageUnits = 8,
    .maxCombinedImageUnitsAndFragmentOutputs = 8,
    .maxCombinedShaderOutputResources = 8,
    .maxImageSamples = 0,
    .maxVertexImageUniforms = 0,
    .maxTessControlImageUniforms = 0,
    .maxTessEvaluationImageUniforms = 0,
    .maxGeometryImageUniforms = 0,
    .maxFragmentImageUniforms = 8,
    .maxCombinedImageUniforms = 8,
    .maxGeometryTextureImageUnits = 16,
    .maxGeometryOutputVertices = 256,
    .maxGeometryTotalOutputComponents = 1024,
    .maxGeometryUniformComponents = 1024,
    .maxGeometryVaryingComponents = 64,
    .maxTessControlInputComponents = 128,
    .maxTessControlOutputComponents = 128,
    .maxTessControlTextureImageUnits = 16,
    .maxTessControlUniformComponents = 1024,
    .maxTessControlTotalOutputComponents = 4096,
    .maxTessEvaluationInputComponents = 128,
    .maxTessEvaluationOutputComponents = 128,
    .maxTessEvaluationTextureImageUnits = 16,
    .maxTessEvaluationUniformComponents = 1024,
    .maxTessPatchComponents = 120,
    .maxPatchVertices = 32,
    .maxTessGenLevel = 64,
    .maxViewports = 16,
    .maxVertexAtomicCounters = 0,
    .maxTessControlAtomicCounters = 0,
    .maxTessEvaluationAtomicCounters = 0,
    .maxGeometryAtomicCounters = 0,
    .maxFragmentAtomicCounters = 8,
    .maxCombinedAtomicCounters = 8,
    .maxAtomicCounterBindings = 1,
    .maxVertexAtomicCounterBuffers = 0,
    .maxTessControlAtomicCounterBuffers = 0,
    .maxTessEvaluationAtomicCounterBuffers = 0,
    .maxGeometryAtomicCounterBuffers = 0,
    .maxFragmentAtomicCounterBuffers = 1,
    .maxCombinedAtomicCounterBuffers = 1,
    .maxAtomicCounterBufferSize = 16384,
    .maxTransformFeedbackBuffers = 4,
    .maxTransformFeedbackInterleavedComponents = 64,
    .maxCullDistances = 8,
    .maxCombinedClipAndCullDistances = 8,
    .maxSamples = 4,
    .limits = {
        .nonInductiveForLoops = 1,
        .whileLoops = 1,
        .doWhileLoops = 1,
        .generalUniformIndexing = 1,
        .generalAttributeMatrixVectorIndexing = 1,
        .generalVaryingIndexing = 1,
        .generalSamplerIndexing = 1,
        .generalVariableIndexing = 1,
        .generalConstantMatrixVectorIndexing = 1,
    },
};

bool vulkan::ShaderCompiler::init( void ) noexcept
{
    CRIMILD_LOG_TRACE( "Initializing shader compiler" );

    if ( !m_initialized ) {
        glslang::InitializeProcess();
        m_initialized = true;
    }
    return m_initialized;
}

bool vulkan::ShaderCompiler::compile( Shader::Stage shaderStage, const std::string &source, Shader::Data &out ) noexcept
{
    if ( !init() ) {
        return false;
    }

    CRIMILD_LOG_TRACE( "Compiling shader for stage ", int( shaderStage ) );

    auto stage = getShaderStage( shaderStage );

    auto prefix = std::string(
        R"(
            #version 450
            #extension GL_ARB_separate_shader_objects : enable
        )"
    );

    auto src = prefix + source;
    auto data = src.c_str();

    auto tShader = glslang::TShader( stage );
    tShader.setStrings( &data, 1 );

    Int32 clientInputSematincsVersion = 100; // #define VULKAN 100
    glslang::EShTargetClientVersion vulkanClientVersion = glslang::EShTargetVulkan_1_0;
    glslang::EShTargetLanguageVersion targetVersion = glslang::EShTargetSpv_1_0;

    tShader.setEnvInput( glslang::EShSourceGlsl, stage, glslang::EShClientVulkan, clientInputSematincsVersion );
    tShader.setEnvClient( glslang::EShClientVulkan, vulkanClientVersion );
    tShader.setEnvTarget( glslang::EShTargetSpv, targetVersion );

    auto &resources = DefaultTBuiltInResource;
    EShMessages messages = ( EShMessages )( EShMsgSpvRules | EShMsgVulkanRules );

    const int defaultVersion = 110;

    glslang::TShader::ForbidIncluder includer;

    auto shaderLines = []( std::string input ) {
        std::stringstream out;
        std::stringstream ss( input );
        std::string buffer;
        std::vector< std::string > lines;
        while ( std::getline( ss, buffer, '\n' ) ) {
            lines.push_back( buffer );
        }

        auto lineCount = 1l;
        for ( auto line : lines ) {
            out << "\t" << lineCount++ << ": " << line << "\n";
        };

        return out.str();
    };

    std::string preprocessedGLSL;
    if ( tShader.preprocess( &resources, defaultVersion, ENoProfile, false, false, messages, &preprocessedGLSL, includer ) ) {
        auto preprocessedGLSLStr = preprocessedGLSL.c_str();
        tShader.setStrings( &preprocessedGLSLStr, 1 );
    }
    else {
        CRIMILD_LOG_ERROR(
            "GLSL preprocessing failed for shader source:\n",
            shaderLines( src ),
            "\n",
            tShader.getInfoLog(),
            "\n",
            tShader.getInfoDebugLog()
        );
        return false;
    }

    if ( !tShader.parse( ( const TBuiltInResource* ) &resources, defaultVersion, ENoProfile, false, false, messages, includer ) ) {
        CRIMILD_LOG_ERROR(
            "GLSL parsing failed for shader source:\n",
            shaderLines( src ),
            "\n",
            tShader.getInfoLog(),
            "\n",
            tShader.getInfoDebugLog()
        );
        return false;
    }

    glslang::TProgram program;
    program.addShader( &tShader );
    if ( !program.link( messages ) ) {
        CRIMILD_LOG_ERROR(
            "GLSL linking failed for shader source:\n",
            shaderLines( src ),
            "\n",
            tShader.getInfoLog(),
            "\n",
            tShader.getInfoDebugLog()
        );
        return false;
    }

    std::vector< unsigned int > spirv;
    spv::SpvBuildLogger logger;
    glslang::SpvOptions spvOptions;
    glslang::GlslangToSpv( *program.getIntermediate( stage ), spirv, &logger, &spvOptions );

    out.resize( spirv.size() * sizeof( unsigned int ) );
    memcpy( out.data(), spirv.data(), out.size() );

    return true;
}
