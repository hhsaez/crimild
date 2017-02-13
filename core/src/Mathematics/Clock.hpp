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

#ifndef CRIMILD_MATHEMATICS_CLOCK_
#define CRIMILD_MATHEMATICS_CLOCK_

#include <functional>

namespace crimild {

	class Clock {
	public:
		static const double DEFAULT_TICK_TIME;

		static double getScaledTickTime( void ) { return getGlobalTimeScale() * DEFAULT_TICK_TIME; }

		static double getGlobalTimeScale( void ) { return _globalTimeScale; }
		static void setGlobalTimeScale( double value ) { _globalTimeScale = value; }

	private:
		static double _globalTimeScale;
		
	public:
		Clock( void );
		explicit Clock( double deltaTime );
		Clock( const Clock &other );
		~Clock( void );

		Clock &operator=( const Clock &other );

		void reset( void );
		void tick( void );

		double getCurrentTime( void ) const { return _currentTime; }
		void setCurrentTime( double value ) { _currentTime = value; }

		double getLastTime( void ) const { return _lastTime; }
		void setLastTime( double value ) { _lastTime = value; }

		double getDeltaTime( void ) const { return _deltaTime; }
		void setDeltaTime( double value ) { _deltaTime = value; }
        
        double getAccumTime( void ) const { return _accumTime; }
        void setAccumTime( double value ) { _accumTime = value; }

	private:
		double _currentTime;
		double _lastTime;
		double _deltaTime;
        double _accumTime;

	public:
		double getTimeScale( void ) const { return _timeScale; }
		void setTimeScale( double value ) { _timeScale = value; }

	private:
		double _timeScale = 1.0;

	public:
		using TimeoutCallback = std::function< void( void ) >;
		void setTimeout( TimeoutCallback const &callback, double timeout, bool repeat = false );

	private:
		TimeoutCallback _timeoutCallback;
		double _timeout;
		bool _repeat;

	public:
		/**
		   \brief Ticks the clock by a fixed delta time

		   As a side effect, _accumTime gets incremented by the new
		   _deltaTime. Callbacks get executed if timeout is over
		 */
		Clock &operator+=( double delta );

		/**
		   \brief Ticks the clock by another clock's delta time

		   As a side effect, _accumTime gets incremented by the new
		   _deltaTime. Callbacks get executed if timeout is over
		 */
		Clock &operator+=( const Clock &other );

	private:
		void onTick( double dt );
	};

}

#endif

