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

#include "NamedObject.hpp"
#include "SharedObject.hpp"
#include "StringUtils.hpp"

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

namespace crimild {
    
    class Log {
    private:
        Log( void ) { }
        ~Log( void ) { }
        
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
        
        static void setLevel( int level ) { _level = level; };
        static int getLevel( void ) { return _level; }
        
    private:
        static int _level;
        
    public:
        template< typename ... Args >
        static void fatal( std::string const &TAG, Args &&... args )
        {
            print( Level::LOG_LEVEL_FATAL, "F", TAG, std::forward< Args >( args )... );
        }
        
        template< typename ... Args >
        static void error( std::string const &TAG, Args &&... args )
        {
            print( Level::LOG_LEVEL_ERROR, "E", TAG, std::forward< Args >( args )... );
        }
        
        template< typename ... Args >
        static void warning( std::string const &TAG, Args &&... args )
        {
            print( Level::LOG_LEVEL_WARNING, "W", TAG, std::forward< Args >( args )... );
        }
        
        template< typename ... Args >
        static void info( std::string const &TAG, Args &&... args )
        {
            print( Level::LOG_LEVEL_INFO, "I", TAG, std::forward< Args >( args )... );
        }
        
        template< typename ... Args >
        static void debug( std::string const &TAG, Args &&... args )
        {
            print( Level::LOG_LEVEL_DEBUG, "D", TAG, std::forward< Args >( args )... );
        }
        
        template< typename ... Args >
        static void trace( std::string const &TAG, Args &&... args )
        {
            print( Level::LOG_LEVEL_TRACE, "T", TAG, std::forward< Args >( args )... );
        }
        
        template< typename ... Args >
        static void print( int level, std::string const &levelStr, std::string const &TAG, Args &&... args )
        {
            if ( getLevel() >= level && _outputHandler != nullptr ) {
                auto tp = std::chrono::system_clock::now();
                auto s = std::chrono::duration_cast< std::chrono::microseconds >( tp.time_since_epoch() );
                auto t = ( time_t )( s.count() );
                
                auto str = StringUtils::toString( t, " ",
						  //std::this_thread::get_id(), " ",
                    levelStr, "/", TAG, " - ",
                    std::forward< Args >( args )... );
                
                _outputHandler->printLine( str );
            }
        }
        
    public:
        class OutputHandler {
        public:
            virtual ~OutputHandler( void ) { }
            
            virtual void printLine( std::string const &line ) = 0;
        };
        
        class NullOutputHandler : public OutputHandler {
        public:
            NullOutputHandler( void ) { }
            virtual ~NullOutputHandler( void ) { }
            
            virtual void printLine( std::string const &line ) override
            {
                // do nothing
            }
        };
        
        class ConsoleOutputHandler : public OutputHandler {
        public:
            ConsoleOutputHandler( void ) 
            { 

            }
            
            virtual ~ConsoleOutputHandler( void ) 
            {
                // force a flush when destroying
                std::cout << std::endl;
            }
            
            virtual void printLine( std::string const &line ) override
            {
                std::lock_guard< std::mutex > lock( _mutex );
                
                std::cout << line << "\n";
            }
                
            private:
                std::mutex _mutex;
        };
                
        class FileOutputHandler : public OutputHandler {
        public:
            FileOutputHandler( std::string const &path ) : _out( path, std::ios::out ) { }
            virtual ~FileOutputHandler( void ) { }
            
            virtual void printLine( std::string const &line ) override
            {
                std::lock_guard< std::mutex > locK( _mutex );
                
                _out << line << "\n";
            }
            
        private:
            std::ofstream _out;
            std::mutex _mutex;
        };
                
        template< class T, typename ... Args >
        static void setOutputHandler( Args &&... args )
        {
            _outputHandler = std::move( std::unique_ptr< T >( new T( std::forward< Args >( args )... ) ) );
        }
        
    private:
        static std::unique_ptr< OutputHandler > _outputHandler;
                
    };

}

#endif

