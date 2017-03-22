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

#include "Profiler.hpp"
#include "Log.hpp"

#include "Mathematics/Numeric.hpp"

#include <chrono>
#include <iostream>
#include <iomanip>

using namespace crimild;

void ProfilerConsoleOutputHandler::beginOutput( crimild::Size fps, crimild::Real64 avgFrameTime, crimild::Real64 minFrameTime, crimild::Real64 maxFrameTime )
{
    std::cout << std::setiosflags( std::ios::fixed )
              << std::setprecision( 3 )
              << std::setw( 10 ) << std::right << "MIN | "
              << std::setw( 10 ) << std::right << "AVG | "
              << std::setw( 10 ) << std::right << "MAX | "
              << std::setw( 10 ) << std::right << "TIME | "
              << std::setw( 10 ) << std::right << "COUNT | "
              << std::left << "NAME"
              << "\n--------------------------------------------------------------------------------------------"
              << std::endl;
}

void ProfilerConsoleOutputHandler::sample( float minPc, float avgPc, float maxPc, unsigned int totalTime, unsigned int callCount, std::string name, unsigned int parentCount )
{
	std::stringstream spaces;
	for ( int i = 0; i < parentCount; i++ ) {
		spaces << " ";
	}
	
	std::cout << std::setiosflags( std::ios::fixed | std::ios::showpoint )
			  << std::setprecision( 3 )
			  << std::setw( 7 ) << std::right << minPc << " | "
			  << std::setw( 7 ) << std::right << avgPc << " | "
			  << std::setw( 7 ) << std::right << maxPc << " | "
			  << std::setw( 7 ) << std::right << totalTime << " | "
			  << std::setw( 7 ) << std::right << callCount << " | "
			  << std::left << spaces.str() << name
			  << "\n";
}

void ProfilerConsoleOutputHandler::endOutput( void )
{
    std::cout << "\n" << std::endl;
}

ProfilerSample::ProfilerSample( std::string name )
{
    assert( Profiler::getInstance() != nullptr && "Profiler not initialized" );
    _sampleIndex = Profiler::getInstance()->onSampleCreated( name );
}

ProfilerSample::~ProfilerSample( void )
{
    Profiler::getInstance()->onSampleDestroyed( _sampleIndex );
}

Profiler::Profiler( void )
{
	resetAll();
}

Profiler::~Profiler( void )
{

}

crimild::Real64 Profiler::getTime( void )
{
	auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
    return 0.001 * std::chrono::duration_cast< std::chrono::microseconds >( now ).count();
}

int Profiler::onSampleCreated( std::string name )
{
    int firstInvalidIndex = -1;
    for ( int i = 0; i < Profiler::MAX_SAMPLES; i++ ) {
        auto &sample = _samples[ i ];

        if ( !sample.isValid ) {
            if ( firstInvalidIndex < 0 ) {
                firstInvalidIndex = i;
            }
        }
        else if ( sample.name == name ) {
            assert( !sample.isOpened && "Tried to profile a sample that is already being profiled" );

            sample.parentIndex = _lastOpenedSample;
            _lastOpenedSample = i;

            sample.parentCount = _openSampleCount;

            _openSampleCount++;

            sample.isOpened = true;

            sample.callCount++;
            sample.startTime = getTime();

            if ( sample.parentIndex < 0 ) {
                _rootBegin = sample.startTime;
            }

            return i;
        }
    }

    assert( firstInvalidIndex >= 0 && "Profiler run out of sample slots" );

    auto &sample = _samples[ firstInvalidIndex ];

    sample.isValid = true;
    sample.name = name;
    sample.parentIndex = _lastOpenedSample;
    _lastOpenedSample = firstInvalidIndex;
    sample.parentCount = _openSampleCount;
    _openSampleCount++;
    sample.isOpened = true;
    sample.callCount = 1;
    sample.totalTime = 0.0f;
    sample.childTime = 0.0f;
    sample.startTime = getTime();

    if ( sample.parentIndex < 0 ) {
        _rootBegin = sample.startTime;
    }

    return firstInvalidIndex;
}

void Profiler::onSampleDestroyed( int sampleIndex )
{
    if ( sampleIndex < 0 ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid sample index ", sampleIndex );
        return;
    }


    auto endTime = getTime();

    auto &sample = _samples[ sampleIndex ];

    sample.isOpened = false;

    unsigned long deltaTime = endTime - sample.startTime;

    if ( sample.parentIndex >= 0 ) {
        _samples[ sample.parentIndex ].childTime += deltaTime;
    }
    else {
        _rootEnd = endTime;
    }

    sample.totalTime += deltaTime;

    _lastOpenedSample = sample.parentIndex;

    _openSampleCount--;
}

void Profiler::resetAll( void )
{
	for ( int i = 0; i < MAX_SAMPLES; i++ ) {
		auto &sample = _samples[ i ];
		sample.avgTime = 0.0f;
		sample.minTime = -1.0f;
		sample.maxTime = -1.0f;
		sample.dataCount = 0;
	}

	if ( _frameCount > 0 ) {
		_fps = _frameCount;
		_avgFrameTime = _totalFrameTime / _fps;
	}

	const auto t = _minFrameTime;
	_minFrameTime = _maxFrameTime;
	_maxFrameTime = t;
	_frameCount = 0;
	_totalFrameTime = 0;
	_lastFrameTime = getTime();
}

void Profiler::step( void )
{
	_frameCount++;
	
	const auto currentFrameTime = getTime();
	const auto frameTime = currentFrameTime - _lastFrameTime;
	_minFrameTime = Numeric< crimild::Real64 >::min( frameTime, _minFrameTime );
	_maxFrameTime = Numeric< crimild::Real64 >::max( frameTime, _maxFrameTime );

	_lastFrameTime = currentFrameTime;
	_totalFrameTime += frameTime;
}

void Profiler::dump( void )
{
    if ( getOutputHandler() == nullptr ) {
        return;
    }

    getOutputHandler()->beginOutput( _fps, _avgFrameTime, _minFrameTime, _maxFrameTime );

	for ( int i = 0; i < MAX_SAMPLES; i++ ) {
		auto &sample = _samples[ i ];
		
		if ( sample.isValid ) {
			float accumTime = sample.avgTime * sample.dataCount + sample.totalTime;
			sample.dataCount++;
			sample.avgTime = accumTime / ( float ) sample.dataCount;
			sample.minTime = sample.minTime < 0 ? sample.totalTime : Numericf::min( sample.minTime, sample.totalTime );
			sample.maxTime = sample.maxTime < 0 ? sample.totalTime : Numericf::max( sample.maxTime, sample.totalTime );
			
			getOutputHandler()->sample( sample.minTime, sample.avgTime, sample.maxTime, sample.totalTime, sample.callCount, sample.name, sample.parentCount );
			
			sample.callCount = 0;
			sample.totalTime = 0;
			sample.childTime = 0;
			
		}
	}

    getOutputHandler()->endOutput();
}

