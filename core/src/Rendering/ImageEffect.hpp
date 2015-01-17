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

#ifndef CRIMILD_RENDERING_IMAGE_EFFECT_
#define CRIMILD_RENDERING_IMAGE_EFFECT_

#include "Foundation/SharedObject.hpp"

namespace crimild {
    
    class Renderer;
    class Camera;
    class FrameBufferObject;
    class Texture;

	class ImageEffect : public SharedObject {
    public:
        static constexpr const char *FBO_AUX_1 = "aux_buffer_1";
        static constexpr const char *FBO_AUX_2 = "aux_buffer_2";
        
        static constexpr const char *FBO_AUX_HALF_RES_1 = "aux_half_buffer_1";
        static constexpr const char *FBO_AUX_HALF_RES_2 = "aux_half_buffer_2";
        
        static constexpr const char *FBO_AUX_QUARTER_RES_1 = "aux_quarter_buffer_1";
        static constexpr const char *FBO_AUX_QUARTER_RES_2 = "aux_quarter_buffer_2";

    private:
        using RendererPtrImpl = SharedPointer< Renderer >;
        using CameraPtrImpl = SharedPointer< Camera >;
        using FrameBufferObjectPtrImpl = SharedPointer< FrameBufferObject >;
        using TexturePtrImpl = SharedPointer< Texture >;

    protected:
        ImageEffect( void );

	public:
		virtual ~ImageEffect( void );

        virtual void compute( RendererPtrImpl const &renderer, CameraPtrImpl const &camera ) = 0;
        virtual void apply( RendererPtrImpl const &renderer, CameraPtrImpl const &camera ) = 0;
        
    protected:
        virtual FrameBufferObjectPtrImpl getFrameBuffer( RendererPtrImpl const &renderer, std::string name );
        
        virtual void renderScreen( RendererPtrImpl const &renderer, TexturePtrImpl const &texture );
	};
    
    using ImageEffectPtr = SharedPointer< ImageEffect >;

}

#endif

