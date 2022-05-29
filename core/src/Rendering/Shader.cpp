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
 *     * Neither the name of the copuright holder nor the
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

#include "Rendering/Shader.hpp"

#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"
#include "Simulation/FileSystem.hpp"

#include <cstring>

using namespace crimild;

std::string Shader::getStageDescription( const Stage &stage ) noexcept
{
    switch ( stage ) {
        case Stage::VERTEX:
            return "VERTEX";
        case Stage::TESSELLATION_CONTROL:
            return "TESSELLATION_CONTROL";
        case Stage::TESSELLATION_EVALUATION:
            return "TESSELLATION_EVALUATION";
        case Stage::GEOMETRY:
            return "GEOMETRY";
        case Stage::FRAGMENT:
            return "FRAGMENT";
        case Stage::COMPUTE:
            return "COMPUTE";
        case Stage::ALL_GRAPHICS:
            return "ALL_GRAPHICS";
        case Stage::ALL:
            return "ALL";
        default:
            break;
    }

    // Should never happen. Thrown exception instead?
    return "UNKNOWN";
}

SharedPointer< Shader > Shader::withSource( Stage stage, const FilePath &filePath ) noexcept
{
    return crimild::alloc< Shader >(
        stage,
        FileSystem::getInstance().readFile( filePath.getAbsolutePath() ),
        DataType::INLINE
    );
}

SharedPointer< Shader > Shader::withBinary( Stage stage, const FilePath &filePath ) noexcept
{
    return crimild::alloc< Shader >(
        stage,
        FileSystem::getInstance().readFile( filePath.getAbsolutePath() ),
        DataType::BINARY
    );
}

Shader::Shader( Stage stage, const std::string &source, const std::string &entryPointName ) noexcept
    : m_stage( stage ),
      m_dataType( DataType::INLINE ),
      m_entryPointName( entryPointName )
{
    m_data.resize( source.length() );
    memcpy( m_data.data(), source.data(), source.length() );
}

Shader::Shader( Stage stage, const Data &data, DataType dataType, std::string entryPointName ) noexcept
    : m_stage( stage ),
      m_dataType( dataType ),
      m_data( data ),
      m_entryPointName( entryPointName )
{
}

Shader::Shader( std::string source )
    : m_source( source )
{
}

void Shader::encode( coding::Encoder &encoder )
{
    Codable::encode( encoder );

    encoder.encodeEnum( "stage", m_stage );
    encoder.encode( "data", m_data );
    encoder.encodeEnum( "data_type", m_dataType );
    encoder.encode( "entry_point_name", m_entryPointName );
}

void Shader::decode( coding::Decoder &decoder )
{
    Codable::decode( decoder );

    decoder.decodeEnum( "stage", m_stage );
    decoder.decode( "data", m_data );
    decoder.decodeEnum( "data_type", m_dataType );
    decoder.decode( "entry_point_name", m_entryPointName );
}
