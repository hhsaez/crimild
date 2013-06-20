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

#include <string>
#include <iostream>
#include <sstream>

namespace crimild {

	class Log : public NamedObject {
	public:
		class LogOutputHandler {
		public:
			virtual ~LogOutputHandler( void );

			virtual void write( Log *log, std::string message ) = 0;
		};

		typedef std::shared_ptr< LogOutputHandler > LogOutputHandlerPtr;

		class ConsoleOutputHandler : public LogOutputHandler {
		public:
			virtual ~ConsoleOutputHandler( void );
			virtual void write( Log *log, std::string message ) override;
		};

	public:
		static Log Debug;
		static Log Warning;
		static Log Error;
		static Log Fatal;
		static Log Info;

		class EndLine {
		public:
		};

		static EndLine End;

		static void setDefaultOutputHandler( LogOutputHandlerPtr handler );

	public:
		Log( std::string name );
		virtual ~Log( void );

		void setOutputHandler( LogOutputHandlerPtr handler ) { _outputHandler = handler; }
		LogOutputHandler *getOutputHandler( void ) { return _outputHandler.get(); }

		template< typename T >
		Log &operator<<( T in )
		{
			_str << in;
			return *this;
		}

		Log &operator<<( EndLine & )
		{
			_outputHandler->write( this, _str.str() );
			_str.str( "" );
			return *this;
		}

	private:
		std::stringstream _str;
		LogOutputHandlerPtr _outputHandler;
	};

}

#endif

