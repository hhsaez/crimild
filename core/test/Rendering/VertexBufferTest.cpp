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
#include "Rendering/Format.hpp"
#include "Rendering/FrameGraph.hpp"
#include "Coding/MemoryEncoder.hpp"
#include "Coding/MemoryDecoder.hpp"

#include "gtest/gtest.h"

namespace crimild {

	namespace utils {

		template< typename T >
		Format getFormat( void ) noexcept { return Format::UNDEFINED; }

		template<> Format getFormat< crimild::Real32 >( void ) noexcept { return Format::R32_SFLOAT; }
		template<> Format getFormat< Vector2f >( void ) noexcept { return Format::R32G32_SFLOAT; }
		template<> Format getFormat< Vector3f >( void ) noexcept { return Format::R32G32B32_SFLOAT; }
		
		crimild::UInt32 getFormatSize( Format format ) noexcept
		{
			switch ( format ) {
				case Format::R32_SFLOAT:
					return 1 * sizeof( crimild::Real32 );
				case Format::R32G32_SFLOAT:
					return 2 * sizeof( crimild::Real32 );
				case Format::R32G32B32_SFLOAT:
					return 3 * sizeof( crimild::Real32 );
				default:
					return 0;
			};
		}

	}

	struct VertexAttribute {
		enum Name {
			POSITION,
			NORMAL,
			TANGENT,
			COLOR,
			TEX_COORD,
			TEX_COORD_1,
			TEX_COORD_2,
			TEX_COORD_3,
			BLEND_INDICES,
			BLEND_WEIGHT,
			USER_ATTRIBUTE = 100,
		};

		Name name;
		Format format;
		crimild::UInt32 offset = 0;

		crimild::Bool operator==( const VertexAttribute &other ) const noexcept
		{
			return name == other.name && format == other.format && offset == other.offset;
		}
	};

	class VertexLayout {
	public:
		static const VertexLayout P3;
		static const VertexLayout P3_N3;
		static const VertexLayout P3_TC2;
		static const VertexLayout P3_N3_TC2;
		
	public:
		VertexLayout( void ) noexcept
			: m_size( 0 )
		{
			
		}
		
		VertexLayout( std::initializer_list< VertexAttribute > attribs ) noexcept
			: m_size( 0 )
		{
			for ( const auto &attrib : attribs ) {
				m_attributes[ attrib.name ] = {
					.name = attrib.name,
					.format = attrib.format,
					.offset = m_size,
				};
				m_size += utils::getFormatSize( attrib.format );
			}
		}

		~VertexLayout( void ) = default;

		crimild::Bool operator==( const VertexLayout &other ) const noexcept
		{
			return m_size == other.m_size && m_attributes == other.m_attributes;
		}

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
	};

	class VertexBuffer2 : public Buffer {
	public:
		VertexBuffer2( const VertexLayout &vertexLayout, const containers::Array< crimild::Real32 > &data ) noexcept
			: m_vertexLayout( vertexLayout ),
			  m_data( data.size() * sizeof( crimild::Real32 ) )
		{
			memcpy( m_data.getData(), data.getData(), m_data.size() );
		}

		// TODO add support for sparse data
		/*
		template< typename T >
		VertexBuffer2( const VertexLayout &vertexLayout, const containers::Array< T > &data ) noexcept
			: m_vertexLayout( vertexLayout )
		{
			
		}
		*/

		VertexBuffer2( const VertexLayout &vertexLayout, crimild::Size count ) noexcept
			: m_vertexLayout( vertexLayout ),
			  // Compute data size based on the vertex layout and the count. Divide by sizeof(Real32)
			  // to get the actual number
			  m_data( count * vertexLayout.getSize() )
		{
			
		}

		virtual ~VertexBuffer2( void ) = default;

		inline const VertexLayout &getVertexLayout( void ) const noexcept { return m_vertexLayout; }

		inline Usage getUsage( void ) const noexcept override { return Buffer::Usage::VERTEX_BUFFER; }

        inline crimild::Size getSize( void ) const noexcept override { return m_data.size(); }
        inline crimild::Size getStride( void ) const noexcept override { return m_vertexLayout.getSize(); }

        inline void *getRawData( void ) noexcept override { return ( void * ) m_data.getData(); }
        inline const void *getRawData( void ) const noexcept override { return ( void * ) m_data.getData(); }

        inline crimild::Size getCount( void ) const noexcept { return getSize() / getStride(); }

		template< typename T >
		T get( VertexAttribute::Name attribute, crimild::Size index ) const noexcept
		{
			assert( utils::getFormat< T >() == m_vertexLayout.getAttributeFormat( attribute ) && "Invalid attribute format" );
			auto offset = m_vertexLayout.getAttributeOffset( attribute );
			auto stride = getStride();
			return *static_cast< const T * >( static_cast< const void * >( &m_data[ index * stride + offset ] ) );
		}

		template< typename T >
		void set( VertexAttribute::Name attribute, crimild::Size index, const T &value ) noexcept
		{
			assert( utils::getFormat< T >() == m_vertexLayout.getAttributeFormat( attribute ) && "Invalid attribute format" );
			auto offset = m_vertexLayout.getAttributeOffset( attribute );
			auto size = m_vertexLayout.getAttributeSize( attribute );
			auto stride = getStride();
			memcpy( &m_data[ index * stride + offset ], static_cast< const void * >( &value ), size );
		}

		void set( VertexAttribute::Name attribute, containers::Array< crimild::Real32 > const &data ) noexcept
		{
			auto offset = m_vertexLayout.getAttributeOffset( attribute );
			auto size = m_vertexLayout.getAttributeSize( attribute );
			auto S = getStride();
			auto N = getCount();

			assert( data.size() * sizeof( crimild::Real32 ) / size == N && "Invalid data size" );

			for ( auto i = 0l; i < N; i++ ) {
				memcpy( &m_data[ i * S + offset ], &data[ i * size / sizeof( crimild::Real32 ) ], size );
			}
		}

		template< typename AttributeType, typename Fn >
		void each( VertexAttribute::Name attribute, Fn fn ) const noexcept
		{
			assert( utils::getFormat< AttributeType >() == m_vertexLayout.getAttributeFormat( attribute ) && "Invalid attribute format" );
			auto offset = m_vertexLayout.getAttributeOffset( attribute );
			auto stride = getStride();
			auto N = getCount();

			for ( auto index = 0l; index < N; index++ ) {
				AttributeType val = *static_cast< const AttributeType * >( static_cast< const void * >( &m_data[ index * stride + offset ] ) );
				fn( val, index );
			}
		}

    private:
		VertexLayout m_vertexLayout;
        containers::Array< crimild::Byte > m_data;

	};

}

using namespace crimild;

const VertexLayout VertexLayout::P3 = {
	{ VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
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

TEST( VertexLayout, P3 )
{
	auto v = VertexLayout::P3;

	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getSize() );
	ASSERT_EQ( 0, v.getAttributeOffset( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::POSITION ) );
}

TEST( VertexLayout, positionsOnly )
{
	auto v = VertexLayout {
		{ VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
	};

	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getSize() );
	ASSERT_EQ( 0, v.getAttributeOffset( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::POSITION ) );
}

TEST( VertexLayout, positionsAndNormals )
{
	auto v = VertexLayout()
	    .withAttribute< Vector3f >( VertexAttribute::Name::POSITION )
	    .withAttribute< Vector3f >( VertexAttribute::Name::NORMAL );

	ASSERT_EQ( sizeof( crimild::Real32 ) * 6, v.getSize() );

	ASSERT_EQ( 0, v.getAttributeOffset( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::POSITION ) );
	
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeOffset( VertexAttribute::Name::NORMAL ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::NORMAL ) );
}

TEST( VertexLayout, multipleAttributes )
{
	auto v = VertexLayout {
		{ VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
		{ VertexAttribute::Name::NORMAL, utils::getFormat< Vector3f >() },
		{ VertexAttribute::Name::COLOR, utils::getFormat< RGBColorf >() },
		{ VertexAttribute::Name::TEX_COORD, utils::getFormat< Vector2f >() },
	};

	ASSERT_EQ( sizeof( crimild::Real32 ) * 11, v.getSize() );

	ASSERT_EQ( 0, v.getAttributeOffset( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeSize( VertexAttribute::Name::POSITION ) );
	
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeOffset( VertexAttribute::Name::NORMAL ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::NORMAL ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeSize( VertexAttribute::Name::NORMAL ) );

	ASSERT_EQ( sizeof( crimild::Real32 ) * 6, v.getAttributeOffset( VertexAttribute::Name::COLOR ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::COLOR ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeSize( VertexAttribute::Name::COLOR ) );

	ASSERT_EQ( sizeof( crimild::Real32 ) * 9, v.getAttributeOffset( VertexAttribute::Name::TEX_COORD ) );
	ASSERT_EQ( Format::R32G32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::TEX_COORD ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 2, v.getAttributeSize( VertexAttribute::Name::TEX_COORD ) );
}

TEST( VertexLayout, multipleAttributesDifferentOrder )
{
	auto v = VertexLayout {
		{ VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
		{ VertexAttribute::Name::TEX_COORD, utils::getFormat< Vector2f >() },
		{ VertexAttribute::Name::COLOR, utils::getFormat< RGBColorf >() },
		{ VertexAttribute::Name::NORMAL, utils::getFormat< Vector3f >() },
	};

	ASSERT_EQ( sizeof( crimild::Real32 ) * 11, v.getSize() );

	ASSERT_EQ( 0, v.getAttributeOffset( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeSize( VertexAttribute::Name::POSITION ) );
	
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeOffset( VertexAttribute::Name::TEX_COORD ) );
	ASSERT_EQ( Format::R32G32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::TEX_COORD ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 2, v.getAttributeSize( VertexAttribute::Name::TEX_COORD ) );

	ASSERT_EQ( sizeof( crimild::Real32 ) * 5, v.getAttributeOffset( VertexAttribute::Name::COLOR ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::COLOR ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeSize( VertexAttribute::Name::COLOR ) );

 	ASSERT_EQ( sizeof( crimild::Real32 ) * 8, v.getAttributeOffset( VertexAttribute::Name::NORMAL ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::NORMAL ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeSize( VertexAttribute::Name::NORMAL ) );
}

TEST( VertexLayout, withAttribute )
{
	auto v = VertexLayout()
	    .withAttribute< Vector3f >( VertexAttribute::Name::POSITION )
	    .withAttribute< Vector2f >( VertexAttribute::Name::TEX_COORD )
	    .withAttribute< RGBColorf >( VertexAttribute::Name::COLOR )
	    .withAttribute< Vector3f >( VertexAttribute::Name::NORMAL );

	ASSERT_EQ( sizeof( crimild::Real32 ) * 11, v.getSize() );

	ASSERT_EQ( 0, v.getAttributeOffset( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeSize( VertexAttribute::Name::POSITION ) );
    
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeOffset( VertexAttribute::Name::TEX_COORD ) );
    ASSERT_EQ( Format::R32G32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::TEX_COORD ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 2, v.getAttributeSize( VertexAttribute::Name::TEX_COORD ) );

	ASSERT_EQ( sizeof( crimild::Real32 ) * 5, v.getAttributeOffset( VertexAttribute::Name::COLOR ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::COLOR ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeSize( VertexAttribute::Name::COLOR ) );

 	ASSERT_EQ( sizeof( crimild::Real32 ) * 8, v.getAttributeOffset( VertexAttribute::Name::NORMAL ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::NORMAL ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeSize( VertexAttribute::Name::NORMAL ) );
}

TEST( VertexLayout, equality )
{
	auto v1 = VertexLayout {
		{ VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
		{ VertexAttribute::Name::NORMAL, utils::getFormat< Vector3f >() },
		{ VertexAttribute::Name::COLOR, utils::getFormat< RGBColorf >() },
	};

	auto v2 = VertexLayout {
		{ VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
		{ VertexAttribute::Name::NORMAL, utils::getFormat< Vector3f >() },
		{ VertexAttribute::Name::COLOR, utils::getFormat< RGBColorf >() },
	};

	ASSERT_EQ( v1, v2 );
}















TEST( VertexBuffer, construction )
{
	auto layout = VertexLayout {
		{ VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
	};

	auto vertices = crimild::alloc< VertexBuffer2 >(
		layout,
		containers::Array< crimild::Real32 > {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f, 0.5f, 0.0f,
		}
	);

	ASSERT_EQ( 3, vertices->getCount() );
	ASSERT_EQ( layout, vertices->getVertexLayout() );
	ASSERT_EQ( Vector3f( -0.5f, -0.5f, 0.0f ), vertices->get< Vector3f >( VertexAttribute::Name::POSITION, 0 ) );
}

TEST( VertexBuffer, setSingleValue )
{
	auto vertices = crimild::alloc< VertexBuffer2 >( VertexLayout::P3, 3 );

	ASSERT_EQ( 3, vertices->getCount() );

	vertices->set( VertexAttribute::Name::POSITION, 0, Vector3f( -0.5f, -0.5f, 0.0f ) );
	ASSERT_EQ( Vector3f( -0.5f, -0.5f, 0.0f ), vertices->get< Vector3f >( VertexAttribute::Name::POSITION, 0 ) );
}

TEST( VertexBuffer, setMultipleValues )
{
	auto vertices = crimild::alloc< VertexBuffer2 >( VertexLayout::P3, 3 );

	ASSERT_EQ( 3, vertices->getCount() );

	vertices->set(
		VertexAttribute::Name::POSITION,
		{
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f, 0.5f, 0.0f,
		}
	);
	
	ASSERT_EQ( Vector3f( -0.5f, -0.5f, 0.0f ), vertices->get< Vector3f >( VertexAttribute::Name::POSITION, 0 ) );
	ASSERT_EQ( Vector3f( 0.5f, -0.5f, 0.0f ), vertices->get< Vector3f >( VertexAttribute::Name::POSITION, 1 ) );
	ASSERT_EQ( Vector3f( 0.0f, 0.5f, 0.0f ), vertices->get< Vector3f >( VertexAttribute::Name::POSITION, 2 ) );
}

TEST( VertexBuffer, setPositionsInterleaved )
{
	auto vertices = crimild::alloc< VertexBuffer2 >( VertexLayout::P3_TC2, 3 );

	ASSERT_EQ( 3, vertices->getCount() );
	ASSERT_EQ( 3 * sizeof( crimild::Real32 ) * 5, vertices->getSize() );

	vertices->set(
		VertexAttribute::Name::POSITION,
		{
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f, 0.5f, 0.0f,
		}
	);
	
	ASSERT_EQ( Vector3f( -0.5f, -0.5f, 0.0f ), vertices->get< Vector3f >( VertexAttribute::Name::POSITION, 0 ) );
	ASSERT_EQ( Vector3f( 0.5f, -0.5f, 0.0f ), vertices->get< Vector3f >( VertexAttribute::Name::POSITION, 1 ) );
	ASSERT_EQ( Vector3f( 0.0f, 0.5f, 0.0f ), vertices->get< Vector3f >( VertexAttribute::Name::POSITION, 2 ) );
}

TEST( VertexBuffer, setTexCoordsInterleaved )
{
	auto vertices = crimild::alloc< VertexBuffer2 >( VertexLayout::P3_TC2, 3 );

	ASSERT_EQ( 3, vertices->getCount() );
	ASSERT_EQ( 3 * sizeof( crimild::Real32 ) * 5, vertices->getSize() );

	vertices->set(
		VertexAttribute::Name::TEX_COORD,
		{
			0.0, 0.0,
			0.0, 1.0,
			1.0, 1.0,
		}
	);
	
	ASSERT_EQ( Vector2f( 0.0f, 0.0f ), vertices->get< Vector2f >( VertexAttribute::Name::TEX_COORD, 0 ) );
	ASSERT_EQ( Vector2f( 0.f, 1.0f ), vertices->get< Vector2f >( VertexAttribute::Name::TEX_COORD, 1 ) );
	ASSERT_EQ( Vector2f( 1.0f, 1.0f ), vertices->get< Vector2f >( VertexAttribute::Name::TEX_COORD, 2 ) );
}

TEST( VertexBuffer, setInterleaved )
{
	auto vertices = crimild::alloc< VertexBuffer2 >( VertexLayout::P3_TC2, 3 );

	ASSERT_EQ( 3, vertices->getCount() );
	ASSERT_EQ( 3 * sizeof( crimild::Real32 ) * 5, vertices->getSize() );

	vertices->set(
		VertexAttribute::Name::POSITION,
		{
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f, 0.5f, 0.0f,
		}
	);
	
	vertices->set(
		VertexAttribute::Name::TEX_COORD,
		{
			0.0, 0.0,
			0.0, 1.0,
			1.0, 1.0,
		}
	);
	
	ASSERT_EQ( Vector3f( -0.5f, -0.5f, 0.0f ), vertices->get< Vector3f >( VertexAttribute::Name::POSITION, 0 ) );
	ASSERT_EQ( Vector3f( 0.5f, -0.5f, 0.0f ), vertices->get< Vector3f >( VertexAttribute::Name::POSITION, 1 ) );
	ASSERT_EQ( Vector3f( 0.0f, 0.5f, 0.0f ), vertices->get< Vector3f >( VertexAttribute::Name::POSITION, 2 ) );

	ASSERT_EQ( Vector2f( 0.0f, 0.0f ), vertices->get< Vector2f >( VertexAttribute::Name::TEX_COORD, 0 ) );
	ASSERT_EQ( Vector2f( 0.f, 1.0f ), vertices->get< Vector2f >( VertexAttribute::Name::TEX_COORD, 1 ) );
	ASSERT_EQ( Vector2f( 1.0f, 1.0f ), vertices->get< Vector2f >( VertexAttribute::Name::TEX_COORD, 2 ) );
}

TEST( VertexBuffer, eachPosition )
{
	auto vertices = crimild::alloc< VertexBuffer2 >( VertexLayout::P3_TC2, 3 );

	ASSERT_EQ( 3, vertices->getCount() );
	ASSERT_EQ( 3 * sizeof( crimild::Real32 ) * 5, vertices->getSize() );

	auto data = containers::Array< float > {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f,
	};

	vertices->set( VertexAttribute::Name::POSITION, data );
	
	vertices->each< Vector3f >(
		VertexAttribute::Name::POSITION,
		[&]( auto &v, auto i ) {
			ASSERT_EQ( data[ i * 3 + 0 ], v[ 0 ] );
			ASSERT_EQ( data[ i * 3 + 1 ], v[ 1 ] );
			ASSERT_EQ( data[ i * 3 + 2 ], v[ 2 ] );
		}
	);
}

TEST( VertexBuffer, eachTexCoord )
{
	auto vertices = crimild::alloc< VertexBuffer2 >( VertexLayout::P3_TC2, 3 );

	ASSERT_EQ( 3, vertices->getCount() );
	ASSERT_EQ( 3 * sizeof( crimild::Real32 ) * 5, vertices->getSize() );

	auto data = containers::Array< float > {
		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
	};

	vertices->set( VertexAttribute::Name::TEX_COORD, data );

	vertices->each< Vector2f >(
		VertexAttribute::Name::TEX_COORD,
		[&]( auto &v, auto i ) {
			ASSERT_EQ( data[ i * 2 + 0 ], v[ 0 ] );
			ASSERT_EQ( data[ i * 2 + 1 ], v[ 1 ] );
		}
	);
}

TEST( VertexBuffer, sparseData )
{
	/*
	struct VertexLayoutType {
		Vector3f position;
		Vector3f normal;
		Vector2i weights;
		Size index;
	};

	auto layout = VertexLayout {
		{ VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
		{ VertexAttribute::Name::NORMAL, utils::getFormat< Vector3f >() },
		{ VertexAttribute::Name::BLEND_INDICES, utils::getFormat< Vector2i >() },
		{ VertexAttribute::Name::INDEX, utils::getFormat< crimild::Size >() },
	};

	auto vertices = crimild::alloc< VertexBuffer2 >(
		layout,
		containers::Array< VertexType > {
			
		}
	);
	*/
}






TEST( VertexBuffer, autoAddToFrameGraph )
{
	auto graph = crimild::alloc< FrameGraph >();

	ASSERT_FALSE( graph->hasNodes() );

	{
		auto vertexBuffer = crimild::alloc< VertexP3C3TC2Buffer >( 0 );

		ASSERT_TRUE( graph->contains( vertexBuffer ) );
		ASSERT_TRUE( graph->hasNodes() );
	}

	ASSERT_FALSE( graph->hasNodes() );
	
}

TEST( VertexBuffer, construction_old )
{
	auto data = containers::Array< VertexP3 > {
		{
			.position = Vector3f( 1.0f, 2.0f, 3.0f ),
		}
	};

	auto vbo = crimild::alloc< VertexP3Buffer >( data );

	ASSERT_EQ( 1, vbo->getCount() );
	ASSERT_EQ( 3 * sizeof( float ), vbo->getSize() );
	ASSERT_EQ( 0, memcmp( data.getData(), vbo->getData(), sizeof( VertexP3 ) * vbo->getCount() ) );
}

TEST( VertexBuffer, extent )
{
	auto data = containers::Array< VertexP3 > {
		{
			.position = Vector3f( 1.0f, 2.0f, 3.0f ),
		},
		{
			.position = Vector3f( -10.0f, 0.0f, 5.0f ),
		},
	};

	auto vbo = crimild::alloc< VertexP3Buffer >( data );

	Vector3f min, max;
	ASSERT_TRUE( vbo->getExtent( min, max ) );
	ASSERT_EQ( Vector3f( -10.0f, 0.0f, 3.0f ), min );
	ASSERT_EQ( Vector3f( 1.0f, 2.0f, 5.0f ), max );
}

TEST( VertexBuffer, positions )
{
	/*
	auto data = containers::Array< VertexP3 > {
		{
			.position = Vector3f( 1.0f, 2.0f, 3.0f ),
		},
		{
			.position = Vector3f( -10.0f, 0.0f, 5.0f ),
		},
	};

	auto vbo = crimild::alloc< VertexP3Buffer >( data );

	ASSERT_EQ( data[ 0 ].position, vbo->getData()[ 0 ].position );
	ASSERT_EQ( data[ 1 ].position, vbo->getData()[ 1 ].position );
	*/

	/*
	auto vbo = crimild::alloc< VertexBuffer2 >(
		{
			{ Attribute::POSITION, Format::R32G32B32 },
		},
		3
	);

	auto p0 = Vector3f( 1.0f, 2.0f, 3.0f );
	vbo->set( Attribute::POSITION, 0, p0 );

	Vector3f p1;
	vbo->get( Attribute::POSITION, 0, p1 );

	vbo->set( Attribute::POSITION, { p0 } );

	ASSERT_EQ( p0, p1 );
	*/
}

TEST( VertexBuffer, coding )
{
	/*
	auto data = containers::Array< VertexP3 > {
		{
			.position = Vector3f( 1.0f, 2.0f, 3.0f ),
		},
		{
			.position = Vector3f( -10.0f, 0.0f, 5.0f ),
		},
	};

	auto vbo = crimild::alloc< VertexP3Buffer >( data );

    auto encoder = crimild::alloc< coding::MemoryEncoder >();
    encoder->encode( vbo );
    auto bytes = encoder->getBytes();

    auto decoder = crimild::alloc< coding::MemoryDecoder >();
    decoder->fromBytes( bytes );
    
    auto vbo1 = decoder->getObjectAt< VertexP3Buffer >( 0 );
    ASSERT_TRUE( vbo1 != nullptr );
	ASSERT_EQ( data.size(), vbo->getCount() );
	ASSERT_EQ( data.size() * sizeof( VertexP3 ), vbo->getSize() );
	ASSERT_EQ( 0, memcmp( data.getData(), vbo->getData(), sizeof( VertexP3 ) * vbo->getCount() ) );
	*/
}

