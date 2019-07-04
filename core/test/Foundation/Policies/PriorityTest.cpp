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

#include "Foundation/Types.hpp"
#include "Foundation/Version.hpp"

#include "gtest/gtest.h"

namespace crimild {

	namespace policies {

		struct Priority {
			crimild::Real32 value;
		};

		/*
		class Prioritable {
		public:
			virtual Priority getPriority
		};
		*/

	}

}

using namespace crimild;

TEST( Priority, after )
{
#if 0
	auto p0 = Priority();
	auto p1 = Priority::after( p0 );

	class Foo : public Prioritable {
	public:
		Priority getPriority( void ) const noexcept { return Priority(); }
	}:

	class Bar : public Prioritable {
	public:
		Priority getPriority( void ) const noexcept { return Priority::after< Foo >(); }
	};

	Foo f;
	Bar b( Priority::after( f->getPriority() ) );

	EXCEPT_GE( Bar::getPriority(), Foo::getPriority() );


	auto s0 = crimild::alloc< MockSystem >( "s0", Priority::DEFAULT );
	auto s1 = crimild::alloc< MockSystem >( "s1", Priority::after( s0 ) );
	auto s2 = crimild::alloc< MockSystem >( "s2", Priority::before( s1 ) );

	auto updates = std::vector< std::string > { "s0", "s1", "s2" };
	
#endif
}



