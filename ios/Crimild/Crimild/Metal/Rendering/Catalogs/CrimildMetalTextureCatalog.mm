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

#import "CrimildMetalTextureCatalog.h"

#import "CrimildMetalRenderer.h"

#if !TARGET_OS_SIMULATOR

using namespace crimild;
using namespace crimild::metal;

TextureCatalog::TextureCatalog( MetalRenderer *renderer )
    : _nextBufferId( 0 ),
      _renderer( renderer )
{
    
}

TextureCatalog::~TextureCatalog( void )
{
    
}

int TextureCatalog::getNextResourceId( void )
{
    int bufferId = _nextBufferId++;
    return bufferId;
}

void TextureCatalog::bind( ShaderLocation *location, Texture *texture )
{
    Catalog< Texture >::bind( texture );
    
    auto mtlTexture = _textures[ texture->getCatalogId() ];
    [getRenderer()->getRenderEncoder() setFragmentTexture:mtlTexture atIndex: _activeTextureCount];
    
    ++_activeTextureCount;
}

void TextureCatalog::unbind( ShaderLocation *location, Texture *texture )
{
    Catalog< Texture >::unbind( texture );

    if ( _activeTextureCount > 0 ) {
        --_activeTextureCount;
    }
}

void TextureCatalog::load( Texture *texture )
{
    Catalog< Texture >::load( texture );
    
    auto image = texture->getImage();
    
    MTLPixelFormat pixelFormat;
    switch ( image->getBpp() ) {
        case 1:
            pixelFormat = MTLPixelFormatR8Unorm;
            break;
            
        default:
            pixelFormat = MTLPixelFormatRGBA8Unorm;
            break;
    }
    
    MTLTextureDescriptor *textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat: pixelFormat
                                                                                                 width: image->getWidth()
                                                                                                height: image->getHeight()
                                                                                             mipmapped: NO];
    
    id< MTLTexture > mtlTexture = [getRenderer()->getDevice() newTextureWithDescriptor:textureDescriptor];
    
    MTLRegion region = MTLRegionMake2D( 0, 0, image->getWidth(), image->getHeight() );
    
    if ( image->getBpp() == 3 ) {
        std::vector< unsigned char > data( image->getWidth() * image->getHeight() * 4 );
        for ( unsigned int i = 0; i < image->getWidth() * image->getHeight(); i++ ) {
            data[ 4 * i + 0 ] = image->getData()[ image->getBpp() * i + 0 ];
            data[ 4 * i + 1 ] = image->getData()[ image->getBpp() * i + 1 ];
            data[ 4 * i + 2 ] = image->getData()[ image->getBpp() * i + 2 ];
            data[ 4 * i + 3 ] = 255;
        }
        auto bytesPerRow = image->getWidth() * 4;
        [mtlTexture replaceRegion: region mipmapLevel: 0 withBytes: &data[ 0 ] bytesPerRow: bytesPerRow];
    }
    else {
        [mtlTexture replaceRegion: region mipmapLevel: 0 withBytes: image->getData() bytesPerRow: image->getWidth() * image->getBpp()];
    }

    _textures[ texture->getCatalogId() ] = mtlTexture;
    
}

void TextureCatalog::unload( Texture *texture )
{
    _textures[ texture->getCatalogId() ] = nullptr;
    
    Catalog< Texture >::unload( texture );
}

void TextureCatalog::cleanup( void )
{
    Catalog< Texture >::cleanup();
}

id< MTLTexture > TextureCatalog::generateRenderTargetTexture( RenderTarget *renderTarget )
{
    MTLTextureDescriptor *textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat: MTLPixelFormatBGRA8Unorm
                                                                                                 width: renderTarget->getWidth()
                                                                                                height: renderTarget->getHeight()
                                                                                             mipmapped: NO];
    
    id< MTLTexture > mtlTexture = [getRenderer()->getDevice() newTextureWithDescriptor:textureDescriptor];
    
    auto texture = renderTarget->getTexture();
    Catalog< Texture >::load( texture );
    _textures[ texture->getCatalogId() ] = mtlTexture;
    
    return mtlTexture;
}

#endif // TARGET_OS_SIMULATOR

