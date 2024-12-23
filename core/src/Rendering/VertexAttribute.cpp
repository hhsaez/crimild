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

#include "Rendering/VertexAttribute.hpp"

using namespace crimild;

VertexAttribute::VertexAttribute( VertexAttribute::Name name, Format format, crimild::UInt32 offset ) noexcept
    : m_name( name ),
      m_format( format ),
      m_offset( offset )
{
    // no-op
}

bool VertexAttribute::operator==( const VertexAttribute &other ) const noexcept
{
    return m_name == other.m_name && m_format == other.m_format && m_offset == other.m_offset;
}

void VertexAttribute::encode( coding::Encoder &encoder )
{
    coding::Codable::encode( encoder );

    encoder.encodeEnum( "name", m_name );
    encoder.encodeEnum( "format", m_format );
    encoder.encode( "offset", m_offset );
}

void VertexAttribute::decode( coding::Decoder &decoder )
{
    coding::Codable::decode( decoder );

    decoder.decodeEnum( "name", m_name );
    decoder.decodeEnum( "format", m_format );
    decoder.decode( "offset", m_offset );
}
