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

#include "Rendering/DeferredRenderPass.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/VisibilitySet.hpp"
#include "Rendering/FrameBufferObject.hpp"
#include "Rendering/RenderQueue.hpp"
#include "SceneGraph/Geometry.hpp"
#include "Components/RenderStateComponent.hpp"
#include "Components/SkinComponent.hpp"
#include "Components/JointComponent.hpp"
#include "Primitives/QuadPrimitive.hpp"

using namespace crimild;

DeferredRenderPass::DeferredRenderPass( void )
{
    /*
    Pointer< FrameBufferObject > gBuffer( new FrameBufferObject( 0, 0 ) );
    
    int width = 0;
    int height = 0;
    gBuffer->getRenderTargets().add( new RenderTarget( RenderTarget::Type::DEPTH_24, RenderTarget::Output::TEXTURE, width, height ) );
    gBuffer->getRenderTargets().add( new RenderTarget( RenderTarget::Type::COLOR_RGBA, RenderTarget::Output::TEXTURE, width, height ) );
    gBuffer->getRenderTargets().add( new RenderTarget( RenderTarget::Type::COLOR_RGBA, RenderTarget::Output::TEXTURE, width, height ) );
    gBuffer->getRenderTargets().add( new RenderTarget( RenderTarget::Type::COLOR_RGBA, RenderTarget::Output::TEXTURE, width, height ) );
     */
}

DeferredRenderPass::~DeferredRenderPass( void )
{
    
}

void DeferredRenderPass::render( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{

}

