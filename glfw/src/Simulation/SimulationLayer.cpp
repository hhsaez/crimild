/*
 * Copyright (c) 2002 - present, H. Hernan Saez
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

#include "Simulation/SimulationLayer.hpp"

#include "Foundation/Log.hpp"
#include "Simulation/Settings.hpp"
#include "Simulation/Simulation.hpp"

#include <array>

using namespace crimild;

SimulationLayer::SimulationLayer( vulkan::RenderDevice *renderDevice ) noexcept
    : m_renderDevice( renderDevice ),
      m_scenePass( renderDevice ),
      m_blitPass(
          renderDevice,
          m_scenePass.getColorAttachment()
      )
{
    CRIMILD_LOG_TRACE();
}

Event SimulationLayer::handle( const Event &e ) noexcept
{
    m_scenePass.handle( e );
    m_blitPass.handle( e );

    if ( e.type != Event::Type::TICK ) {
        // TODO: Handle return event
        Simulation::getInstance()->handle( e );
    }

    return Layer::handle( e );
}

void SimulationLayer::render( void ) noexcept
{
    Layer::render();

    m_scenePass.render(
        Simulation::getInstance()->getScene(),
        Simulation::getInstance()->getMainCamera()
    );

    m_blitPass.render();
}
