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

#include "Log.hpp"

using namespace crimild;

Log Log::Debug( "DEBUG" );
Log Log::Warning( "WARNING" );
Log Log::Error( "ERROR" );
Log Log::Fatal( "FATAL" );
Log Log::Info( "INFO" );
Log::EndLine Log::End;

Log::LogOutputHandler::~LogOutputHandler( void )
{

}

Log::ConsoleOutputHandler::~ConsoleOutputHandler( void )
{

}

void Log::ConsoleOutputHandler::write( Log *log, std::string message )
{
	std::cout << log->getName() << " - " << message << std::endl;
}

void Log::setDefaultOutputHandler( LogOutputHandlerPtr handler )
{
	Debug.setOutputHandler( handler );
	Error.setOutputHandler( handler );
	Fatal.setOutputHandler( handler );
	Info.setOutputHandler( handler );
}

Log::Log( std::string name )
	: NamedObject( name )
{
	LogOutputHandlerPtr handler( new ConsoleOutputHandler() );
	setOutputHandler( handler );
}

Log::~Log( void )
{

}

