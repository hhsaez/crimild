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

#include "Simulation/Simulation.hpp"

#include "Debug/DebugRenderHelper.hpp"

#include "Visitors/Apply.hpp"

#include "Concurrency/Async.hpp"

using namespace crimild;

DebugSystem::DebugSystem( void )
	: System( "Debug System" )
{
    CRIMILD_BIND_MEMBER_MESSAGE_HANDLER( messaging::DidRenderScene, DebugSystem, onDidRenderScene );
    CRIMILD_BIND_MEMBER_MESSAGE_HANDLER( messaging::ToggleDebugInfo, DebugSystem, onToggleDebugInfo );
    CRIMILD_BIND_MEMBER_MESSAGE_HANDLER( messaging::ToggleProfilerInfo, DebugSystem, onToggleProfilerInfo );
}

DebugSystem::~DebugSystem( void )
{

}

bool DebugSystem::start( void )
{	
	if ( !System::start() ) {
		return false;
	}
    
    DebugRenderHelper::init();
    
	return true;
}

void DebugSystem::stop( void )
{
	System::stop();
}

void DebugSystem::onDidRenderScene( messaging::DidRenderScene const & )
{
    auto renderer = Simulation::getInstance()->getRenderer();
    auto scene = Simulation::getInstance()->getScene();
    auto camera = Simulation::getInstance()->getMainCamera();
    
    if ( renderer == nullptr || scene == nullptr || camera == nullptr ) {
        return;
    }
    
    if ( _debugInfoEnabled ) {
        scene->perform( Apply( [renderer, camera]( NodePtr const &node ) {
            node->foreachComponent( [renderer, camera]( NodeComponentPtr const &component ) {
                component->renderDebugInfo( renderer, camera );
            });
        }));
    }
    
    if ( _profilerInfoEnabled ) {
        Profiler::getInstance()->dump();

        static double accum = 0.0;
        auto t = Simulation::getInstance()->getSimulationClock();
        accum += t.getDeltaTime() * 100.0;
        if ( accum >= 1.0 ) {
            Profiler::getInstance()->resetAll();
            accum = 0.0;
        }
    }
}

void DebugSystem::onToggleDebugInfo( messaging::ToggleDebugInfo const & )
{
    _debugInfoEnabled = !_debugInfoEnabled;
}

void DebugSystem::onToggleProfilerInfo( messaging::ToggleProfilerInfo const & )
{
    _profilerInfoEnabled = !_profilerInfoEnabled;
}

