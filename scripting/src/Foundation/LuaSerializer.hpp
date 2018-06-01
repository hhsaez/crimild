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

#ifndef CRIMILD_SCRIPTING_FOUNDATION_LUA_SERIALIZER_
#define CRIMILD_SCRIPTING_FOUNDATION_LUA_SERIALIZER_

#include <Foundation/NonCopyable.hpp>
#include <Mathematics/Vector.hpp>
#include <Mathematics/Quaternion.hpp>

#include <sstream>

namespace crimild {

	namespace scripting {

		class LuaSerializer : public NonCopyable {
		public:
			LuaSerializer( std::ostream &output );
			virtual ~LuaSerializer( void );

			void pushObject( std::string name = "" );
			void popObject( void );

			template< typename T >
			void pushProperty( std::string name, T value ) 
			{
				std::stringstream str;
				str << name << " = " << value << ", ";
				pushLine( str.str() );
			}

			void pushProperty( std::string name, const char *value );
			void pushProperty( std::string name, std::string value );
			void pushProperty( std::string name, bool value );
			void pushProperty( std::string name, const Vector3f &v );
			void pushProperty( std::string name, const Quaternion4f &q );

			void pushText( std::string text );
			void pushLine( std::string line );

		private:
			int _depth;
			std::ostream &_output;
		};

	}

}

#endif

