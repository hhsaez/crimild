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

#ifndef CRIMILD_FOUNDATION_STRING_UTILS_
#define CRIMILD_FOUNDATION_STRING_UTILS_

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>

namespace crimild {

	class StringUtils {
	public:
		template< typename T >
		static void toValue( std::string input, T &output )
		{
			std::stringstream str;
			str << input;
			str >> output;
		}

		template< typename T >
		static std::vector< T > split( std::string input, char delim )
		{
			std::vector< T > result;
			std::istringstream iss( input );
			while ( !iss.eof() ) {
				std::string str;
				getline( iss, str, delim );
				T value;
				toValue< T >( str, value );
				result.push_back( value );
			}
			return result;
		}

		static std::string replaceAll( std::string str, std::string from, std::string to ) 
		{
		    if ( from.empty() ) {
		        return str;
		    }

		    size_t start_pos = 0;
		    while ( ( start_pos = str.find( from, start_pos ) ) != std::string::npos ) {
		        str.replace( start_pos, from.length(), to );
		        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		    }

		    return str;
		}

		static std::string getFileExtension( std::string path ) 
		{
			auto pos = path.find_last_of( "." );
			if ( pos == std::string::npos ) {
				return "";
			}

			return path.substr( pos + 1 );
		}

		static std::string splitLines( std::string input, int charsPerLine )
		{
			std::stringstream out;
            
            std::stringstream ss( input );
            std::string buffer;
            std::vector< std::string > lines;
            while ( std::getline( ss, buffer, '\n' ) ) {
                lines.push_back( buffer );
            }
            
            for ( auto line : lines ) {
                std::stringstream str;
                str << line;
                
                int charCount = 0;
                while ( !str.eof() ) {
                    std::string temp;
                    str >> temp;
                    charCount += temp.length() + 1;
                    if ( charCount >= charsPerLine ) {
                        out << "\n";
                        charCount = 0;
                    }
                    
                    out << temp << " ";
                }
                out << "\n";
            }

			return out.str();
		}
        
        /**
            \brief Read a string from a stream, including all white spaces
         */
        static std::string readFullString( std::istream &input )
        {
            std::string result;
            input >> result;
            
            if ( !input.eof() ) {
                // handle spaces in string
                std::istreambuf_iterator< char > it( input ), end;
                std::string temp;
                std::copy( it, end, std::inserter( temp, temp.begin() ) );
                if ( temp != "\r" ) result += temp;
            }
            
            return result;
        }
        
        static std::string toLower( const std::string &input )
        {
            std::string result( input );
            std::transform( result.begin(), result.end(), result.begin(), ::tolower );
            return result;
        }
        
        template< typename ... Args >
        static std::string toString( Args &&... args )
        {
            std::stringstream ss;
            ( void ) std::initializer_list< int > {
                (
                 ss << args,
                 0
                 )...
            };
            
            return ss.str();
        }

		static bool startsWith( const std::string &str, const std::string &prefix )
		{
			return str.length() >= prefix.length() && str.compare( 0, prefix.length(), prefix ) == 0;
		}

	};

}

#endif

