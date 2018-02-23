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

#include "Coding/Codable.hpp"
#include "Coding/Encoder.hpp"
#include "Coding/Decoder.hpp"
#include "Coding/MemoryEncoder.hpp"
#include "Coding/MemoryDecoder.hpp"
#include "SceneGraph/Node.hpp"
#include "Foundation/ObjectFactory.hpp"

#include "gtest/gtest.h"

namespace crimild {
    
    class CodableNode : public Node {
        CRIMILD_IMPLEMENT_RTTI( crimild::CodableNode )
        
    public:
		explicit CodableNode( std::string name = "" ) : Node( name ) { }
        virtual ~CodableNode( void ) { }

		containers::Array< int > &getValues( void ) { return _values; }
		containers::Array< SharedPointer< CodableNode > > &getChildren( void ) { return _children; }
        
    private:
		containers::Array< int > _values;
        containers::Array< SharedPointer< CodableNode >> _children;
        
    public:
        virtual void encode( coding::Encoder &encoder ) override
        {
            Node::encode( encoder );
            
            encoder.encode( "values", _values );
            encoder.encode( "children", _children );
        }
        
        virtual void decode( coding::Decoder &decoder ) override
        {
            Node::decode( decoder );
            
            decoder.decode( "values", _values );
            decoder.decode( "children", _children );
        }
        
    };
    
}

using namespace crimild;

TEST( CodableTest, codingEncoding )
{
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::CodableNode )

    auto n = crimild::alloc< crimild::CodableNode >( "a scene" );
	n->getValues() = { 1, 2, 3, 4, 5 };
    n->local().setTranslate( 10, 20, 30 );
    n->world().setTranslate( 50, 70, 90 );
    n->setWorldIsCurrent( true );
    n->getChildren().add( crimild::alloc< crimild::CodableNode >( "child 1" ) );
    n->getChildren().add( crimild::alloc< crimild::CodableNode >( "child 2" ) );
    n->getChildren().add( crimild::alloc< crimild::CodableNode >( "child 3" ) );
    
	auto encoder = crimild::alloc< crimild::coding::MemoryEncoder >();
	encoder->encode( n );
    
	auto bytes = encoder->getBytes();
    
	auto decoder = crimild::alloc< crimild::coding::MemoryDecoder >();
	decoder->fromBytes( bytes );
	auto n2 = decoder->getObjectAt< crimild::CodableNode >( 0 );

	EXPECT_TRUE( n2 != nullptr );

	EXPECT_EQ( n->getName(), n2->getName() );
	EXPECT_EQ( n->getValues(), n2->getValues() );
	EXPECT_EQ( n->getLocal().getTranslate(), n2->getLocal().getTranslate() );
	EXPECT_EQ( n->getWorld().getTranslate(), n2->getWorld().getTranslate() );
	EXPECT_EQ( n->worldIsCurrent(), n2->worldIsCurrent() );

	EXPECT_EQ( n->getChildren().size(), n2->getChildren().size() );
	EXPECT_EQ( n->getChildren()[ 0 ]->getName(), n2->getChildren()[ 0 ]->getName() );
	EXPECT_EQ( n->getChildren()[ 1 ]->getName(), n2->getChildren()[ 1 ]->getName() );
	EXPECT_EQ( n->getChildren()[ 2 ]->getName(), n2->getChildren()[ 2 ]->getName() );
}

