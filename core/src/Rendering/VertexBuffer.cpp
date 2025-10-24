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

#include "Rendering/VertexBuffer.hpp"

#include "Crimild_Coding.hpp"

using namespace crimild;

VertexBuffer::VertexBuffer( const VertexLayout &vertexLayout, crimild::Size count ) noexcept
   : VertexBuffer(
        vertexLayout,
        [ & ] {
           return crimild::alloc< BufferView >(
              BufferView::Target::VERTEX,
              crimild::alloc< Buffer >( Array< crimild::Byte >( count * vertexLayout.getSize() ) ),
              0,
              vertexLayout.getSize()
           );
        }()
     )
{
   // no-op
}

VertexBuffer::VertexBuffer( const VertexLayout &vertexLayout, SharedPointer< BufferView > const &bufferView ) noexcept
   : m_vertexLayout( vertexLayout ),
     m_bufferView( bufferView )
{
   assert( bufferView->getTarget() == BufferView::Target::VERTEX && "Invalid buffer view" );

   vertexLayout.eachAttribute(
      [ & ]( const auto &attrib ) {
         auto accessor = crimild::alloc< BufferAccessor >(
            bufferView,
            attrib->getOffset(),
            utils::getFormatSize( attrib->getFormat() )
         );
         m_accessors[ attrib->getName() ] = accessor;
      }
   );
}

VertexBuffer::VertexBuffer( const VertexLayout &vertexLayout, BufferView *bufferView ) noexcept
   : VertexBuffer( vertexLayout, crimild::retain( bufferView ) )
{
   // no-op
}

void VertexBuffer::encode( coding::Encoder &encoder )
{
   Entity::encode( encoder );

   Array< std::shared_ptr< VertexAttribute > > attribs;
   m_vertexLayout.eachAttribute(
      [ & ]( auto attr ) {
         attribs.add( attr );
      }
   );

   encoder.encode( "vertexLayoutAttribs", attribs );

   encoder.encode( "bufferView", m_bufferView );
   // encoder.encode( "accessor", m_accessors );
}

void VertexBuffer::decode( coding::Decoder &decoder )
{
   Entity::decode( decoder );

   Array< std::shared_ptr< VertexAttribute > > attribs;
   decoder.decode( "vertexLayoutAttribs", attribs );
   m_vertexLayout = VertexLayout( attribs );

   decoder.decode( "bufferView", m_bufferView );

   // Re-creating accessors seems simpler than actually encoding them.
   m_vertexLayout.eachAttribute(
      [ & ]( const auto &attrib ) {
         auto accessor = crimild::alloc< BufferAccessor >(
            m_bufferView,
            attrib->getOffset(),
            utils::getFormatSize( attrib->getFormat() )
         );
         m_accessors[ attrib->getName() ] = accessor;
      }
   );
}
