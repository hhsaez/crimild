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

void ProfilerConsoleOutputHandler::beginOutput( void )
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
    std::cout << std::setiosflags( std::ios::fixed | std::ios::showpoint )
              << std::setprecision( 3 )
              << std::setw( 7 ) << std::right << minPc << " | "
              << std::setw( 7 ) << std::right << avgPc << " | "
              << std::setw( 7 ) << std::right << maxPc << " | "
              << std::setw( 7 ) << std::right << totalTime << " | "
              << std::setw( 7 ) << std::right << callCount << " | "
              << std::left << name
              << std::endl;
}

void ProfilerConsoleOutputHandler::endOutput( void )
{
    std::cout << "\n" << std::endl;
}

ProfilerSample::ProfilerSample( std::string name )
{
    _sampleIndex = Profiler::getInstance()->onSampleCreated( name );
}

ProfilerSample::~ProfilerSample( void )
{
    Profiler::getInstance()->onSampleDestroyed( _sampleIndex );
}

Profiler::Profiler( void )
{
    setOutputHandler( crimild::alloc< ProfilerConsoleOutputHandler >() );
}

Profiler::~Profiler( void )
{

}

unsigned long Profiler::getTime( void )
{
    return std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::high_resolution_clock::now().time_since_epoch() ).count();
}

int Profiler::onSampleCreated( std::string name )
{
    std::thread::id this_id = std::this_thread::get_id();

    auto stack = _samples[ this_id ];
    if ( stack == nullptr ) {
        stack = crimild::alloc< ProfilerSampleStack >();
        _samples[ this_id ] = stack;
    }

    int firstInvalidIndex = -1;
    for ( int i = 0; i < Profiler::MAX_SAMPLES; i++ ) {
        auto &sample = stack->samples[ i ];

        if ( !sample.isValid ) {
            if ( firstInvalidIndex < 0 ) {
                firstInvalidIndex = i;
            }
        }
        else if ( sample.name == name ) {
            assert( !sample.isOpened && "Tried to profile a sample that is already being profiled" );

            sample.parentIndex = stack->lastOpenedSample;
            stack->lastOpenedSample = i;

            sample.parentCount = stack->openSampleCount;

            stack->openSampleCount++;

            sample.isOpened = true;

            sample.callCount++;
            sample.startTime = getTime();

            if ( sample.parentIndex < 0 ) {
                stack->rootBegin = sample.startTime;
            }

            return i;
        }
    }

    assert( firstInvalidIndex >= 0 && "Profiler run out of sample slots" );

    auto &sample = stack->samples[ firstInvalidIndex ];

    sample.isValid = true;
    sample.name = name;
    sample.parentIndex = stack->lastOpenedSample;
    stack->lastOpenedSample = firstInvalidIndex;
    sample.parentCount = stack->openSampleCount;
    stack->openSampleCount++;
    sample.isOpened = true;
    sample.callCount = 1;
    sample.totalTime = 0.0f;
    sample.childTime = 0.0f;
    sample.startTime = getTime();

    if ( sample.parentIndex < 0 ) {
        stack->rootBegin = sample.startTime;
    }

    return firstInvalidIndex;
}

void Profiler::onSampleDestroyed( int sampleIndex )
{
    if ( sampleIndex < 0 ) {
        Log::Error << "Invalid sample index " << sampleIndex << Log::End;
        return;
    }


    std::thread::id this_id = std::this_thread::get_id();
    auto stack = _samples[ this_id ];
    if ( stack == nullptr ) {
        Log::Error << "Cannot find stack for given thread id" << Log::End;
        return;
    }

    auto endTime = getTime();

    auto &sample = stack->samples[ sampleIndex ];

    sample.isOpened = false;

    unsigned long deltaTime = endTime - sample.startTime;

    if ( sample.parentIndex >= 0 ) {
        stack->samples[ sample.parentIndex ].childTime += deltaTime;
    }
    else {
        stack->rootEnd = endTime;
    }

    sample.totalTime += deltaTime;

    stack->lastOpenedSample = sample.parentIndex;

    stack->openSampleCount--;
}

void Profiler::resetAll( void )
{
    for ( auto it : _samples ) {
        for ( int i = 0; i < MAX_SAMPLES; i++ ) {
            auto &sample = it.second->samples[ i ];
            sample.avgTime = 0.0f;
            sample.minTime = -1.0f;
            sample.maxTime = -1.0f;
            sample.dataCount = 0;
        }
    }
}

void Profiler::dump( void )
{
    if ( getOutputHandler() == nullptr ) {
        return;
    }

    getOutputHandler()->beginOutput();

    for ( auto it : _samples ) {
        for ( int i = 0; i < MAX_SAMPLES; i++ ) {
            auto &sample = it.second->samples[ i ];

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
    }

    getOutputHandler()->endOutput();
}

