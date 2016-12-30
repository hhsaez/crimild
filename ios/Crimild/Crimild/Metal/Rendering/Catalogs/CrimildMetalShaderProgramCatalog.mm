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

#import "CrimildMetalShaderProgramCatalog.h"

#if !TARGET_OS_SIMULATOR

#import "CrimildMetalRenderer.h"

using namespace crimild;
using namespace crimild::metal;

ShaderProgramCatalog::ShaderProgramCatalog( MetalRenderer *renderer )
    : _nextBufferId( 0 ),
      _renderer( renderer ),
      _defaultLibrary( nil )
{
    
}

ShaderProgramCatalog::~ShaderProgramCatalog( void )
{
    
}

int ShaderProgramCatalog::getNextResourceId( void )
{
    int bufferId = _nextBufferId++;
    return bufferId;
}

id< MTLLibrary > ShaderProgramCatalog::getDefaultLibrary( void )
{
    if ( _defaultLibrary == nil ) {
        _defaultLibrary = [getRenderer()->getDevice() newDefaultLibrary];
        if ( _defaultLibrary == nil ) {
            Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot create default library" );
        }
    }
    
    return _defaultLibrary;
}

void ShaderProgramCatalog::bind( ShaderProgram *program )
{
    Catalog< ShaderProgram >::bind( program );
    
    [getRenderer()->getRenderEncoder() setRenderPipelineState: _pipelines[ program->getCatalogId() ]];
}

void ShaderProgramCatalog::unbind( ShaderProgram *program )
{
    Catalog< ShaderProgram >::unbind( program );
}

void ShaderProgramCatalog::load( ShaderProgram *program )
{
    NSString *vertexProgramName = [NSString stringWithUTF8String: program->getVertexShader()->getSource().c_str()];
    id <MTLFunction> vertexProgram = [getDefaultLibrary() newFunctionWithName: vertexProgramName];
    if ( vertexProgram == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Could not load vertex program named ", [vertexProgramName UTF8String] );
        return;
    }
    
    NSString *fragmentProgramName = [NSString stringWithUTF8String: program->getFragmentShader()->getSource().c_str()];
    id <MTLFunction> fragmentProgram = [getDefaultLibrary() newFunctionWithName: fragmentProgramName];
    if ( fragmentProgram == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Could not load fragment program named ", [fragmentProgramName UTF8String] );
        return;
    }
    
    MTLRenderPipelineDescriptor *desc = [MTLRenderPipelineDescriptor new];
    desc.sampleCount = 1;
    desc.vertexFunction = vertexProgram;
    desc.fragmentFunction = fragmentProgram;
    desc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    
    MTLVertexDescriptor* vertexDesc = [[MTLVertexDescriptor alloc] init];
    vertexDesc.attributes[0].format = MTLVertexFormatFloat3;
    vertexDesc.attributes[0].bufferIndex = 0;
    vertexDesc.attributes[0].offset = VertexFormat::VF_P3_N3_UV2.getPositionsOffset() * sizeof( float );;
    vertexDesc.attributes[1].format = MTLVertexFormatFloat3;
    vertexDesc.attributes[1].bufferIndex = 0;
    vertexDesc.attributes[1].offset = VertexFormat::VF_P3_N3_UV2.getNormalsOffset() * sizeof( float );
    vertexDesc.attributes[2].format = MTLVertexFormatFloat2;
    vertexDesc.attributes[2].bufferIndex = 0;
    vertexDesc.attributes[2].offset = VertexFormat::VF_P3_N3_UV2.getTextureCoordsOffset() * sizeof( float );
    vertexDesc.layouts[0].stride = VertexFormat::VF_P3_N3_UV2.getVertexSize() * sizeof( float );
    vertexDesc.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
    desc.vertexDescriptor = vertexDesc;
    
    NSError *error = nullptr;
    id<MTLRenderPipelineState> renderPipeline = [getRenderer()->getDevice() newRenderPipelineStateWithDescriptor: desc error: &error];
    if ( renderPipeline == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Couldn't create render pipeline: ", [[error description] UTF8String] );
        return;
    }
    
    Catalog< ShaderProgram >::load( program );
    
    _pipelines[ program->getCatalogId() ] = renderPipeline;
}

void ShaderProgramCatalog::unload( ShaderProgram *program )
{
    _pipelines[ program->getCatalogId() ] = nullptr;
    
    Catalog< ShaderProgram >::unload( program );    
}

void ShaderProgramCatalog::cleanup( void )
{
    Catalog< ShaderProgram >::cleanup();
}

#endif // TARGET_OS_SIMULATOR

