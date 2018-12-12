/*
 * Copyright (c) 2002-present, H. Hernan Saez
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

#ifndef CRIMILD_CORE_VERSION_
#define CRIMILD_CORE_VERSION_

#include <string>
#include <sstream>

#ifndef CRIMILD_VERSION_MAJOR
#define CRIMILD_VERSION_MAJOR 4
#endif

#ifndef CRIMILD_VERSION_MINOR
#define CRIMILD_VERSION_MINOR 10
#endif

#ifndef CRIMILD_VERSION_PATCH
#define CRIMILD_VERSION_PATCH 0
#endif

namespace crimild {

	class Version {
	public:
		Version( void );
		Version( const Version &other );
		explicit Version( int major, int minor, int patch );
		explicit Version( std::string versionStr );
		~Version( void );

		Version &operator=( const Version &other );

		inline int getMajor( void ) const { return _major; }
		inline int getMinor( void ) const { return _minor; }
		inline int getPatch( void ) const { return _patch; }

		bool operator<( const Version &other ) const { return toInt() < other.toInt(); }
		bool operator<=( const Version &other ) const { return toInt() <= other.toInt(); }
		bool operator>( const Version &other ) const { return toInt() > other.toInt(); }
		bool operator>=( const Version &other ) const { return toInt() >= other.toInt(); }
		bool operator==( const Version &other ) const { return toInt() == other.toInt(); }
		bool operator!=( const Version &other ) const { return toInt() != other.toInt(); }

	private:
		int _major;
		int _minor;
		int _patch;

	public:
		std::string getDescription( void ) const;

		void fromString( std::string str );

		inline int toInt( void ) const { return _major * 1000000 + _minor * 1000 + _patch; }
	};

}

#endif

