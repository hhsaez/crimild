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

#ifndef CRIMILD_FOUNDATION_LOG_
#define CRIMILD_FOUNDATION_LOG_

#include "Common/Macros.hpp"
#include "Common/StringUtils.hpp"

#include <chrono>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>

namespace crimild {

    class Log {
    public:
        class OutputHandler {
        protected:
            OutputHandler( int level ) noexcept
                : m_level( level )
            {
                // no-op
            }

        public:
            virtual ~OutputHandler( void ) = default;

            inline int getLevel( void ) const noexcept { return m_level; }

            template< typename... Args >
            void print( int level, const std::string &prefix, const std::string &tag, Args &&...args ) noexcept
            {
                if ( level <= getLevel() ) {
                    const auto tp = std::chrono::system_clock::now();
                    const auto s = std::chrono::duration_cast< std::chrono::microseconds >( tp.time_since_epoch() );
                    const auto t = ( time_t ) ( s.count() );
                    const auto line = StringUtils::toString(
                        t,
                        " ",
                        std::this_thread::get_id(), // requires including <thread>
                        " ",
                        prefix,
                        "/",
                        tag,
                        " - ",
                        std::forward< Args >( args )...
                    );
                    print( level, line );
                }
            }

        protected:
            virtual void print( int level, const std::string &line ) noexcept = 0;

        private:
            int m_level = Log::LOG_LEVEL_DEBUG;
        };

    private:
        Log( void ) = default;
        ~Log( void ) = default;

    public:
        enum Level {
            LOG_LEVEL_NONE = -1,
            LOG_LEVEL_FATAL = 100,
            LOG_LEVEL_ERROR = 200,
            LOG_LEVEL_WARNING = 300,
            LOG_LEVEL_INFO = 400,
            LOG_LEVEL_DEBUG = 500,
            LOG_LEVEL_TRACE = 600,
            LOG_LEVEL_ALL = 9999
        };

    public:
        static void setOutputHandlers( const std::vector< std::shared_ptr< OutputHandler > > &outputHandlers ) noexcept
        {
            m_outputHandlers = outputHandlers;
        }

    public:
        template< typename... Args >
        static void fatal( std::string const &TAG, Args &&...args )
        {
            print( Level::LOG_LEVEL_FATAL, "F", TAG, std::forward< Args >( args )... );
        }

        template< typename... Args >
        static void error( std::string const &TAG, Args &&...args )
        {
            print( Level::LOG_LEVEL_ERROR, "E", TAG, std::forward< Args >( args )... );
        }

        template< typename... Args >
        static void warning( std::string const &TAG, Args &&...args )
        {
            print( Level::LOG_LEVEL_WARNING, "W", TAG, std::forward< Args >( args )... );
        }

        template< typename... Args >
        static void info( std::string const &TAG, Args &&...args )
        {
            print( Level::LOG_LEVEL_INFO, "I", TAG, std::forward< Args >( args )... );
        }

        template< typename... Args >
        static void debug( std::string const &TAG, Args &&...args )
        {
            print( Level::LOG_LEVEL_DEBUG, "D", TAG, std::forward< Args >( args )... );
        }

        template< typename... Args >
        static void trace( std::string const &TAG, Args &&...args )
        {
            print( Level::LOG_LEVEL_TRACE, "T", TAG, std::forward< Args >( args )... );
        }

        template< typename... Args >
        static void print( int level, std::string const &levelStr, std::string const &TAG, Args &&...args )
        {
            for ( auto &handler : m_outputHandlers ) {
                handler->print( level, levelStr, TAG, std::forward< Args >( args )... );
            }
        }

    private:
        static std::vector< std::shared_ptr< OutputHandler > > m_outputHandlers;
    };

}

// TODO: Disable some of these macros in release build?
#define CRIMILD_LOG_FATAL( ... ) ( crimild::Log::fatal( CRIMILD_CURRENT_CLASS_NAME, __VA_ARGS__ ) )
#define CRIMILD_LOG_ERROR( ... ) ( crimild::Log::error( CRIMILD_CURRENT_CLASS_NAME, __VA_ARGS__ ) )
#define CRIMILD_LOG_WARNING( ... ) ( crimild::Log::warning( CRIMILD_CURRENT_CLASS_NAME, __VA_ARGS__ ) )
#define CRIMILD_LOG_INFO( ... ) ( crimild::Log::info( CRIMILD_CURRENT_CLASS_NAME, __VA_ARGS__ ) )
#define CRIMILD_LOG_DEBUG( ... ) ( crimild::Log::debug( CRIMILD_CURRENT_CLASS_NAME, __VA_ARGS__ ) )
#define CRIMILD_LOG_TRACE() ( crimild::Log::trace( CRIMILD_CURRENT_CLASS_NAME, CRIMILD_CURRENT_FUNCTION_NAME ) )

#endif
