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

#include "Console.hpp"

#include "Commands/EchoConsoleCommand.hpp"
#include "Commands/SetConsoleCommand.hpp"

#include "Mathematics/Numeric.hpp"

#include "Simulation/Input.hpp"
#include "Simulation/Simulation.hpp"

#include "Foundation/Version.hpp"

using namespace crimild;

Console::Console( void )
{
	registerCommand( crimild::alloc< SimpleConsoleCommand >( "help", [this]( Console *, ConsoleCommand::ConsoleCommandArgs const & ) {
		pushLine( "Available commands:" );
		for ( const auto &it : _commands ) {
			pushLine( "   " + it.first );
		}
	}));

	registerCommand( crimild::alloc< SimpleConsoleCommand >( "clear", [ this ]( Console *console, ConsoleCommand::ConsoleCommandArgs const & ) {
		_lines.clear();
	}));

	registerCommand( crimild::alloc< EchoConsoleCommand >() );
	registerCommand( crimild::alloc< SetConsoleCommand >() );

	pushLine( Simulation::getInstance()->getName() );

	Version version;
	pushLine( version.getDescription() );

	_commandBufferHistoryIt = _commandBufferHistory.begin();

	setEnabled( true );
}

Console::~Console( void )
{

}

void Console::open( std::string line )
{
    clear();
    _commandBuffer << line;
    setActive( true );
}

void Console::close( void )
{
    clear();
    setActive( false );
}

void Console::registerCommand( ConsoleCommandPtr const &cmd )
{
	_commands[ cmd->getName() ] = cmd;
}

bool Console::handleInput( crimild::Int32 key, crimild::Int32 mod )
{
	if ( !isEnabled() ) {
		return false;
	}

	if ( key == '`' ) {
		setActive( !isActive() );
		return true;
	}

	if ( !isActive() ) {
		return false;
	}

	if ( key == CRIMILD_INPUT_KEY_ENTER ) {
		evaluate();
		clear();
		_historyOffset = 0;
	}
	else if ( key == CRIMILD_INPUT_KEY_BACKSPACE ) {
		popChar();
	}
	else if ( key == CRIMILD_INPUT_KEY_UP ) {
		if ( mod & CRIMILD_INPUT_MOD_SHIFT ) {
			_historyOffset++;
		}
		else {
			if ( _commandBufferHistoryIt != _commandBufferHistory.end() ) {
				clear();
				_commandBuffer << *_commandBufferHistoryIt;
				_commandBufferHistoryIt++;
			}
		}
	}
	else if ( key == CRIMILD_INPUT_KEY_DOWN ) {
		if ( mod & CRIMILD_INPUT_MOD_SHIFT ) {
			if ( _historyOffset > 0 ) {
				_historyOffset--;
			}
		}
		else {
			if ( _commandBufferHistoryIt != _commandBufferHistory.end() ) {
				clear();
				_commandBuffer << *_commandBufferHistoryIt;
				_commandBufferHistoryIt--;
			}
			else {
				clear();
				_commandBufferHistoryIt = _commandBufferHistory.begin();
			}
		}
	}
	else if ( key >= 32 && key < 128 ) {
		pushChar( key );
	}

	return true;
}

void Console::pushChar( char c )
{
	_commandBuffer << c;
}

void Console::popChar( void )
{
	// TODO: there has to be an easy way to do this
	if ( _commandBuffer.str().length() > 0 ) {
		std::string currentStr = _commandBuffer.str();
		clear();
		_commandBuffer << currentStr.substr( 0, currentStr.length() - 1 );
	}
}

void Console::clear( void )
{
	_commandBuffer.str( "" );
}

void Console::evaluate( void )
{
	auto line = _commandBuffer.str();

	if ( line.empty() ) {
		return;
	}

	_commandBufferHistory.push_front( line );
	_commandBufferHistoryIt = _commandBufferHistory.begin();

	pushLine( "> " + line );

	std::stringstream ss;
	ss << line;

	std::string cmdName;
	ss >> cmdName;

	if ( _commands.find( cmdName ) == _commands.end() ) {
		pushLine( "Command not found: " + cmdName );
		return;
	}

	auto cmd = _commands[ cmdName ];
	if ( cmd == nullptr ) {
		pushLine( "Invalid command: " + cmdName );
		return;
	}

	ConsoleCommand::ConsoleCommandArgs args;
	while ( !ss.eof() ) {
		std::string arg;
		ss >> arg;
		args.push_back( arg );
	}

	cmd->execute( this, args );
}

std::string Console::getOutput( crimild::UInt8 maxLines ) const
{
	std::stringstream output;

	const auto totalLines = _lines.size();

	if ( totalLines > 0 ) {
		const auto offset = Numerici::min( Numerici::max( 0, totalLines - maxLines ), _historyOffset );
		const auto start = Numerici::max( 0, totalLines - maxLines - offset );
		const auto end = Numerici::max( 0, totalLines - offset );

		for ( crimild::UInt8 i = start; i < end; i++ ) {
			output << _lines[ i ] << "\n";
		}
	}

	output << "$ " << _commandBuffer.str() << "_";

	return output.str();
}

