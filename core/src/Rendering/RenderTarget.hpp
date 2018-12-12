/*
 * Copyright (c) 2013-2018, Hernan Saez
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

#ifndef CRIMILD_RENDERING_RENDER_TARGET_
#define CRIMILD_RENDERING_RENDER_TARGET_

#include "Catalog.hpp"

#include "Mathematics/Vector.hpp"
#include "Foundation/Containers/Map.hpp"

#include <memory>

namespace crimild {

	class Texture;

    class RenderTarget : public SharedObject, public Catalog< RenderTarget >::Resource {
    public:
		static constexpr const char *RENDER_TARGET_NAME_COLOR = "fbo_color";
		static constexpr const char *RENDER_TARGET_NAME_DEPTH = "fbo_depth";
		
        enum class Type {
            COLOR_RGB,
            COLOR_RGBA,
            DEPTH_16,
            DEPTH_24,
            DEPTH_32
        };

        class Output {
        public:
            enum {
                RENDER = 1 << 0,
                TEXTURE = 1 << 1,
                RENDER_AND_TEXTURE = RENDER | TEXTURE,
            };
        };

    public:
        RenderTarget( Type type, crimild::Int8 output, int width, int height );
        RenderTarget( Type type, crimild::Int8 output, int width, int height, bool floatTextureHint );
        virtual ~RenderTarget( void );
        
        Type getType( void ) const { return _type; }
        crimild::Int8 getOutput( void ) const { return _output; }
        
        int getWidth( void ) const { return _width; }
        int getHeight( void ) const { return _height; }
        
        Texture *getTexture( void ) { return crimild::get_ptr( _texture ); }
        bool useFloatTexture( void ) const { return _useFloatTexture; }
        void setUseFloatTexture( bool value ) { _useFloatTexture = value; }
        
    private:
        Type _type;
        crimild::Int8 _output;
        int _width;
        int _height;
        SharedPointer< Texture > _texture;
        bool _useFloatTexture = false;
    };

}

#endif

