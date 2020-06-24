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

#ifndef CRIMILD_RENDERING_VERTEX_LAYOUT_
#define CRIMILD_RENDERING_VERTEX_LAYOUT_

#include "Rendering/VertexAttribute.hpp"
#include "Foundation/Containers/Array.hpp"
#include "Foundation/Containers/Map.hpp"

namespace crimild {

    class VertexLayout {
	public:
		static const VertexLayout P3;
        static const VertexLayout P3_C3;
		static const VertexLayout P3_N3;
		static const VertexLayout P3_TC2;
		static const VertexLayout P3_N3_TC2;
		
	public:
		VertexLayout( void ) noexcept;
		VertexLayout( std::initializer_list< VertexAttribute > attribs ) noexcept;
		VertexLayout( const containers::Array< VertexAttribute > &attribs ) noexcept;
		~VertexLayout( void ) = default;

		crimild::Bool operator==( const VertexLayout &other ) const noexcept;

		crimild::UInt32 getSize( void ) const noexcept { return m_size; }

		inline crimild::UInt32 getAttributeOffset( VertexAttribute::Name name ) const noexcept
		{
			return m_attributes[ name ].offset;
		}
		
		inline Format getAttributeFormat( VertexAttribute::Name attrib ) const noexcept
		{
			return m_attributes[ attrib ].format;
		}

		inline crimild::Size getAttributeSize( VertexAttribute::Name attrib ) const noexcept
		{
			return utils::getFormatSize( m_attributes[ attrib ].format );
		}

        template< typename Fn >
        void eachAttribute( Fn fn ) const
        {
            m_sorted.each(
                [&]( const auto &name ) {
                    fn( m_attributes[ name ] );
                }
            );
        }

		/**
		   \brief Append a new attribute

		   Allows construction of vertex descriptors like:
		   auto vertex = VertexLayout()
		       .withAttribute< Vector3f >( "position" )
			   .withAttribute< Vector3f >( "normal" )
			   .withAttribute< Vector2f >( "texCoord" );
		 */
		template< typename AttributeType >
		VertexLayout &withAttribute( VertexAttribute::Name attrib ) noexcept
		{
			auto format = utils::getFormat< AttributeType >();
			m_attributes[ attrib ] = {
				.format = format,
				.offset = m_size,
			};

            m_sorted.add( attrib );
			
			// What if there is already another attrib with the same name?
			// This will increase the final vector size, which is wrong.
			// But recalculating the vector size every time might be too
			// expensive. 
			m_size += utils::getFormatSize( format );

			// Return this pointer so we can keep adding attribs
			return *this;
		}

	private:
		crimild::UInt32 m_size = 0;
		containers::Map< VertexAttribute::Name, VertexAttribute > m_attributes;
        containers::Array< VertexAttribute::Name > m_sorted;
	};


}

#endif
