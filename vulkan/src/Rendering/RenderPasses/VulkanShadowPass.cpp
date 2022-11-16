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

#include "Rendering/RenderPasses/VulkanShadowPass.hpp"

#include "Rendering/RenderPasses/VulkanGenerateDirectionalLightsShadowMapsPass.hpp"
#include "Rendering/RenderPasses/VulkanGeneratePointLightsShadowMapsPass.hpp"
#include "Rendering/RenderPasses/VulkanGenerateSpotLightsShadowMapsPass.hpp"

using namespace crimild;
using namespace crimild::vulkan;

ShadowPass::ShadowPass( RenderDevice *renderDevice ) noexcept
    : RenderPassBase( renderDevice )
{
    m_generateDirectionalLightsShadowMaps = crimild::alloc< GenerateDirectionalLightsShadowMaps >( renderDevice );
    m_generateSpotLightsShadowMaps = crimild::alloc< GenerateSpotLightsShadowMaps >( renderDevice );
    m_generatePointLightsShadowMaps = crimild::alloc< GeneratePointLightsShadowMaps >( renderDevice );
}

ShadowPass::~ShadowPass( void ) noexcept
{
    m_generateDirectionalLightsShadowMaps = nullptr;
    m_generateSpotLightsShadowMaps = nullptr;
    m_generatePointLightsShadowMaps = nullptr;
}

Event ShadowPass::handle( const Event &e ) noexcept
{
    m_generateDirectionalLightsShadowMaps->handle( e );
    m_generateSpotLightsShadowMaps->handle( e );
    m_generatePointLightsShadowMaps->handle( e );

    return e;
}

void ShadowPass::render(
    const SceneRenderState::Lights &lights,
    const SceneRenderState::ShadowCasters &shadowCasters,
    const Camera *camera
) noexcept
{
    m_generateDirectionalLightsShadowMaps->render( lights, shadowCasters, camera );
    m_generateSpotLightsShadowMaps->render( lights, shadowCasters, camera );
    m_generatePointLightsShadowMaps->render( lights, shadowCasters, camera );
}
