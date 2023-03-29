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

#ifndef CRIMILD_EDITOR_PANELS_SCENE_RT
#define CRIMILD_EDITOR_PANELS_SCENE_RT

#include "Foundation/VulkanUtils.hpp"
#include "Panels/Panel.hpp"

namespace crimild::vulkan::framegraph {

    class RenderSceneRT;

}

namespace crimild::editor::panels {

    class SceneRT
        : public Panel,
          public vulkan::WithRenderDevice,
          public DynamicSingleton< SceneRT > {
    public:
        SceneRT( vulkan::RenderDevice *renderDevice ) noexcept;
        virtual ~SceneRT( void ) = default;

        virtual Event handle( const Event &e ) noexcept;

        virtual const char *getTitle( void ) const noexcept override { return "Scene RT"; }

    protected:
        virtual void onRender( void ) noexcept override;

    private:
        std::shared_ptr< vulkan::framegraph::RenderSceneRT > m_framegraph;
        std::shared_ptr< ImGuiVulkanTexture > m_outputTexture;
    };

}

#endif
