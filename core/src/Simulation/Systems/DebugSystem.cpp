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

#include "DebugSystem.hpp"

#include "Concurrency/Async.hpp"
#include "Debug/DebugRenderHelper.hpp"
#include "Simulation/Settings.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/Apply.hpp"

using namespace crimild;

void DebugSystem::start( void ) noexcept
{
    /*
	if ( !System::start() ) {
		return false;
	}

    DebugRenderHelper::init();

	_profilerInfoEnabled = Simulation::getInstance()->getSettings()->get< crimild::Bool >( "profiler.enabled", false );

    CRIMILD_BIND_MEMBER_MESSAGE_HANDLER( messaging::ToggleDebugInfo, DebugSystem, onToggleDebugInfo );
    CRIMILD_BIND_MEMBER_MESSAGE_HANDLER( messaging::ToggleProfilerInfo, DebugSystem, onToggleProfilerInfo );

	return true;
    */
}

void DebugSystem::update( void ) noexcept
{
    auto renderer = Simulation::getInstance()->getRenderer();
    auto scene = Simulation::getInstance()->getScene();
    auto camera = Simulation::getInstance()->getMainCamera();

    if ( renderer == nullptr ) {
        return;
    }

    if ( scene != nullptr && camera != nullptr ) {
        if ( _debugInfoEnabled ) {
            scene->perform( Apply( [ renderer, camera ]( Node *node ) {
                node->forEachComponent( [ renderer, camera ]( NodeComponent *component ) {
                    component->renderDebugInfo( renderer, camera );
                } );
            } ) );
        }
    }

    Profiler::getInstance()->step();

    auto profilerEnabled = Simulation::getInstance()->getSettings()->get< crimild::Bool >( "profiler.enabled", false );
    if ( profilerEnabled ) {
        Profiler::getInstance()->dump();

        static double accum = 0.0;
        auto t = Simulation::getInstance()->getSimulationClock();
        accum += t.getDeltaTime();
        if ( accum >= 1.0 ) {
            Profiler::getInstance()->resetAll();
            accum = 0.0;
        }
    }
}

void DebugSystem::stop( void ) noexcept
{
    System::stop();
}

void DebugSystem::onToggleDebugInfo( messaging::ToggleDebugInfo const & )
{
    /*
    _debugInfoEnabled = !_debugInfoEnabled;
    if ( _debugInfoEnabled ) {
        broadcastMessage( messaging::DebugModeEnabled {} );
    } else {
        broadcastMessage( messaging::DebugModeDisabled {} );
    }
    */
}

void DebugSystem::onToggleProfilerInfo( messaging::ToggleProfilerInfo const & )
{
    auto profilerEnabled = Simulation::getInstance()->getSettings()->get< crimild::Bool >( "profiler.enabled", false );
    Simulation::getInstance()->getSettings()->set( "profiler.enabled", !profilerEnabled );
}
