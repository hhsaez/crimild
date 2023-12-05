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

#ifndef CRIMILD_CORE_FOUNDATION_PROFILER_
#define CRIMILD_CORE_FOUNDATION_PROFILER_

#include "Macros.hpp"
#include "Memory.hpp"
#include "SharedObject.hpp"
#include "Singleton.hpp"
#include "Types.hpp"

#include <string>
#include <sstream>
#include <thread>
#include <map>
#include <deque>

#ifndef CRIMILD_PROFILER_ENABLED
#define CRIMILD_PROFILER_ENABLED 1
#endif

namespace crimild {

    class ProfilerSample {
    public:
        explicit ProfilerSample( std::string name );
        ~ProfilerSample( void );

    private:
        int _sampleIndex;
    };

    class ProfilerOutputHandler : public SharedObject {
    public:
        virtual ~ProfilerOutputHandler( void );

        virtual void beginOutput( crimild::Size fps, crimild::Real64 avgFrameTime, crimild::Real64 minFrameTime, crimild::Real64 maxFrameTime );
        virtual void sample( float minPc, float avgPc, float maxPc, unsigned int totalTime, unsigned int callCount, std::string name, unsigned int parentCount );
        virtual void endOutput( void );

    protected:
        inline std::string getOutput( void ) { return _output.str(); }

    private:
        std::stringstream _output;
    };

    using ProfilerOutputHandlerPtr = SharedPointer< ProfilerOutputHandler >;

    class ProfilerConsoleOutputHandler : public ProfilerOutputHandler {
    public:
        ProfilerConsoleOutputHandler( void );
        virtual ~ProfilerConsoleOutputHandler( void );

        virtual void endOutput( void ) override;
    };

    class ProfilerScreenOutputHandler : public ProfilerOutputHandler {
    public:
        ProfilerScreenOutputHandler( void );
        virtual ~ProfilerScreenOutputHandler( void );

        virtual void endOutput( void ) override;
    };

    class Profiler : public DynamicSingleton< Profiler > {
    private:
        static constexpr int MAX_SAMPLES = 50;

    public:
        Profiler( void );
        ~Profiler( void );

		void step( void );
		
        void dump( void );

        void resetAll( void );
        void reset( std::string name );

        void setOutputHandler( ProfilerOutputHandlerPtr const &handler ) { _outputHandler = handler; }
        ProfilerOutputHandlerPtr &getOutputHandler( void ) { return _outputHandler; }

    // internal use only
    public: 
        int onSampleCreated( std::string name );
        void onSampleDestroyed( int index );

    private:
		crimild::Real64 getTime( void );

    private:

        struct ProfilerSampleInfo {
            bool isValid = false;
            bool isOpened = false;

            std::string name;
            unsigned int callCount = 0;

			crimild::Real64 startTime = 0;
			crimild::Real64 totalTime = 0;
			crimild::Real64 childTime = 0;

			crimild::Real64 avgTime = 0.0f;
			crimild::Real64 minTime = 0.0f;
			crimild::Real64 maxTime = 0.0f;

            unsigned int dataCount = 0;

            int parentCount = 0;
            int parentIndex = -1;
        };

        ProfilerSampleInfo _samples[ MAX_SAMPLES ];

        int _lastOpenedSample = -1;
        int _openSampleCount = 0;
        float _rootBegin = 0.0f;
        float _rootEnd = 0.0f;

        ProfilerOutputHandlerPtr _outputHandler;

		crimild::Real64 _minFrameTime = 0;
		crimild::Real64 _maxFrameTime = 0;
		crimild::Size _frameCount = 0;
		crimild::Real64 _totalFrameTime = 0;
		crimild::Real64 _lastFrameTime = 0;
        std::deque< crimild::Real64 > _frameTimeHistory;

		crimild::Size _fps = 0;
		crimild::Real64 _avgFrameTime = 0;
    };

}

#if CRIMILD_PROFILER_ENABLED
    #define CRIMILD_PROFILE( X ) crimild::ProfilerSample __crimild__profile__sample__instance__( X );
#else
    #define CRIMILD_PROFILE( X )
#endif

#endif

