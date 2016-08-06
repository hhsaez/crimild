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

#import "CrimildMetalFrameBufferObjectCatalog.h"
#import "CrimildMetalTextureCatalog.h"

#import "CrimildMetalRenderer.h"

#if !TARGET_OS_SIMULATOR

using namespace crimild;
using namespace crimild::metal;

FrameBufferObjectCatalog::FrameBufferObjectCatalog( MetalRenderer *renderer )
    : _renderer( renderer )
{
    
}

FrameBufferObjectCatalog::~FrameBufferObjectCatalog( void )
{
    
}

int FrameBufferObjectCatalog::getNextResourceId( void )
{
    return Catalog< FrameBufferObject >::getNextResourceId();
}

void FrameBufferObjectCatalog::bind( FrameBufferObject *fbo )
{
    Catalog< FrameBufferObject >::bind( fbo );
    
    auto &cachedFBO = _fboCache[ fbo->getCatalogId() ];
    
    auto renderPassDescriptor = cachedFBO.renderPassDescriptor;
    if ( renderPassDescriptor == nil ) {
        Log::Error << "Cannot obtain a render pass descriptor" << Log::End;
        return;
    }
    
    if ( cachedFBO.texture == nil ) {
        renderPassDescriptor.colorAttachments[ 0 ].texture = getRenderer()->getDrawable().texture;
    }
    
    auto renderEncoder = [getRenderer()->getCommandBuffer() renderCommandEncoderWithDescriptor: renderPassDescriptor];
    if ( renderEncoder == nil ) {
        Log::Error << "Cannot create render encoder" << Log::End;
        return;
    }
    
    getRenderer()->setRenderEncoder( renderEncoder );
    
    float width = fbo->getWidth();
    float height = fbo->getHeight();
    auto viewport = MTLViewport {
        0.0f, 0.0f,
        width, height,
        0.0f, 1.0f
    };
    
    [getRenderer()->getRenderEncoder() setViewport: viewport];
    
    [getRenderer()->getRenderEncoder() setFrontFacingWinding: MTLWindingCounterClockwise];
    
    MTLDepthStencilDescriptor *depthStateDesc = [MTLDepthStencilDescriptor new];
    depthStateDesc.depthCompareFunction = MTLCompareFunctionLess;
    depthStateDesc.depthWriteEnabled = YES;
    auto depthStateState = [getRenderer()->getDevice() newDepthStencilStateWithDescriptor: depthStateDesc];
    [getRenderer()->getRenderEncoder() setDepthStencilState: depthStateState];
    depthStateDesc = nil;
}

void FrameBufferObjectCatalog::unbind( FrameBufferObject *fbo )
{
    Catalog< FrameBufferObject >::unbind( fbo );
    
    [getRenderer()->getRenderEncoder() endEncoding];
}

void FrameBufferObjectCatalog::load( FrameBufferObject *fbo )
{
    Catalog< FrameBufferObject >::load( fbo );
    
    auto &cache = _fboCache[ fbo->getCatalogId() ];

    cache.renderPassDescriptor = [MTLRenderPassDescriptor new];
    cache.renderPassDescriptor.colorAttachments[ 0 ].loadAction = MTLLoadActionClear;
    const RGBAColorf &clearColor = fbo->getClearColor();
    cache.renderPassDescriptor.colorAttachments[ 0 ].clearColor = MTLClearColorMake( clearColor[ 0 ], clearColor[ 1 ], clearColor[ 2 ], clearColor[ 3 ] );
    cache.renderPassDescriptor.colorAttachments[ 0 ].storeAction = MTLStoreActionStore;
    
    if ( fbo == getRenderer()->getScreenBuffer() ) {
        // no need for further operations
        return;
    }
    
    auto textureCatalog = static_cast< TextureCatalog * >( getRenderer()->getTextureCatalog() );
    
    int currentColorAttachment = 0;
    fbo->getRenderTargets().each( [textureCatalog, &cache, &currentColorAttachment]( std::string, RenderTarget *renderTarget ) {
        if ( ( renderTarget->getType() == RenderTarget::Type::COLOR_RGB || renderTarget->getType() == RenderTarget::Type::COLOR_RGBA ) &&
             ( renderTarget->getOutput() == RenderTarget::Output::TEXTURE || renderTarget->getOutput() == RenderTarget::Output::RENDER_AND_TEXTURE ) ) {
            
            cache.texture = textureCatalog->generateRenderTargetTexture( renderTarget );
            cache.renderPassDescriptor.colorAttachments[ currentColorAttachment++ ].texture = cache.texture;
        }
    });
}

void FrameBufferObjectCatalog::unload( FrameBufferObject *fbo )
{
    Catalog< FrameBufferObject >::unload( fbo );
}

void FrameBufferObjectCatalog::cleanup( void )
{
    Catalog< FrameBufferObject >::cleanup();
}

#endif // TARGET_OS_SIMULATOR

