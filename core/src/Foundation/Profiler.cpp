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
#include "Mathematics/Clock.hpp"

#include "Debug/DebugRenderHelper.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/ShaderProgram.hpp"

#include "Concurrency/JobScheduler.hpp"

#include <chrono>
#include <iostream>
#include <iomanip>

using namespace crimild;
using namespace crimild::concurrency;

ProfilerOutputHandler::~ProfilerOutputHandler( void )
{

}

void ProfilerOutputHandler::beginOutput( crimild::Size fps, crimild::Real64 avgFrameTime, crimild::Real64 minFrameTime, crimild::Real64 maxFrameTime )
{
    _output.str( "" );

    // FPS
    _output << std::setiosflags( std::ios::fixed )
            << std::setprecision( 3 )
            << "FPS: " << std::setw( 10 ) << fps
            << "MIN: " << std::setw( 10 ) << minFrameTime
            << "AVG: " << std::setw( 10 ) << avgFrameTime
            << "MAX: " << std::setw( 10 ) << maxFrameTime
            << "\n----------------------------------------------------------------------------------------\n";

    // Rendering
    _output << std::setiosflags( std::ios::fixed )
            << std::setprecision( 3 )
            << std::left
            << "Rendering"
            << "\n   Textures:"
                << " Loaded: " << Renderer::getInstance()->getTextureCatalog()->getResourceCount()
                << " Active: " << Renderer::getInstance()->getTextureCatalog()->getActiveResourceCount()
            << "\n   FBOs:"
                << " Loaded: " << Renderer::getInstance()->getFrameBufferObjectCatalog()->getResourceCount()
                << " Active: " << Renderer::getInstance()->getFrameBufferObjectCatalog()->getActiveResourceCount()
            << "\n   VBOs:"
                << " Loaded: " << Renderer::getInstance()->getVertexBufferObjectCatalog()->getResourceCount()
                << " Active: " << Renderer::getInstance()->getVertexBufferObjectCatalog()->getActiveResourceCount()
            << "\n   IBOs:"
                << " Loaded: " << Renderer::getInstance()->getIndexBufferObjectCatalog()->getResourceCount()
                << " Active: " << Renderer::getInstance()->getIndexBufferObjectCatalog()->getActiveResourceCount()
            << "\n   Programs:"
                << " Loaded: " << Renderer::getInstance()->getShaderProgramCatalog()->getResourceCount()
                << " Active: " << Renderer::getInstance()->getShaderProgramCatalog()->getActiveResourceCount()
            << "\n----------------------------------------------------------------------------------------\n";

    // Job system
    _output << "Job count (" << ( JobScheduler::getInstance()->getNumWorkers() + 1 ) << " workers): ";
    JobScheduler::getInstance()->eachWorkerStat( [this]( JobScheduler::WorkerId workerId, const JobScheduler::WorkerStat &stat ) {
        _output << std::setw( 5 ) << std::right << stat.jobCount;
    });
    JobScheduler::getInstance()->clearWorkerStats();
    _output << std::left
            << "\n----------------------------------------------------------------------------------------\n";

    // profilers
    _output << std::setiosflags( std::ios::fixed )
            << std::setprecision( 3 )
            << std::setw( 10 ) << std::right << "MIN | "
            << std::setw( 10 ) << std::right << "AVG | "
            << std::setw( 10 ) << std::right << "MAX | "
            << std::setw( 10 ) << std::right << "TIME | "
            << std::setw( 10 ) << std::right << "COUNT | "
            << std::left << "NAME"
            << "\n----------------------------------------------------------------------------------------\n";
}

void ProfilerOutputHandler::sample( float minPc, float avgPc, float maxPc, unsigned int totalTime, unsigned int callCount, std::string name, unsigned int parentCount )
{
	_output << std::setiosflags( std::ios::fixed | std::ios::showpoint )
			<< std::setprecision( 3 )
			<< std::setw( 7 ) << std::right << minPc << " | "
			<< std::setw( 7 ) << std::right << avgPc << " | "
			<< std::setw( 7 ) << std::right << maxPc << " | "
			<< std::setw( 7 ) << std::right << totalTime << " | "
			<< std::setw( 7 ) << std::right << callCount << " | "
			<< std::left << name
			<< "\n";
}

void ProfilerOutputHandler::endOutput( void )
{
    _output << "\n";
}

ProfilerConsoleOutputHandler::ProfilerConsoleOutputHandler( void )
{

}

ProfilerConsoleOutputHandler::~ProfilerConsoleOutputHandler( void )
{

}

void ProfilerConsoleOutputHandler::endOutput( void )
{
    ProfilerOutputHandler::endOutput();

    std::cout << getOutput() << std::endl;
}

ProfilerScreenOutputHandler::ProfilerScreenOutputHandler( void )
{

}

ProfilerScreenOutputHandler::~ProfilerScreenOutputHandler( void )
{

}

void ProfilerScreenOutputHandler::endOutput( void )
{
    ProfilerOutputHandler::endOutput();

    /*
    auto screen = Renderer::getInstance()->getScreenBuffer();
    auto aspect = ( crimild::Real32 ) screen->getWidth() / ( crimild::Real32 ) screen->getHeight();

    DebugRenderHelper::renderText( getOutput(), Vector3f( -aspect + 0.01f, 0.95f, 0.0f ), RGBAColorf( 1.0f, 1.0f, 0.0f, 1.0f ) );
    */
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
    setOutputHandler( crimild::alloc< ProfilerScreenOutputHandler >() );
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

    // normalized frame time, clamped to [0, 2]
    _frameTimeHistory.push_back( Numericd::min( 2.0, ( 0.001 * frameTime ) / Clock::DEFAULT_TICK_TIME ) );
    while ( _frameTimeHistory.size() > 100 ) {
        _frameTimeHistory.pop_front();
    }

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

    const auto HISTOGRAM_WIDTH = 1.1f;
    const auto HISTOGRAM_HEIGHT = 0.15f;

    Vector3f frame[] = {
        Vector3f( -HISTOGRAM_WIDTH, -0.9f + 2.0f * HISTOGRAM_HEIGHT, 0.0f ),
        Vector3f( +HISTOGRAM_WIDTH, -0.9f + 2.0f * HISTOGRAM_HEIGHT, 0.0f ),

        Vector3f( -HISTOGRAM_WIDTH, -0.9f + 2.0f * HISTOGRAM_HEIGHT, 0.0f ),
        Vector3f( -HISTOGRAM_WIDTH, -0.9f + 0.0f * HISTOGRAM_HEIGHT, 0.0f ),

        Vector3f( -HISTOGRAM_WIDTH, -0.9f + HISTOGRAM_HEIGHT, 0.0f ),
        Vector3f( +HISTOGRAM_WIDTH, -0.9f + HISTOGRAM_HEIGHT, 0.0f ),

        Vector3f( -HISTOGRAM_WIDTH, -0.9f + 0.0f * HISTOGRAM_HEIGHT, 0.0f ),
        Vector3f( +HISTOGRAM_WIDTH, -0.9f + 0.0f * HISTOGRAM_HEIGHT, 0.0f ),

        Vector3f( +HISTOGRAM_WIDTH, -0.9f + 2.0f * HISTOGRAM_HEIGHT, 0.0f ),
        Vector3f( +HISTOGRAM_WIDTH, -0.9f + 0.0f * HISTOGRAM_HEIGHT, 0.0f ),
    };

    DebugRenderHelper::renderLines( &frame[ 0 ], 10, RGBAColorf( 1.0f, 1.0f, 1.0f, 0.5f ) );

    const auto frameTimeCount = _frameTimeHistory.size();

    std::vector< Vector3f > lines( frameTimeCount * 2 );

    for ( int i = 0; i < frameTimeCount - 1; i++ ) {
        auto f0 = _frameTimeHistory[ i ];
        auto f1 = _frameTimeHistory[ i + 1 ];

        lines[ i * 2 + 0 ] = Vector3f( -HISTOGRAM_WIDTH + 2.0 * HISTOGRAM_WIDTH * ( i / ( crimild::Real32 ) frameTimeCount ), -0.9f + HISTOGRAM_HEIGHT * f0, 0.0f );
        lines[ i * 2 + 1 ] = Vector3f( -HISTOGRAM_WIDTH + 2.0 * HISTOGRAM_WIDTH * ( ( i  + 1 ) / ( crimild::Real32 ) frameTimeCount ), -0.9f + HISTOGRAM_HEIGHT * f1, 0.0f );
    }

    DebugRenderHelper::renderLines( &lines[ 0 ], frameTimeCount * 2, RGBAColorf( 1.0f, 0.0f, 0.0f, 1.0f ) );

}
