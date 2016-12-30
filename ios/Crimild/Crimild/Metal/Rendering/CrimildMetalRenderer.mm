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

#if !TARGET_OS_SIMULATOR

#import "CrimildMetalVertexBufferObjectCatalog.h"
#import "CrimildMetalIndexBufferObjectCatalog.h"
#import "CrimildMetalTextureCatalog.h"
#import "CrimildMetalShaderProgramCatalog.h"
#import "CrimildMetalFrameBufferObjectCatalog.h"

#import "CrimildMetalView.h"

#import "CrimildMetalUnlitDiffuseShaderProgram.h"
#import "CrimildMetalUnlitTextureShaderProgram.h"
#import "CrimildMetalLitTextureShaderProgram.h"
#import "CrimildMetalTextShaderProgram.h"
#import "CrimildMetalForwardShaderProgram.h"
#import "CrimildMetalScreenTextureShaderProgram.h"

using namespace crimild;
using namespace crimild::metal;

static const long CRIMILD_METAL_IN_FLIGHT_COMMAND_BUFFERS = 3;

simd::float4x4 convertMatrix( const Matrix4f &input )
{
    simd::float4 c0 = { input[ 0 ], input[ 1 ], input[ 2 ], input[ 3 ] };
    simd::float4 c1 = { input[ 4 ], input[ 5 ], input[ 6 ], input[ 7 ] };
    simd::float4 c2 = { input[ 8 ], input[ 9 ], input[ 10 ], input[ 11 ] };
    simd::float4 c3 = { input[ 12 ], input[ 13 ], input[ 14 ], input[ 15 ] };
    
    return simd::float4x4( c0, c1, c2, c3 );
}

void matrix2float4x4( const Matrix4f &input, simd::float4x4 &output )
{
    simd::float4 c0 = { input[ 0 ], input[ 1 ], input[ 2 ], input[ 3 ] };
    simd::float4 c1 = { input[ 4 ], input[ 5 ], input[ 6 ], input[ 7 ] };
    simd::float4 c2 = { input[ 8 ], input[ 9 ], input[ 10 ], input[ 11 ] };
    simd::float4 c3 = { input[ 12 ], input[ 13 ], input[ 14 ], input[ 15 ] };
    
    memcpy( &output.columns[ 0 ], &c0, 4 * sizeof( float ) );
    memcpy( &output.columns[ 1 ], &c1, 4 * sizeof( float ) );
    memcpy( &output.columns[ 2 ], &c2, 4 * sizeof( float ) );
    memcpy( &output.columns[ 3 ], &c3, 4 * sizeof( float ) );
}

MetalRenderer::MetalRenderer( CrimildMetalView *view, id< MTLDevice > device )
    : _view( view ),
      _device( device )
{
    float scale = [[UIScreen mainScreen] scale];
    setScreenBuffer( crimild::alloc< FrameBufferObject >( scale * getView().bounds.size.width, scale * getView().bounds.size.height ) );

    setShaderProgram( Renderer::SHADER_PROGRAM_UNLIT_TEXTURE, crimild::alloc< UnlitTextureShaderProgram >() );
    setShaderProgram( Renderer::SHADER_PROGRAM_UNLIT_DIFFUSE, crimild::alloc< UnlitDiffuseShaderProgram >() );

    setShaderProgram( Renderer::SHADER_PROGRAM_LIT_TEXTURE, crimild::alloc< LitTextureShaderProgram >() );

    setShaderProgram( Renderer::SHADER_PROGRAM_RENDER_PASS_STANDARD, crimild::alloc< ForwardShaderProgram >() );
    setShaderProgram( Renderer::SHADER_PROGRAM_RENDER_PASS_FORWARD, crimild::alloc< ForwardShaderProgram >() );
    
    setShaderProgram( Renderer::SHADER_PROGRAM_TEXT_BASIC, crimild::alloc< TextShaderProgram >() );

    setShaderProgram( Renderer::SHADER_PROGRAM_SCREEN_TEXTURE, crimild::alloc< ScreenTextureShaderProgram >() );
}

MetalRenderer::~MetalRenderer( void )
{
    _device = nil;
    _view = nil;
    _layer = nil;
    _commandQueue = nil;
}

void MetalRenderer::configure( void )
{
    _layer = (CAMetalLayer *) getView().layer;
    _layer.contentsScale = [[UIScreen mainScreen] scale];
    _layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    _layer.framebufferOnly = true;
    _layer.presentsWithTransaction = false;
    _layer.drawsAsynchronously = true;
    _layer.device = _device;

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
    
    _inflightSemaphore = dispatch_semaphore_create( CRIMILD_METAL_IN_FLIGHT_COMMAND_BUFFERS );
    
    _commandQueue = [getDevice() newCommandQueue];
    
    setVertexBufferObjectCatalog( crimild::alloc< VertexBufferObjectCatalog > ( this ) );
    setIndexBufferObjectCatalog( crimild::alloc< IndexBufferObjectCatalog > ( this ) );
    setShaderProgramCatalog( crimild::alloc< ShaderProgramCatalog > ( this ) );
    setTextureCatalog( crimild::alloc< TextureCatalog > ( this ) );
    setFrameBufferObjectCatalog( crimild::alloc< FrameBufferObjectCatalog >( this ) );
}

void MetalRenderer::setViewport( const Rectf &viewport )
{

}

void MetalRenderer::beginRender( void )
{
    Renderer::beginRender();
    
    dispatch_semaphore_wait( _inflightSemaphore, DISPATCH_TIME_FOREVER );
    
    _commandBuffer = [_commandQueue commandBuffer];
    if ( _commandBuffer == nil ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot obtain command buffer" );
        return;
    }
}

void MetalRenderer::endRender( void )
{
    Renderer::endRender();
}

void MetalRenderer::presentFrame( void )
{
    _drawable = [getLayer() nextDrawable];
    if ( _drawable == nil ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot obtain next drawable" );
        return;
    }
    
    getFrameBufferObjectCatalog()->bind( getScreenBuffer() );

    Renderer::presentFrame();

    getFrameBufferObjectCatalog()->unbind( getScreenBuffer() );
    
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

void MetalRenderer::bindMaterial( ShaderProgram *program, Material *material )
{
    memcpy( &_standardUniforms.material.ambient, material->getAmbient().getData(), 4 * sizeof( float ) );
    memcpy( &_standardUniforms.material.diffuse, material->getDiffuse().getData(), 4 * sizeof( float ) );
    memcpy( &_standardUniforms.material.specular, material->getSpecular().getData(), 4 * sizeof( float ) );

    _standardUniforms.material.shininess = material->getShininess();
    
    setDepthState( material->getDepthState() );
    setAlphaState( material->getAlphaState() );
    setCullFaceState( material->getCullFaceState() );
    
    bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_USE_COLOR_MAP_UNIFORM ), material->getColorMap() != nullptr );
    if ( material->getColorMap() != nullptr ) {
        auto loc = program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_COLOR_MAP_UNIFORM );
        bindTexture( loc, material->getColorMap() );
    }
}

void MetalRenderer::bindLight( ShaderProgram *program, Light *light )
{
    memcpy( &_standardUniforms.lights[ _lightCount ].position, light->getPosition().getData(), 3 * sizeof( float ) );
    memcpy( &_standardUniforms.lights[ _lightCount ].attenuation, light->getAttenuation().getData(), 3 * sizeof( float ) );
    memcpy( &_standardUniforms.lights[ _lightCount ].direction, light->getDirection().getData(), 3 * sizeof( float ) );
    memcpy( &_standardUniforms.lights[ _lightCount ].color, light->getColor().getData(), 4 * sizeof( float ) );
    
    ++_lightCount;
}

void MetalRenderer::unbindLight( ShaderProgram *program, Light *light )
{
    --_lightCount;
}

void MetalRenderer::bindUniform( ShaderLocation *location, int value )
{
    /*
    if ( location == nullptr ) {
        return;
    }
    
    id< MTLBuffer > uniform = [_device newBufferWithBytes: &value
                                                   length: sizeof( int )
                                                  options: MTLResourceCPUCacheModeDefaultCache];
    [getRenderEncoder() setVertexBuffer: uniform offset: 0 atIndex: location->getLocation()];
     */
}

void MetalRenderer::bindUniform( ShaderLocation *location, float value )
{
    /*
    if ( location == nullptr || location->getLocation() < VERTEX_BUFFER_INDEX_UNIFORM_DATA ) {
        return;
    }
    
    id< MTLBuffer > uniform = [_device newBufferWithBytes: &value
                                                   length: sizeof( float )
                                                  options: MTLResourceCPUCacheModeDefaultCache];
    [getRenderEncoder() setVertexBuffer: uniform offset: 0 atIndex: location->getLocation()];
     */

}

void MetalRenderer::bindUniform( ShaderLocation *location, const Vector3f &vector )
{
    /*
    if ( location == nullptr || location->getLocation() < VERTEX_BUFFER_INDEX_UNIFORM_DATA ) {
        return;
    }
    
    id< MTLBuffer > uniform = [_device newBufferWithBytes: vector.getData()
                                                   length: 3 * sizeof( float )
                                                  options: MTLResourceCPUCacheModeDefaultCache];
    [getRenderEncoder() setVertexBuffer: uniform offset: 0 atIndex: location->getLocation()];
     */
}

void MetalRenderer::bindUniform( ShaderLocation *location, const Vector2f &vector )
{
    /*
    if ( location == nullptr || location->getLocation() < VERTEX_BUFFER_INDEX_UNIFORM_DATA ) {
        return;
    }
    
    id< MTLBuffer > uniform = [_device newBufferWithBytes: vector.getData()
                                                   length: 2 * sizeof( float )
                                                  options: MTLResourceCPUCacheModeDefaultCache];
    [getRenderEncoder() setVertexBuffer: uniform offset: 0 atIndex: location->getLocation()];
     */
}

void MetalRenderer::bindUniform( ShaderLocation *location, const RGBAColorf &color )
{
    /*
    if ( location == nullptr || location->getLocation() < VERTEX_BUFFER_INDEX_UNIFORM_DATA ) {
        return;
    }
    
    id< MTLBuffer > uniform = [_device newBufferWithBytes: color.getData()
                                                   length: 4 * sizeof( float )
                                                  options: MTLResourceCPUCacheModeDefaultCache];
    [getRenderEncoder() setVertexBuffer: uniform offset: 0 atIndex: location->getLocation()];
     */
}

void MetalRenderer::bindUniform( ShaderLocation *location, const Matrix4f &matrix )
{
    if ( location == nullptr ) {
        return;
    }
    
    switch ( location->getLocation() ) {
        case MetalStandardLocation::PROJECTION_MATRIX_UNIFORM:
            matrix2float4x4( matrix, _standardUniforms.pMatrix );
            break;
        case MetalStandardLocation::VIEW_MATRIX_UNIFORM:
            matrix2float4x4( matrix, _standardUniforms.vMatrix );
            break;
        case MetalStandardLocation::MODEL_MATRIX_UNIFORM:
            matrix2float4x4( matrix, _standardUniforms.mMatrix );
            break;
    }
}

void MetalRenderer::applyTransformations( ShaderProgram *program, const Matrix4f &projection, const Matrix4f &view, const Matrix4f &model, const Matrix4f &normal )
{
    matrix2float4x4( projection, _standardUniforms.pMatrix );
    matrix2float4x4( view, _standardUniforms.vMatrix );
    matrix2float4x4( model, _standardUniforms.mMatrix );
}

void MetalRenderer::applyTransformations( ShaderProgram *program, const Matrix4f &projection, const Matrix4f &view, const Matrix4f &model )
{
    matrix2float4x4( projection, _standardUniforms.pMatrix );
    matrix2float4x4( view, _standardUniforms.vMatrix );
    matrix2float4x4( model, _standardUniforms.mMatrix );
}

void MetalRenderer::drawPrimitive( ShaderProgram *program, Primitive *primitive )
{
    auto location = program->getLocation( "uniforms" );
    
    _standardUniforms.lightCount = _lightCount;

    auto uniforms = [_device newBufferWithLength: sizeof( MetalStandardUniforms ) options:MTLResourceCPUCacheModeDefaultCache];
    void *uniformData = [uniforms contents];
    memcpy( uniformData, &_standardUniforms, sizeof( MetalStandardUniforms ) );
    
    [getRenderEncoder() setVertexBuffer: uniforms offset: 0 atIndex: location->getLocation()];
    [getRenderEncoder() setFragmentBuffer: uniforms offset: 0 atIndex: location->getLocation()];

    auto indexCount = primitive->getIndexBuffer()->getIndexCount();
    auto indexBuffer = static_cast< IndexBufferObjectCatalog * >( getIndexBufferObjectCatalog() )->getMetalIndexBuffer( primitive->getIndexBuffer() );
    
    [getRenderEncoder() drawIndexedPrimitives: MTLPrimitiveTypeTriangle
                                   indexCount: indexCount
                                    indexType: MTLIndexTypeUInt16
                                  indexBuffer: indexBuffer
                            indexBufferOffset: 0];
}

void MetalRenderer::drawBuffers( ShaderProgram *program, Primitive::Type bufferType, VertexBufferObject *vbo, unsigned int count )
{

}

void MetalRenderer::setAlphaState( AlphaState *state )
{

}

void MetalRenderer::setDepthState( DepthState *state )
{
    MTLDepthStencilDescriptor *depthStencilDescriptor = [MTLDepthStencilDescriptor new];
    depthStencilDescriptor.depthCompareFunction = MTLCompareFunctionLess;
    depthStencilDescriptor.depthWriteEnabled = state->isEnabled() ? YES : NO;

    auto depthStencilState = [_device newDepthStencilStateWithDescriptor:depthStencilDescriptor];
    
    [getRenderEncoder() setDepthStencilState: depthStencilState];
}

void MetalRenderer::setCullFaceState( CullFaceState *state )
{
    [getRenderEncoder() setFrontFacingWinding: MTLWindingCounterClockwise];
    [getRenderEncoder() setCullMode: MTLCullModeBack];
}

void MetalRenderer::setColorMaskState( ColorMaskState *state )
{
    // TODO
}

#endif // TARGET_OS_IOS

