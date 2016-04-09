/*
 * Copyright (c) 2014, Hernan Saez
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

#import "CrimildMetalRenderer.h"

#import "CrimildMetalVertexBufferObjectCatalog.h"
#import "CrimildMetalIndexBufferObjectCatalog.h"
#import "CrimildMetalShaderProgramCatalog.h"

#import "CrimildMetalView.h"

#import "CrimildMetalUnlitDiffuseShaderProgram.h"

using namespace crimild;
using namespace crimild::metal;

static const long IN_FLIGHT_COMMAND_BUFFERS = 1;

MetalRenderer::MetalRenderer( CrimildMetalView *view )
    : _view( view )
{
    setScreenBuffer( crimild::alloc< FrameBufferObject >( getView().bounds.size.width, getView().bounds.size.height ) );

    setVertexBufferObjectCatalog( crimild::alloc< VertexBufferObjectCatalog > ( this ) );
    setIndexBufferObjectCatalog( crimild::alloc< IndexBufferObjectCatalog > ( this ) );
    setShaderProgramCatalog( crimild::alloc< ShaderProgramCatalog > ( this ) );
    
    setShaderProgram( Renderer::SHADER_PROGRAM_UNLIT_TEXTURE, crimild::alloc< UnlitDiffuseShaderProgram >() );
    setShaderProgram( Renderer::SHADER_PROGRAM_UNLIT_DIFFUSE, crimild::alloc< UnlitDiffuseShaderProgram >() );
}

MetalRenderer::~MetalRenderer( void )
{
    _depthStencilState = nil;
    _commandQueue = nil;
    _device = nil;
    _view = nil;
    _layer = nil;
}

void MetalRenderer::configure( void )
{
    _device = MTLCreateSystemDefaultDevice();

    _commandQueue = [_device newCommandQueue];

    _layer = (CAMetalLayer *) getView().layer;
    _layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    _layer.framebufferOnly = true;
    _layer.presentsWithTransaction = false;
    _layer.drawsAsynchronously = true;
    _layer.device = _device;

    CGRect bounds = getView().frame;
    _viewport = MTLViewport {
        0.0f, 0.0f,
        bounds.size.width, bounds.size.height,
        0.0f, 1.0f
    };

    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    if ( colorSpace != nullptr ) {
        CGFloat components[ 4 ] = { 0.5, 0.5, 0.5, 1.0 };   // TODO: set alpha to 0?
        CGColorRef grayColor = CGColorCreate( colorSpace, components );
        
        if ( grayColor != nullptr ) {
            _layer.backgroundColor = grayColor;
            CFRelease( grayColor );
        }
        
        CFRelease( colorSpace );
    }
    
    MTLDepthStencilDescriptor *depthStateDesc = [MTLDepthStencilDescriptor new];
    if ( !depthStateDesc ) {
        Log::Error << "Cannot create depth-stencil descriptor";
        return;
    }
    depthStateDesc.depthCompareFunction = MTLCompareFunctionAlways;
    depthStateDesc.depthWriteEnabled = true;
    _depthStencilState = [_device newDepthStencilStateWithDescriptor: depthStateDesc];
    depthStateDesc = nil;
    
    // create semaphores
    _inflightSemaphore = dispatch_semaphore_create( IN_FLIGHT_COMMAND_BUFFERS );
}

void MetalRenderer::setViewport( const Rectf &viewport )
{

}

void MetalRenderer::beginRender( void )
{
    Renderer::beginRender();

    dispatch_semaphore_wait( _inflightSemaphore, DISPATCH_TIME_FOREVER );
    
    _drawable = [_layer nextDrawable];
    if ( _drawable == nil ) {
        Log::Error << "Cannot obtain next drawable" << Log::End;
        return;
    }
    
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
    
    renderPassDescriptor.colorAttachments[0].texture = _drawable.texture;
    renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
    
    const RGBAColorf &clearColor = getScreenBuffer()->getClearColor();
    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake( clearColor[ 0 ], clearColor[ 1 ], clearColor[ 2 ], clearColor[ 3 ] );
    
    renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
    
    _renderEncoder = [_commandBuffer renderCommandEncoderWithDescriptor: renderPassDescriptor];
    if ( _renderEncoder == nil ) {
        Log::Error << "Cannot create render encoder" << Log::End;
        return;
    }
    
    [_renderEncoder setViewport: _viewport];
    [_renderEncoder setFrontFacingWinding: MTLWindingCounterClockwise];
    [_renderEncoder setDepthStencilState: _depthStencilState];
}

void MetalRenderer::endRender( void )
{
    Renderer::endRender();

    [_renderEncoder endEncoding];
    
    __block dispatch_semaphore_t dispatchSemaphore = _inflightSemaphore;
    
    [_commandBuffer addCompletedHandler:^(id<MTLCommandBuffer>) {
        dispatch_semaphore_signal( dispatchSemaphore );
    }];
    
    [_commandBuffer presentDrawable: _drawable];
    [_commandBuffer commit];
}

void MetalRenderer::clearBuffers( void )
{

}

void MetalRenderer::bindUniform( ShaderLocation *location, int value )
{

}

void MetalRenderer::bindUniform( ShaderLocation *location, float value )
{

}

void MetalRenderer::bindUniform( ShaderLocation *location, const Vector3f &vector )
{
    
}

void MetalRenderer::bindUniform( ShaderLocation *location, const Vector2f &vector )
{

}

void MetalRenderer::bindUniform( ShaderLocation *location, const RGBAColorf &color )
{

}

void MetalRenderer::bindUniform( ShaderLocation *location, const Matrix4f &matrix )
{
    /*
    if ( location == nullptr ) {
        return;
    }
    
    auto uniforms = [_device newBufferWithLength: sizeof( float ) * 16 options: 0];
    float *data = ( float * )[uniforms contents];
    memcpy( data, matrix.getData(), 16 * sizeof( float ) );
    [getRenderEncoder() setVertexBuffer: uniforms offset: 0 atIndex: location->getLocation()];
     */
}

void MetalRenderer::applyTransformations( ShaderProgram *program, const Matrix4f &projection, const Matrix4f &view, const Matrix4f &model, const Matrix4f &normal )
{
//    Renderer::applyTransformations( program, projection, view, model, normal );
     //TODO: do not allocate this every frame!
    _uniforms = [_device newBufferWithLength: sizeof( float ) * 16 * 3 options:0];
    float *data = ( float * )[_uniforms contents];

    memcpy( &data[ 0 * 16 ], model.getData(), 16 * sizeof( float ) );
    memcpy( &data[ 1 * 16 ], view.getData(), 16 * sizeof( float ) );
    memcpy( &data[ 2 * 16 ], projection.getData(), 16 * sizeof( float ) );
}

void MetalRenderer::drawPrimitive( ShaderProgram *program, Primitive *primitive )
{
    [getRenderEncoder() setVertexBuffer: _uniforms offset: 0 atIndex: 2];
    
    [getRenderEncoder() drawPrimitives: MTLPrimitiveTypeTriangle
                           vertexStart: 0
                           vertexCount: primitive->getIndexBuffer()->getIndexCount()
                         instanceCount: 1];
}

void MetalRenderer::drawBuffers( ShaderProgram *program, Primitive::Type bufferType, VertexBufferObject *vbo, unsigned int count )
{

}

void MetalRenderer::setAlphaState( AlphaState *state )
{

}

void MetalRenderer::setDepthState( DepthState *state )
{

}

void MetalRenderer::setCullFaceState( CullFaceState *state )
{

}

