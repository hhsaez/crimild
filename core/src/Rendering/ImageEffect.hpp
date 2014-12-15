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
    class FrameBufferObject;
    class Primitive;
    class Texture;
    class ShaderProgram;

	class ImageEffect : public SharedObject {
    protected:
        ImageEffect( void );

	public:
		virtual ~ImageEffect( void );

        virtual void apply( std::shared_ptr< Renderer > const &renderer, int inputCount, Texture **inputs, std::shared_ptr< Primitive > const &primitive, std::shared_ptr< FrameBufferObject > const &output ) = 0;
        
    protected:
        void render( std::shared_ptr< Renderer > const &renderer, std::shared_ptr< FrameBufferObject > const &output, std::shared_ptr< Texture > const &texture, std::shared_ptr< ShaderProgram > const &program, std::shared_ptr< Primitive > const &primitive );
	};
    
    using ImageEffectPtr = std::shared_ptr< ImageEffect >;

}

#endif

