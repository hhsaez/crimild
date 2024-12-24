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
    crimild::alloc< VertexAttribute >( VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() ),
};

const VertexLayout VertexLayout::P3_C3 = {
    crimild::alloc< VertexAttribute >( VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() ),
    crimild::alloc< VertexAttribute >( VertexAttribute::Name::COLOR, utils::getFormat< ColorRGB >() ),
};

const VertexLayout VertexLayout::P3_N3 = {
    crimild::alloc< VertexAttribute >( VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() ),
    crimild::alloc< VertexAttribute >( VertexAttribute::Name::NORMAL, utils::getFormat< Vector3f >() ),
};

const VertexLayout VertexLayout::P3_TC2 = {
    crimild::alloc< VertexAttribute >( VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() ),
    crimild::alloc< VertexAttribute >( VertexAttribute::Name::TEX_COORD, utils::getFormat< Vector2f >() ),
};

const VertexLayout VertexLayout::P3_N3_TC2 = {
    crimild::alloc< VertexAttribute >( VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() ),
    crimild::alloc< VertexAttribute >( VertexAttribute::Name::NORMAL, utils::getFormat< Vector3f >() ),
    crimild::alloc< VertexAttribute >( VertexAttribute::Name::TEX_COORD, utils::getFormat< Vector2f >() ),
};

VertexLayout::VertexLayout( std::initializer_list< std::shared_ptr< VertexAttribute > > attribs ) noexcept
{
    m_size = 0;
    for ( const auto &attrib : attribs ) {
        attrib->setOffset( m_size );
        m_attributes[ attrib->getName() ] = attrib;
        m_sorted.add( attrib->getName() );
        m_size += utils::getFormatSize( attrib->getFormat() );
    }
}

VertexLayout::VertexLayout( const Array< std::shared_ptr< VertexAttribute > > &attribs ) noexcept
{
    m_size = 0;
    attribs.each(
        [ & ]( const auto &attrib ) {
            attrib->setOffset( m_size );
            m_attributes[ attrib->getName() ] = attrib;
            m_sorted.add( attrib->getName() );
            m_size += utils::getFormatSize( attrib->getFormat() );
        }
    );
}

crimild::Bool VertexLayout::operator==( const VertexLayout &other ) const noexcept
{
    if ( m_size != other.m_size ) {
        return false;
    }

    if ( m_sorted.size() != other.m_sorted.size() ) {
        return false;
    }

    if ( m_attributes.size() != other.m_attributes.size() ) {
        return false;
    }

    bool success = true;
    m_attributes.eachValue(
        [ & ]( const auto &attrib ) {
            success = success || other.m_attributes.contains( attrib->getName() );
            success = success || ( *attrib == *other.m_attributes[ attrib->getName() ] );
        }
    );
    return success;
}
