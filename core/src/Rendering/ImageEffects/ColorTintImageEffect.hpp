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

#ifndef CRIMILD_CORE_RENDERING_IMAGE_EFFECT_COLOR_TINT_
#define CRIMILD_CORE_RENDERING_IMAGE_EFFECT_COLOR_TINT_

#include "ImageEffect.hpp"

#include "Mathematics/Vector.hpp"
#include "Rendering/ShaderUniformImpl.hpp"

namespace crimild {
    
	class ColorTintImageEffect : public ImageEffect {
	public:
		static constexpr const char *COLOR_TINT_PROGRAM_NAME = "image_effects/shaders/color_tint";
		static constexpr const char *COLOR_TINT_UNIFORM_TINT = "uTint";
        static constexpr const char *COLOR_TINT_UNIFORM_TINT_VALUE = "uTintValue";

		static const RGBAColorf TINT_RED;
        static const RGBAColorf TINT_GREEN;
        static const RGBAColorf TINT_BLUE;
        static const RGBAColorf TINT_SEPIA;

	public:
		ColorTintImageEffect( const RGBAColorf &tint = TINT_SEPIA, float value = 1.0f );
		virtual ~ColorTintImageEffect( void );
            
		virtual void compute( crimild::Renderer *renderer, Camera *camera ) override;
		virtual void apply( crimild::Renderer *renderer, crimild::Camera *camera ) override;

		void setTint( const RGBAColorf &tint ) { _tint->setValue( tint ); }
		const RGBAColorf &getTint( void ) const { return _tint->getValue(); }
            
        void setTintValue( float value ) { _tintValue->setValue( value ); }
        float getTintValue( void ) const { return _tintValue->getValue(); }
        
	private:
		SharedPointer< RGBAColorfUniform > _tint;
        SharedPointer< FloatUniform > _tintValue;
		
		ShaderProgram *_colorTintProgram = nullptr;
	};
            
}

#endif

