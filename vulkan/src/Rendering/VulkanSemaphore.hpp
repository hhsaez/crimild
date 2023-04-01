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

#ifndef CRIMILD_VULKAN_RENDERING_SEMAPHORE
#define CRIMILD_VULKAN_RENDERING_SEMAPHORE

#include "Foundation/Named.hpp"
#include "Foundation/VulkanUtils.hpp"

namespace crimild::vulkan {

    class Semaphore
        : public SharedObject,
          public WithRenderDevice,
          public Named,
          public WithHandle< VkSemaphore > {
    public:
        Semaphore( RenderDevice *device, std::string name, VkPipelineStageFlags waitStageMask ) noexcept;
        virtual ~Semaphore( void ) noexcept;

        inline VkPipelineStageFlags getWaitStageMask( void ) const noexcept { return m_waitStageMask; }

    private:
        VkPipelineStageFlags m_waitStageMask = VK_PIPELINE_STAGE_NONE;
    };

    class WithSemaphore {
    protected:
        WithSemaphore( RenderDevice *device, std::string name, VkPipelineStageFlags waitStageMask ) noexcept
            : m_semaphore( crimild::alloc< Semaphore >( device, name, waitStageMask ) )
        {
            // no-op
        }

    public:
        virtual ~WithSemaphore( void ) = default;

        inline std::shared_ptr< Semaphore > &getSemaphore( void ) noexcept { return m_semaphore; }

    private:
        std::shared_ptr< Semaphore > m_semaphore;
    };

}

#endif
