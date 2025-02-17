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

#ifndef CRIMILD_SIMULATION_SYSTEMS_RENDER_
#define CRIMILD_SIMULATION_SYSTEMS_RENDER_

#include "Crimild_Foundation.hpp"
#include "Simulation/Systems/System.hpp"

namespace crimild {

    class FrameGraphOperation;
    class CommandBuffer;
    class RenderPass;
    class ComputePass;
    class ScenePass;

    namespace messaging {

        struct FrameGraphDidChange {
            // no data
        };

    }

    class RenderSystem
        : public System,
          public DynamicSingleton< RenderSystem > {
        CRIMILD_IMPLEMENT_RTTI( crimild::RenderSystem )

    public:
        void start( void ) noexcept override;

        void onPreRender( void ) noexcept override;

        void setFrameGraph( SharedPointer< FrameGraphOperation > const &frameGraph ) noexcept;
        inline SharedPointer< FrameGraphOperation > &getFrameGraph( void ) noexcept { return m_frameGraph; }

        using CommandBufferArray = Array< CommandBuffer * >;

        CommandBufferArray &getGraphicsCommands( Size imageIndex, Bool includeConditionalPasses ) noexcept;
        CommandBufferArray &getComputeCommands( Size imageIndex, Bool includeConditionalPasses ) noexcept;

        void useDefaultRenderPath( Bool debug = false ) noexcept;
        void useRTSoftRenderPath( Bool debug = false ) noexcept;
        void useRTComputeRenderPath( Bool debug = false ) noexcept;

    private:
        void sort( SharedPointer< FrameGraphOperation > const &root ) noexcept;

    private:
        SharedPointer< FrameGraphOperation > m_frameGraph;
        Array< SharedPointer< FrameGraphOperation > > m_sortedOperations;

        Array< SharedPointer< ScenePass > > m_scenePasses;
        Array< SharedPointer< RenderPass > > m_renderPasses;
        Array< SharedPointer< ComputePass > > m_computePasses;

        CommandBufferArray m_graphicsCommands;
        CommandBufferArray m_computeCommands;
    };

}

#endif
