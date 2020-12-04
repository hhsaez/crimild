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

#include "RenderSystem.hpp"

#include "Rendering/Compositions/BloomComposition.hpp"
#include "Rendering/Compositions/DebugComposition.hpp"
#include "Rendering/Compositions/PresentComposition.hpp"
#include "Rendering/Compositions/RenderScene.hpp"
#include "Rendering/Compositions/TonemappingComposition.hpp"
#include "Simulation/Simulation.hpp"

using namespace crimild;

void RenderSystem::lateStart( void ) noexcept
{
    auto sim = Simulation::getInstance();
    if ( sim->getComposition().getOutput() != nullptr ) {
        // Composition is valid. Nothing to do here
        return;
    }

    auto scene = sim->getScene();
    if ( scene == nullptr ) {
        CRIMILD_LOG_WARNING( "No available scene and no composition defined by user" );
        return;
    }

    CRIMILD_LOG_DEBUG( "No composition provided. Using default one" );
    sim->setComposition(
        [ scene ] {
            using namespace crimild::compositions;
            auto settings = Simulation::getInstance()->getSettings();
            auto enableTonemapping = settings->get< Bool >( "tonemapping", true );
            auto enableBloom = settings->get< Bool >( "bloom", false );
            auto enableDebug = settings->get< Bool >( "debug", false );

            auto withTonemapping = [ enableTonemapping ]( auto cmp ) {
                return enableTonemapping ? tonemapping( cmp, 0.5 ) : cmp;
            };

            auto withDebug = [ enableDebug ]( auto cmp ) {
                return enableDebug ? debug( cmp ) : cmp;
            };

            auto withBloom = [ enableBloom ]( auto cmp ) {
                return enableBloom ? bloom( cmp ) : cmp;
            };

            auto withHDR = [ enableHDR = settings->get< Bool >( "hdr", true ) ]( auto scene ) {
                return enableHDR ? renderSceneHDR( scene ) : renderScene( scene );
            };

            return present( withDebug( withTonemapping( withBloom( withHDR( scene ) ) ) ) );
        }() );
}
