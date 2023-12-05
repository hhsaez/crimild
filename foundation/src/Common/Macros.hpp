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

#ifndef CRIMILD_MACROS_
#define CRIMILD_MACROS_

#include <string>

#if !defined( NDEBUG )
    #define CRIMILD_DEBUG 1
#endif

// Identify known platforms
#if defined( __EMSCRIPTEN__ )
    #define CRIMILD_PLATFORM_EMSCRIPTEN 1
#elif defined( __APPLE__ )
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
        #define CRIMILD_PLATFORM_IOS
    #else
        #define CRIMILD_PLATFORM_OSX
    #endif
#elif defined( __ANDROID__ )
    #define CRIMILD_PLATFORM_ANDROID
#elif defined( __CYGWIN__ ) || defined( __MINGW32__ ) || defined( _WIN32 ) || defined( __WIN32__ ) || defined( WIN32 )
    #define CRIMILD_PLATFORM_WIN32
#endif

#if defined( CRIMILD_PLATFORM_OSX ) || defined( CRIMILD_PLATFORM_WIN32 )
    #define CRIMILD_PLATFORM_DESKTOP
#endif

#if defined( CRIMILD_PLATFORM_IOS ) || defined( CRIMILD_PLATFORM_ANDROID )
    #define CRIMILD_PLATFORM_MOBILE
#endif

#ifdef __GNUC__
    #define CRIMILD_CURRENT_FUNCTION __PRETTY_FUNCTION__
#else
    #define CRIMILD_CURRENT_FUNCTION __FUNCTION__
#endif

namespace crimild {

    inline std::string getClassName( const std::string &funcName )
    {
        auto parentesis = funcName.find( "(" );
        if ( parentesis == std::string::npos ) {
            return "::";
        }

        auto colons = funcName.substr( 0, parentesis ).rfind( "::" );
        if ( colons == std::string::npos ) {
            return "::";
        }

        auto begin = funcName.substr( 0, colons ).rfind( " " ) + 1;
        auto end = colons - begin;

        return funcName.substr( begin, end );
    }

    inline std::string getFunctionName( const std::string &fqn )
    {
        auto end = fqn.find( "(" );
        if ( end == std::string::npos ) {
            return fqn;
        }

        auto begin = fqn.substr( 0, end ).rfind( "::" );
        if ( begin == std::string::npos ) {
            begin = fqn.substr( 0, end ).rfind( ' ' );
            if ( begin == std::string::npos ) {
                return fqn;
            }
            begin += 1;
        } else {
            begin += 2;
        }

        return fqn.substr( begin, end - begin );
    }
}

#define CRIMILD_CURRENT_FUNCTION_NAME ::crimild::getFunctionName( CRIMILD_CURRENT_FUNCTION )
#define CRIMILD_CURRENT_CLASS_NAME ::crimild::getClassName( CRIMILD_CURRENT_FUNCTION )

#define CRIMILD_TO_STRING( A ) #A

#define CRIMILD_CONCAT_IMPL( A, B ) A##B
#define CRIMILD_CONCAT( A, B ) CRIMILD_CONCAT_IMPL( A, B )

#define CRIMILD_RANDOM_VARIABLE_NAME( PREFIX ) \
    CRIMILD_CONCAT( PREFIX, __LINE__ )

#ifndef CRIMILD_USE_DEPTH_RANGE_ZERO_TO_ONE
    #define CRIMILD_USE_DEPTH_RANGE_ZERO_TO_ONE 1
#endif

#endif
