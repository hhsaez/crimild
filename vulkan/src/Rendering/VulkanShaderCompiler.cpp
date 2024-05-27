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

#include "Crimild_Foundation.hpp"
#include "Rendering/Shader.hpp"

// TODO (hernan): In order for the Vulkan shader compiler to work correctly, make sure there
// is no `glslang` directory in VULKAN_SDK/macOS/include
// That way, the compiler uses the header files that are in `third-party` instead.
// I definitely need to fix this

#include <SPIRV/GlslangToSpv.h>
#include <fstream>
#include <glslang/Public/ShaderLang.h>
#include <iostream>
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
    CRIMILD_LOG_TRACE();

    if ( !m_initialized ) {
        glslang::InitializeProcess();
        initPreprocessor();
        m_initialized = true;
    }
    return m_initialized;
}

bool vulkan::ShaderCompiler::compile( Shader::Stage shaderStage, const std::string &source, Shader::Data &out ) noexcept
{
    if ( !init() ) {
        return false;
    }

    CRIMILD_LOG_DEBUG( "Compiling shader for stage ", int( shaderStage ) );

    auto stage = getShaderStage( shaderStage );

    auto prefix = std::string(
        R"(
            #version 450
            #extension GL_ARB_separate_shader_objects : enable
        )"
    );

    auto src = prefix + m_preprocessor.expand( source );
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
    EShMessages messages = ( EShMessages ) ( EShMsgSpvRules | EShMsgVulkanRules );

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
    } else {
        CRIMILD_LOG_ERROR(
            "GLSL preprocessing failed for shader source:\n",
            shaderLines( src ),
            "\n",
            tShader.getInfoLog(),
            "\n",
            tShader.getInfoDebugLog()
        );
        exit( -1 );
        return false;
    }

    if ( !tShader.parse( ( const TBuiltInResource * ) &resources, defaultVersion, ENoProfile, false, false, messages, includer ) ) {
        CRIMILD_LOG_ERROR(
            "GLSL parsing failed for shader source:\n",
            shaderLines( src ),
            "\n",
            tShader.getInfoLog(),
            "\n",
            tShader.getInfoDebugLog()
        );
        exit( -1 );
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
        exit( -1 );
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

void vulkan::ShaderCompiler::initPreprocessor( void ) noexcept
{
    if ( m_preprocessor.isInitialized() ) {
        return;
    }

    m_preprocessor.addChunk(
        "random",
        R"(
            #ifndef CRIMILD_GLSL_RANDOM
            #define CRIMILD_GLSL_RANDOM

            uint seed = 0;
            int flat_idx = 0;

            void encrypt_tea( inout uvec2 arg ) {
                uvec4 key = uvec4( 0xa341316c, 0xc8013ea4, 0xad90777d, 0x7e95761e );
                uint v0 = arg[ 0 ], v1 = arg[ 1 ];
                uint sum = 0u;
                uint delta = 0x9e3779b9u;

                for ( int i = 0; i < 32; i++ ) {
                    sum += delta;
                    v0 += ( ( v1 << 4 ) + key[ 0 ] ) ^ ( v1 + sum ) ^ ( ( v1 >> 5 ) + key[ 1 ] );
                    v1 += ( ( v0 << 4 ) + key[ 2 ] ) ^ ( v0 + sum ) ^ ( ( v0 >> 5 ) + key[ 3 ] );
                }
                arg[ 0 ] = v0;
                arg[ 1 ] = v1;
            }
            float getRandom() {
                uvec2 arg = uvec2( flat_idx, seed++ );
                encrypt_tea( arg );
                vec2 r = fract( vec2( arg ) / vec2( 0xffffffffu ) );
                return r.x;
            }

            float getRandomRange( float min, float max ) {
                return min + getRandom() * ( max - min );
            }

            vec3 getRandomVec3() {
                return vec3(
                    getRandom(),
                    getRandom(),
                    getRandom() );
            }

            vec3 getRandomVec3Range( float min, float max ) {
                return vec3(
                    getRandomRange( min, max ),
                    getRandomRange( min, max ),
                    getRandomRange( min, max ) );
            }

            vec3 getRandomInUnitSphere() {
                while ( true ) {
                    vec3 p = getRandomVec3Range( -1.0, 1.0 );
                    if ( dot( p, p ) < 1.0 ) {
                        return p;
                    }
                }
                return vec3( 0 );
            }

            vec3 getRandomInUnitDisc() {
                while ( true ) {
                    vec3 p = vec3(
                        getRandomRange( -1.0, 1.0 ),
                        getRandomRange( -1.0, 1.0 ),
                        0.0 );
                    if ( dot( p, p ) >= 1.0 ) {
                        break;
                    }
                    return p;
                }
            }

            vec3 getRandomUnitVector() {
                return normalize( getRandomInUnitSphere() );
            }

            vec3 getRandomInHemisphere( vec3 N ) {
                vec3 inUnitSphere = getRandomInUnitSphere();
                if ( dot( inUnitSphere, N ) > 0.0 ) {
                    return inUnitSphere;
                } else {
                    return -inUnitSphere;
                }
            }

            void initRandom( int seedValue )
            {
                seed = seedValue;
                 flat_idx = int( dot( gl_GlobalInvocationID.xy, vec2( 1, 4096 ) ) );
            }

            #endif
        )"
    );

    m_preprocessor.addChunk(
        "textureCube",
        R"(
            #ifndef CRIMILD_GLSL_TEXTURE_CUBE
            #define CRIMILD_GLSL_TEXTURE_CUBE

            const float FACE_INVALID = -1.0;
            const float FACE_LEFT = 0.0;
            const float FACE_RIGHT = 1.0;
            const float FACE_FRONT = 2.0;
            const float FACE_BACK = 3.0;
            const float FACE_UP = 4.0;
            const float FACE_DOWN = 5.0;

            // Return the face in cubemap based on the principle component of the direction
            float getFace( vec3 direction )
            {
                vec3 absDirection = abs( direction );
                float face = -1.0;
                if ( absDirection.x > absDirection.z ) {
                    if ( absDirection.x > absDirection.y ) {
                        return direction.x > 0.0 ? FACE_RIGHT : FACE_LEFT;
                    } else {
                        return direction.y > 0.0 ? FACE_UP : FACE_DOWN;
                    }
                } else {
                    if ( absDirection.z > absDirection.y ) {
                        return direction.z > 0.0 ? FACE_FRONT : FACE_BACK;
                    } else {
                        return direction.y > 0.0 ? FACE_UP : FACE_DOWN;
                    }
                }
                return FACE_INVALID;
            }

            vec2 getUV( vec3 direction, float face )
            {
                vec2 uv;
                if ( face == FACE_LEFT ) {
                    uv = vec2( -direction.z, direction.y ) / abs( direction.x );
                } else if ( face == FACE_RIGHT ) {
                    uv = vec2( direction.z, direction.y ) / abs( direction.x );
                } else if ( face == FACE_FRONT ) {
                    uv = vec2( -direction.x, direction.y ) / abs( direction.z );
                } else if ( face == FACE_BACK ) {
                    uv = vec2( direction.x, direction.y ) / abs( direction.z );
                } else if ( face == FACE_UP ) {
                    uv = vec2( direction.x, direction.z ) / abs( direction.y );
                } else if ( face == FACE_DOWN ) {
                    uv = vec2( direction.x, -direction.z ) / abs( direction.y );
                }
                return 0.5 + 0.5 * uv;
            }

            vec2 getFaceOffsets( float face )
            {
                if ( face == FACE_LEFT ) {
                    return vec2( 0.0, 0.5 );
                } else if ( face == FACE_RIGHT ) {
                    return vec2( 0.5, 0.5 );
                } else if ( face == FACE_FRONT ) {
                    return vec2( 0.25, 0.5 );
                } else if ( face == FACE_BACK ) {
                    return vec2( 0.75, 0.5 );
                } else if ( face == FACE_UP ) {
                    return vec2( 0.5, 0.25 );
                } else if ( face == FACE_DOWN ) {
                    return vec2( 0.5, 0.75 );
                }
            }

            // Performs bilinear filtering
            vec4 textureCubeUV( sampler2D envMap, vec3 direction, vec4 viewport )
            {
                const float faceSize = 0.25;
                const vec2 texelSize = 1.0 / textureSize( envMap, 0 );

                float face = getFace( direction );
                vec2 faceOffsets = getFaceOffsets( face );

                vec2 uv = getUV( direction, face );
                uv.y = 1.0 - uv.y;
                uv = faceOffsets + faceSize * uv;
                uv = viewport.xy + uv * viewport.zw;

                // make sure UV values are within the face to avoid most artifacts in the borders
                // of the cube map. Some visual artifacts migth still appear, though, but they
                // should be rare.
                vec2 fBL = viewport.xy + ( faceOffsets ) * viewport.zw;
                vec2 fTR = viewport.xy + ( faceOffsets + vec2( faceSize ) ) * viewport.zw;
                uv = clamp( uv, fBL + texelSize, fTR - 2.0 * texelSize );

                vec4 color = texture( envMap, uv );
                return color;
            }

            vec4 textureCubeLOD( sampler2D envMap, vec3 D, int lod )
            {
                vec4 viewport = vec4( 0, 0, 1, 1 );
                if ( lod == 0 ) {
                    viewport = vec4( 0, 0, 0.6666666667, 1.0 );
                } else if ( lod == 1 ) {
                    viewport = vec4( 0.6666666667, 0, 0.3333333333, 0.3333333333 );
                } else if ( lod == 2 ) {
                    viewport = vec4( 0.6666666667, 0.3333333333, 0.1666666667, 0.1666666667 );
                } else if ( lod == 3 ) {
                    viewport = vec4( 0.6666666667, 0.5, 0.08333333333, 0.08333333333 );
                } else {
                    viewport = vec4( 0.6666666667, 0.5833333333, 0.04166666667, 0.04166666667 );
                }
                return textureCubeUV(
                    envMap,
                    D,
                    viewport
                );
            }

            #endif
        )"
    );

    m_preprocessor.addChunk(
        "linearizeDepth",
        R"(
            #ifndef CRIMILD_GLSL_LINEARIZE_DEPTH
            #define CRIMILD_GLSL_LINEARIZE_DEPTH

            float linearizeDepth( float depth, float nearPlane, float farPlane )
            {
                float z = depth * 2.0 - 1.0; // Back to NDC
                return ( 2.0 * nearPlane * farPlane ) / ( farPlane + nearPlane - z * ( farPlane - nearPlane ) ) / farPlane;
            }

            #endif
        )"
    );

    m_preprocessor.addChunk(
        "isZero",
        R"(
            #ifndef CRIMILD_GLSL_IS_ZERO
            #define CRIMILD_GLSL_IS_ZERO

            bool isZero( float x )
            {
                float EPSILON = 0.000001;
                return abs( x ) < EPSILON;
            }

            bool isZero( vec3 v ) {
                float s = 0.00001;
                return abs( v.x ) < s && abs( v.y ) < s && abs( v.z ) < s;
            }

            #endif
        )"
    );

    m_preprocessor.addChunk(
        "reflectance",
        R"(
            #ifndef CRIMILD_GLSL_IS_REFLECTANCE
            #define CRIMILD_GLSL_IS_REFLECTANCE

            float reflectance( float cosine, float refIdx ) {
                float r0 = ( 1.0 - refIdx ) / ( 1.0 + refIdx );
                r0 = r0 * r0;
                return r0 + ( 1.0 - r0 ) * pow( ( 1.0 - cosine ), 5.0 );
            }

            #endif
        )"
    );

    m_preprocessor.addChunk(
        "max",
        R"(
            #ifndef CRIMILD_GLSL_MAX
            #define CRIMILD_GLSL_MAX

            int maxDimension( vec3 u )
            {
                int ret = 0;
                if ( u[ 1 ] > u[ ret ] ) {
                    ret = 1;
                }
                if ( u[ 2 ] > u[ ret ] ) {
                    ret = 2;
                }
                return ret;
            }

            #endif
        )"
    );

    m_preprocessor.addChunk(
        "swapsHandedness",
        R"(
            #ifndef CRIMILD_GLSL_SWAPS_HANDEDNESS
            #define CRIMILD_GLSL_SWAPS_HANDEDNESS

            bool swapsHandedness( mat4 M )
            {
                return determinant( mat3( M ) ) < 0;
            }

            #endif
        )"
    );

    resetPreprocessor();
}

void vulkan::ShaderCompiler::resetPreprocessor( void ) noexcept
{
    m_preprocessor.addChunk(
        "vert_main",
        R"(
            vec4 vert_main( inout Vertex vert, mat4 proj, mat4 view, mat4 model )
            {
                return proj * view * vec4( vert.worldPosition, 1.0 );
            }
        )"
    );

    m_preprocessor.addChunk(
        "frag_main",
        "void frag_main( inout Fragment frag ) { }"
    );
}

void vulkan::ShaderCompiler::addChunks( const Array< SharedPointer< Shader > > &chunks ) noexcept
{
    chunks.each(
        [ & ]( auto shader ) {
            if ( shader->getDataType() != Shader::DataType::INLINE ) {
                // Ignore shaders without code
                return;
            }
            const auto &code = shader->getData();
            auto src = std::string( reinterpret_cast< const char * >( code.data() ), code.size() );
            switch ( shader->getStage() ) {
                case Shader::Stage::VERTEX:
                    m_preprocessor.addChunk( "vert_main", src );
                    break;
                case Shader::Stage::FRAGMENT:
                    m_preprocessor.addChunk( "frag_main", src );
                    break;
                default:
                    break;
            }
        }
    );
}
