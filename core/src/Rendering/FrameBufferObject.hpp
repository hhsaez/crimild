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

#ifndef CRIMILD_RENDERING_FRAME_BUFFER_OBJECT_
#define CRIMILD_RENDERING_FRAME_BUFFER_OBJECT_

#include "Catalog.hpp"
#include "Texture.hpp"

#include "Mathematics/Vector.hpp"
#include "Foundation/SharedObjectMap.hpp"

#include <memory>

namespace crimild {
    
    class RenderTarget : public SharedObject {
    public:
        enum class Type {
            COLOR_RGB,
            COLOR_RGBA,
            DEPTH_16,
            DEPTH_24,
            DEPTH_32
        };
        
        enum class Output {
            RENDER,
            TEXTURE,
            RENDER_AND_TEXTURE
        };
        
    public:
        RenderTarget( Type type, Output output, int width, int height );
        RenderTarget( Type type, Output output, int width, int height, bool floatTextureHint );
        virtual ~RenderTarget( void );
        
        Type getType( void ) const { return _type; }
        Output getOutput( void ) const { return _output; }
        
        int getId( void ) const { return _id; }
        void setId( int value ) { _id = value; }
        
        int getWidth( void ) const { return _width; }
        int getHeight( void ) const { return _height; }
        
        Texture *getTexture( void ) { return crimild::get_ptr( _texture ); }
        bool useFloatTexture( void ) const { return _useFloatTexture; }
        void setUseFloatTexture( bool value ) { _useFloatTexture = value; }
        
    private:
        int _id;
        Type _type;
        Output _output;
        int _width;
        int _height;
        SharedPointer< Texture > _texture;
        bool _useFloatTexture = false;
    };
    
    using RenderTargetMap = SharedObjectMap< RenderTarget >;

	class FrameBufferObject : public Catalog< FrameBufferObject >::Resource {
	public:
        FrameBufferObject( int width, int height );
		virtual ~FrameBufferObject( void );

		int getWidth( void ) const { return _width; }
		int getHeight( void ) const { return _height; }

        float getAspectRatio( void ) const { return ( float ) _width / ( float ) _height; }

		void setClearColor( const RGBAColorf &color ) { _clearColor = color; }
		const RGBAColorf &getClearColor( void ) const { return _clearColor; }

        RenderTargetMap &getRenderTargets( void ) { return _renderTargets; }
        
	private:
		int _width;
		int _height;
		RGBAColorf _clearColor;
        RenderTargetMap _renderTargets;
	};

}

#endif

