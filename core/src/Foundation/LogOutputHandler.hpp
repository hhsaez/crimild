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

#ifndef CRIMILD_FOUNDATION_LOG_OUTPUT_HANDLER
#define CRIMILD_FOUNDATION_LOG_OUTPUT_HANDLER

#include "Foundation/Log.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <mutex>

namespace crimild {

    class NullOutputHandler : public Log::OutputHandler {
    public:
        NullOutputHandler( int level ) noexcept
            : Log::OutputHandler( level )
        {
            // no-op
        }

        virtual ~NullOutputHandler( void ) = default;

        virtual void print( int, std::string const & ) noexcept override
        {
            // no-op
        }
    };

    class ConsoleOutputHandler : public Log::OutputHandler {
    public:
        ConsoleOutputHandler( int level ) noexcept
            : Log::OutputHandler( level )
        {
            // no-op
        }

        virtual ~ConsoleOutputHandler( void ) noexcept
        {
            // force a flush when destroying
            std::cout << std::endl;
        }

        virtual void print( int, std::string const &line ) noexcept override
        {
            std::lock_guard< std::mutex > lock( m_mutex );

            std::cout << line << "\n";
        }

    private:
        std::mutex m_mutex;
    };

    class FileOutputHandler : public Log::OutputHandler {
    public:
        FileOutputHandler( int level, std::string const &path ) noexcept
            : Log::OutputHandler( level ),
              m_out( path, std::ios::out )
        {
            // no-op
        }

        virtual ~FileOutputHandler( void ) = default;

        virtual void print( int, std::string const &line ) noexcept override
        {
            std::lock_guard< std::mutex > lock( m_mutex );

            m_out << line << "\n";
        }

    private:
        std::ofstream m_out;
        std::mutex m_mutex;
    };

}

#endif
