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

#ifndef CRIMILD_SIMULATION_CONSOLE_
#define CRIMILD_SIMULATION_CONSOLE_

#include "ConsoleCommand.hpp"

#include "Foundation/Types.hpp"
#include "Foundation/Singleton.hpp"

#include <map>
#include <list>
#include <sstream>

namespace crimild {

	/**
		\brief Implements an interactive command console
	*/
	class Console : public DynamicSingleton< Console > {
	public:
		Console( void );
		virtual ~Console( void );

	public:
		inline bool isEnabled( void ) const { return _enabled; }
		inline void setEnabled( bool enabled ) { _enabled = enabled; }

		inline bool isActive( void ) const { return _active; }
		inline void setActive( bool active ) { _active = active; }

	private:
		bool _enabled = false;
		bool _active = false;

	public:
		void registerCommand( ConsoleCommandPtr const &cmd );

	private:
		std::map< std::string, ConsoleCommandPtr > _commands;

	public:
		inline void pushLine( std::string const &line )
		{
			_lines.push_back( line );
		}

	private:
		std::vector< std::string > _lines;

	public:
		bool handleInput( crimild::Int32 key, crimild::Int32 mod );

	private:
		void pushChar( char c );
		void popChar( void );
		void clear( void );

		void evaluate( void );

	private:
		std::stringstream _commandBuffer;
		std::list< std::string > _commandBufferHistory;
		std::list< std::string >::iterator _commandBufferHistoryIt;

	public:
		std::string getOutput( crimild::UInt8 lines = 10 ) const;

	private:
		crimild::UInt32 _historyOffset = 0;
	};

};

#endif

