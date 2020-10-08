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

#include "Rendering/Compositions/Composition.hpp"

#include "Rendering/RenderPass.hpp"

using namespace crimild;
using namespace crimild::compositions;

Composition::Composition( const Composition &cmp1, const Composition &cmp2 ) noexcept
    : Composition( cmp1 )
{
    cmp2.m_objects.each(
        [ this ]( auto &obj ) {
            m_objects.add( obj );
        } );

    cmp2.m_attachments.each(
        [ this ]( auto key, auto att ) {
            m_attachments[ key ] = att;
        } );

    enableHDR( cmp1.isHDREnabled() || cmp2.isHDREnabled() );
}

Composition::Composition( const Composition &other ) noexcept
    : m_objects( other.m_objects ),
      m_attachments( other.m_attachments ),
      m_output( other.m_output ),
      m_useHDR( other.m_useHDR )
{
}

Composition::Composition( Composition &&other ) noexcept
    : m_objects( std::move( other.m_objects ) ),
      m_attachments( std::move( other.m_attachments ) ),
      m_output( other.m_output ),
      m_useHDR( other.m_useHDR )
{
    other.m_output = nullptr;
}

Composition &Composition::operator=( const Composition &other ) noexcept
{
    m_objects = other.m_objects;
    m_attachments = other.m_attachments;
    m_output = other.m_output;
    m_useHDR = other.m_useHDR;
    return *this;
}

Composition &Composition::operator=( Composition &&other ) noexcept
{
    m_objects = std::move( other.m_objects );
    m_attachments = std::move( other.m_attachments );
    m_output = other.m_output;
    m_useHDR = other.m_useHDR;
    other.m_output = nullptr;
    return *this;
}

Attachment *Composition::createAttachment( std::string name ) noexcept
{
    auto att = crimild::alloc< Attachment >();
    m_objects.add( att );
    if ( m_attachments.contains( name ) ) {
        // avoid name clashing
        std::stringstream ss;
        ss << name << "_" << m_attachments.size();
        name = ss.str();
    }
    m_attachments[ name ] = crimild::get_ptr( att );
    return crimild::get_ptr( att );
}
