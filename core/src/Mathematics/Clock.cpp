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

const double Clock::DEFAULT_TICK_TIME = 1.0 / 60.0;
double Clock::_globalTimeScale = 1.0;

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
	_timeScale = t._timeScale;
	_ignoreGlobalTimeScale = t._ignoreGlobalTimeScale;
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
	_timeScale = t._timeScale;
	_ignoreGlobalTimeScale = t._ignoreGlobalTimeScale;

	return *this;
}

void Clock::reset( void )
{
	auto now = std::chrono::high_resolution_clock::now().time_since_epoch();

    _currentTime = 0.001 * std::chrono::duration_cast< std::chrono::milliseconds >( now ).count();
	_lastTime = _currentTime;
    _deltaTime = 0;
    _accumTime = 0;
	_timeScale = 1.0;
	_ignoreGlobalTimeScale = false;
}

void Clock::tick( void )
{
	auto now = std::chrono::high_resolution_clock::now().time_since_epoch();

    _currentTime = 0.001 * std::chrono::duration_cast< std::chrono::milliseconds >( now ).count();
    auto dt = _currentTime - _lastTime;
    _lastTime = _currentTime;

	onTick( dt );
}

void Clock::setTimeout( Clock::TimeoutCallback const &callback, double timeout, bool repeat )
{
	_timeoutCallback = callback;
	_timeout = timeout;
	_repeat = repeat;
}

Clock &Clock::operator+=( double dt )
{
	onTick( dt );
	return *this;
}

Clock &Clock::operator+=( const Clock &other )
{
	onTick( other.getDeltaTime() );
	return *this;
}

void Clock::onTick( double dt )
{
	_deltaTime = dt * getTimeScale();
	if ( !shouldIgnoreGlobalTimeScale() ) {
		_deltaTime *= getGlobalTimeScale();
	}

	_accumTime += _deltaTime;
	
	if ( _timeoutCallback != nullptr ) {
		_timeout -= _deltaTime;
		if ( _timeout <= 0.0 ) {
			_timeoutCallback();
			if ( !_repeat ) {
				_timeoutCallback = nullptr;
			}
		}
	}	
}
