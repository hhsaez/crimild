/*
 * Copyright (c) 2013, Hernan Saez
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Foundation/Stream.hpp"
#include "Foundation/Memory.hpp"

#include "gtest/gtest.h"

namespace crimild {

	namespace test {

		template< typename T >
		class MockStreamObject : public StreamObject {
		public:
			MockStreamObject( void ) { }
			MockStreamObject( T value ) : _value( value ) { }
			virtual ~MockStreamObject( void ) { }

			T get( void ) const { return _value; }

		public:
			virtual bool registerInStream( Stream &s ) override
			{
				if ( !StreamObject::registerInStream( s ) ) {
					return false;
				}

				return true;
			}

			virtual void save( Stream &s ) override
			{
				StreamObject::save( s );
				s.write( _value );
			}

			virtual void load( Stream &s ) override
			{
				StreamObject::load( s );

				s.read( _value );
			}

		private:
			T _value;
		};

		class IntMockStreamObject : public MockStreamObject< int > {
			CRIMILD_IMPLEMENT_RTTI( crimild::test::IntMockStreamObject );

		public:
			IntMockStreamObject( void ) { }
			IntMockStreamObject( int value ) : MockStreamObject< int >( value ) { }
			virtual ~IntMockStreamObject( void ) { }
		};

		class Vector3fMockStreamObject : public MockStreamObject< Vector3f > {
			CRIMILD_IMPLEMENT_RTTI( crimild::test::Vector3fMockStreamObject );

		public:
			Vector3fMockStreamObject( void ) { }
			Vector3fMockStreamObject( Vector3f value ) : MockStreamObject< Vector3f >( value ) { }
			virtual ~Vector3fMockStreamObject( void ) { }
		};

		class Matrix4fMockStreamObject : public MockStreamObject< Matrix4f > {
			CRIMILD_IMPLEMENT_RTTI( crimild::test::Matrix4fMockStreamObject );

		public:
			Matrix4fMockStreamObject( void ) { }
			Matrix4fMockStreamObject( Matrix4f value ) : MockStreamObject< Matrix4f >( value ) { }
			virtual ~Matrix4fMockStreamObject( void ) { }
		};

		class CompositeMockStreamObject : public StreamObject {
			CRIMILD_IMPLEMENT_RTTI( crimild::test::CompositeMockStreamObject )

		public:
			CompositeMockStreamObject( SharedPointer< IntMockStreamObject > const &child )
				: _child( child )
			{

			}

			virtual ~CompositeMockStreamObject( void )
			{

			}

			IntMockStreamObject *getChild( void )
			{
				return crimild::get_ptr( _child );
			}

		public:
			CompositeMockStreamObject( void )
			{

			}

			virtual bool registerInStream( Stream &s ) override
			{
				if ( !StreamObject::registerInStream( s ) ) {
					return false;
				}

				if ( _child != nullptr ) {
					_child->registerInStream( s );
				}

				return true;
			}

			virtual void save( Stream &s ) override
			{
				StreamObject::save( s );

				s.writeChildObject( _child );
			}

			virtual void load( Stream &s ) override
			{
				StreamObject::load( s );

				auto self = this;
				s.readChildObject< IntMockStreamObject >( [self]( SharedPointer< IntMockStreamObject > const &child ) {
					self->_child = child;
				});
			}

		private:
			SharedPointer< IntMockStreamObject > _child;
		};

	}

}

CRIMILD_REGISTER_STREAM_OBJECT_BUILDER( crimild::test::IntMockStreamObject );
CRIMILD_REGISTER_STREAM_OBJECT_BUILDER( crimild::test::Vector3fMockStreamObject );
CRIMILD_REGISTER_STREAM_OBJECT_BUILDER( crimild::test::Matrix4fMockStreamObject );
CRIMILD_REGISTER_STREAM_OBJECT_BUILDER( crimild::test::CompositeMockStreamObject );

using namespace crimild;
using namespace crimild::test;

TEST( StreamingTest, saveFileStream )
{
	auto obj0 = crimild::alloc< IntMockStreamObject >( 5 );

	{
		FileStream os( "test.crimild", FileStream::OpenMode::WRITE );
		os.addObject( obj0 );
		EXPECT_TRUE( os.flush() );
	}

	{
		FileStream is( "test.crimild", FileStream::OpenMode::READ );
		EXPECT_TRUE( is.load() );
		EXPECT_EQ( 1, is.getObjectCount() );
		auto obj1 = is.getObjectAt< IntMockStreamObject >( 0 );
		EXPECT_TRUE( obj1 != nullptr );
		EXPECT_EQ( obj0->get(), obj1->get() );
	}
}

TEST( StreamingTest, streamVector3f )
{
	auto obj0 = crimild::alloc< Vector3fMockStreamObject >( Vector3f( 0.0f, 1.0f, 2.0f ) );

	{
		FileStream os( "test.crimild", FileStream::OpenMode::WRITE );
		os.addObject( obj0 );
		EXPECT_TRUE( os.flush() );
	}

	{
		FileStream is( "test.crimild", FileStream::OpenMode::READ );
		EXPECT_TRUE( is.load() );
		EXPECT_EQ( 1, is.getObjectCount() );
		auto obj1 = is.getObjectAt< Vector3fMockStreamObject >( 0 );
		EXPECT_TRUE( obj1 != nullptr );
		EXPECT_EQ( obj0->get(), obj1->get() );
	}
}

TEST( StreamingTest, streamMatrix4f )
{
	auto obj0 = crimild::alloc< Matrix4fMockStreamObject >( Matrix4f( 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 ) );

	{
		FileStream os( "test.crimild", FileStream::OpenMode::WRITE );
		os.addObject( obj0 );
		EXPECT_TRUE( os.flush() );
	}

	{
		FileStream is( "test.crimild", FileStream::OpenMode::READ );
		EXPECT_TRUE( is.load() );
		EXPECT_EQ( 1, is.getObjectCount() );
		auto obj1 = is.getObjectAt< Matrix4fMockStreamObject >( 0 );
		EXPECT_TRUE( obj1 != nullptr );
		EXPECT_EQ( obj0->get(), obj1->get() );
	}
}

TEST( StreamingTest, streamComposition )
{
	auto child = crimild::alloc< IntMockStreamObject >( 10 );
	auto parent = crimild::alloc< CompositeMockStreamObject >( child );

	{
		FileStream os( "streamComposition.crimild", FileStream::OpenMode::WRITE );
		os.addObject( parent );
		EXPECT_TRUE( os.flush() );
	}

	{
		FileStream is( "streamComposition.crimild", FileStream::OpenMode::READ );
		EXPECT_TRUE( is.load() );
		EXPECT_EQ( 1, is.getObjectCount() );
		
		auto p = is.getObjectAt< CompositeMockStreamObject >( 0 );
		EXPECT_TRUE( p != nullptr );

		EXPECT_TRUE( p->getChild() != nullptr );
		EXPECT_EQ( child->get(), p->getChild()->get() );
	}
}

TEST( StreamingTest, streamNullChild )
{
	auto parent = crimild::alloc< CompositeMockStreamObject >( nullptr );

	{
		FileStream os( "streamNullChild.crimild", FileStream::OpenMode::WRITE );
		os.addObject( parent );
		EXPECT_TRUE( os.flush() );
	}

	{
		FileStream is( "streamNullChild.crimild", FileStream::OpenMode::READ );
		EXPECT_TRUE( is.load() );
		EXPECT_EQ( 1, is.getObjectCount() );
		
		auto p = is.getObjectAt< CompositeMockStreamObject >( 0 );
		EXPECT_TRUE( p != nullptr );

		EXPECT_TRUE( p->getChild() == nullptr );
	}
}

