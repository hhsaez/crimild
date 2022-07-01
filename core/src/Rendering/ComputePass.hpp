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
 *     * Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CRIMILD_CORE_RENDERING_COMPUTE_PASS_
#define CRIMILD_CORE_RENDERING_COMPUTE_PASS_

#include "Foundation/SharedObject.hpp"
#include "Rendering/CommandBuffer.hpp"
#include "Rendering/FrameGraphOperation.hpp"
#include "Rendering/RenderResource.hpp"

namespace crimild {

    class [[deprecated]] ComputePass
        : public FrameGraphOperation,
          public RenderResourceImpl< ComputePass > {

    public:
        virtual ~ComputePass( void ) = default;

        inline FrameGraphOperation::Type getType( void ) const noexcept override { return FrameGraphOperation::Type::COMPUTE_PASS; }

        inline void setCommandBuffers( Array< SharedPointer< CommandBuffer > > const &commands ) noexcept { m_commands = commands; }
        inline Array< SharedPointer< CommandBuffer > > &getCommandBuffers( void ) noexcept { return m_commands; }

    private:
        Array< SharedPointer< CommandBuffer > > m_commands;

        // DEPRECATED FROM HERE

    public:
        inline void setCommandRecorder( CommandRecorder commandRecorder ) noexcept { m_commandRecorder = commandRecorder; }

        inline CommandBuffer *execute( Size imageIndex ) noexcept
        {
            return m_commandRecorder != nullptr ? m_commandRecorder( imageIndex ) : nullptr;
        }

    private:
        CommandRecorder m_commandRecorder;

    public:
        void setConditional( Bool conditional ) noexcept { m_conditional = conditional; }
        Bool isConditional( void ) const noexcept { return m_conditional; }

    private:
        Bool m_conditional = false;
    };

}

#endif
