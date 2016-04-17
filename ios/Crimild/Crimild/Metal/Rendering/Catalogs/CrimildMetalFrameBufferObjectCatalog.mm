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

#import "CrimildMetalRenderer.h"

using namespace crimild;
using namespace crimild::metal;

static const long CRIMILD_METAL_IN_FLIGHT_COMMAND_BUFFERS = 1;

FrameBufferObjectCatalog::FrameBufferObjectCatalog( MetalRenderer *renderer )
    : _nextBufferId( 0 ),
      _renderer( renderer )
{
    _inflightSemaphore = dispatch_semaphore_create( CRIMILD_METAL_IN_FLIGHT_COMMAND_BUFFERS );
    
    _commandQueue = [getRenderer()->getDevice() newCommandQueue];
}

FrameBufferObjectCatalog::~FrameBufferObjectCatalog( void )
{
    _commandQueue = nil;
}

int FrameBufferObjectCatalog::getNextResourceId( void )
{
    int bufferId = _nextBufferId++;
    return bufferId;
}

void FrameBufferObjectCatalog::bind( FrameBufferObject *fbo )
{
    Catalog< FrameBufferObject >::bind( fbo );
    
    dispatch_semaphore_wait( _inflightSemaphore, DISPATCH_TIME_FOREVER );
    
    _commandBuffer = [_commandQueue commandBuffer];
    if ( _commandBuffer == nil ) {
        Log::Error << "Cannot obtain command buffer" << Log::End;
        return;
    }
    
    MTLRenderPassDescriptor *renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    if ( renderPassDescriptor == nil ) {
        Log::Error << "Cannot obtain a render pass descriptor" << Log::End;
        return;
    }
    
    if ( fbo == getRenderer()->getScreenBuffer() ) {
        id< CAMetalDrawable > nextDrawable = [getRenderer()->getLayer() nextDrawable];
        if ( nextDrawable == nil ) {
            Log::Error << "Cannot obtain next drawable" << Log::End;
            return;
        }
        _drawable = nextDrawable;
        renderPassDescriptor.colorAttachments[ 0 ].texture = nextDrawable.texture;
    }
    else {
        assert( false && "Unsorported" );
    }
    
    renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
    
    const RGBAColorf &clearColor = fbo->getClearColor();
    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake( clearColor[ 0 ], clearColor[ 1 ], clearColor[ 2 ], clearColor[ 3 ] );
    
    renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
    
    auto renderEncoder = [_commandBuffer renderCommandEncoderWithDescriptor: renderPassDescriptor];
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
    
    __block dispatch_semaphore_t dispatchSemaphore = _inflightSemaphore;
    
    [_commandBuffer addCompletedHandler:^(id<MTLCommandBuffer>) {
        dispatch_semaphore_signal( dispatchSemaphore );
    }];
    
    [_commandBuffer presentDrawable: _drawable];
    [_commandBuffer commit];
    
}

void FrameBufferObjectCatalog::load( FrameBufferObject *fbo )
{
    Catalog< FrameBufferObject >::load( fbo );
}

void FrameBufferObjectCatalog::unload( FrameBufferObject *fbo )
{
    Catalog< FrameBufferObject >::unload( fbo );
}

void FrameBufferObjectCatalog::cleanup( void )
{
    Catalog< FrameBufferObject >::cleanup();
}

