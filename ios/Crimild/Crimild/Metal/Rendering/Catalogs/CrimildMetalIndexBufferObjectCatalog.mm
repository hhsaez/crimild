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

#import "CrimildMetalIndexBufferObjectCatalog.h"

#import "CrimildMetalRenderer.h"

using namespace crimild;
using namespace crimild::metal;

IndexBufferObjectCatalog::IndexBufferObjectCatalog( MetalRenderer *renderer )
: _nextBufferId( 0 ),
_renderer( renderer )
{
    
}

IndexBufferObjectCatalog::~IndexBufferObjectCatalog( void )
{
    
}

int IndexBufferObjectCatalog::getNextResourceId( void )
{
    int bufferId = _nextBufferId++;
    return bufferId;
}

void IndexBufferObjectCatalog::bind( ShaderProgram *program, IndexBufferObject *ibo )
{
    Catalog< IndexBufferObject >::bind( ibo );
//    
//    [getRenderer()->getRenderEncoder() setVertexBuffer: _ibos[ ibo->getCatalogId() ]
//                                                offset: 0
//                                               atIndex: 1];
}

void IndexBufferObjectCatalog::unbind( ShaderProgram *program, IndexBufferObject *vbo )
{
    Catalog< IndexBufferObject >::unbind( vbo );
}

void IndexBufferObjectCatalog::load( IndexBufferObject *ibo )
{
    Catalog< IndexBufferObject >::load( ibo );
    
    id< MTLBuffer > indexArray = [getRenderer()->getDevice() newBufferWithBytes: ibo->getData()
                                                                         length: ibo->getSizeInBytes()
                                                                        options: MTLResourceOptionCPUCacheModeDefault];
    
    _ibos[ ibo->getCatalogId() ] = indexArray;
    
}

void IndexBufferObjectCatalog::unload( IndexBufferObject *ibo )
{
    _ibos[ ibo->getCatalogId() ] = nullptr;
    
    Catalog< IndexBufferObject >::unload( ibo );
}

void IndexBufferObjectCatalog::cleanup( void )
{
    // TODO
}

id< MTLBuffer > IndexBufferObjectCatalog::getMetalIndexBuffer( crimild::IndexBufferObject *ibo )
{
    return _ibos[ ibo->getCatalogId() ];
}

