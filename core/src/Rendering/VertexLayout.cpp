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

#include "Rendering/VertexLayout.hpp"

#include "Crimild_Mathematics.hpp"

using namespace crimild;

const VertexLayout VertexLayout::P3 = {
    { VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
};

const VertexLayout VertexLayout::P3_C3 = {
    { VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
    { VertexAttribute::Name::COLOR, utils::getFormat< ColorRGB >() },
};

const VertexLayout VertexLayout::P3_N3 = {
    { VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
    { VertexAttribute::Name::NORMAL, utils::getFormat< Vector3f >() },
};

const VertexLayout VertexLayout::P3_TC2 = {
    { VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
    { VertexAttribute::Name::TEX_COORD, utils::getFormat< Vector2f >() },
};

const VertexLayout VertexLayout::P3_N3_TC2 = {
    { VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
    { VertexAttribute::Name::NORMAL, utils::getFormat< Vector3f >() },
    { VertexAttribute::Name::TEX_COORD, utils::getFormat< Vector2f >() },
};

VertexLayout::VertexLayout( void ) noexcept
    : m_size( 0 )
{
}

VertexLayout::VertexLayout( std::initializer_list< VertexAttribute > attribs ) noexcept
    : m_size( 0 )
{
    for ( const auto &attrib : attribs ) {
        m_attributes[ attrib.name ] = {
            .name = attrib.name,
            .format = attrib.format,
            .offset = m_size,
        };
        m_sorted.add( attrib.name );
        m_size += utils::getFormatSize( attrib.format );
    }
}

VertexLayout::VertexLayout( const Array< VertexAttribute > &attribs ) noexcept
    : m_size( 0 )
{
    attribs.each(
        [ & ]( const auto &attrib ) {
            m_attributes[ attrib.name ] = {
                .name = attrib.name,
                .format = attrib.format,
                .offset = m_size,
            };
            m_sorted.add( attrib.name );
            m_size += utils::getFormatSize( attrib.format );
        }
    );
}

crimild::Bool VertexLayout::operator==( const VertexLayout &other ) const noexcept
{
    return m_size == other.m_size
           && m_attributes == other.m_attributes
           && m_sorted == other.m_sorted;
}
