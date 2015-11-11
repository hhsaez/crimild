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

#include "Clock.hpp"

#include <chrono>

using namespace crimild;

Clock::Clock( void )
{
	reset();
}

Clock::Clock( double deltaTime )
{
    reset();
    
    _deltaTime = deltaTime;
}

Clock::Clock( const Clock &t )
{
	_currentTime = t._currentTime;
	_lastTime = t._lastTime;
	_deltaTime = t._deltaTime;
    _accumTime = t._accumTime;
}

Clock::~Clock( void )
{

}

Clock &Clock::operator=( const Clock &t )
{
	_currentTime = t._currentTime;
	_lastTime = t._lastTime;
	_deltaTime = t._deltaTime;
    _accumTime = t._accumTime;

	return *this;
}

void Clock::reset( double current )
{
	_lastTime = _currentTime;
	_deltaTime = 0.0;
    _accumTime = 0.0;
}

void Clock::tick( void )
{
    _currentTime = 0.001 * std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::system_clock::now().time_since_epoch() ).count();
    _deltaTime = _currentTime - _lastTime;
    _lastTime = _currentTime;
    _accumTime += _deltaTime;
}

