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

#ifndef CRIMILD_GL3_RENDERING_IMAGE_EFFECT_DEPTH_OF_FIELD_
#define CRIMILD_GL3_RENDERING_IMAGE_EFFECT_DEPTH_OF_FIELD_

#include <Crimild.hpp>

namespace crimild {
    
    namespace opengl {
        
        class DepthOfFieldImageEffect : public ImageEffect {
        public:
            class DoFBlurShaderProgram : public ShaderProgram {
            public:
                DoFBlurShaderProgram( void );
                virtual ~DoFBlurShaderProgram( void );
                
                void setOrientation( int value ) { _uOrientation->setValue( value ); }
                int getOrientation( void ) { return _uOrientation->getValue(); }
                
                void setTexelSize( const Vector2f &value ) { _uTexelSize->setValue( value ); }
                Vector2f getTexelSize( void ) { return _uTexelSize->getValue(); }
                
                void setBlurCoefficient( float value ) { _uBlurCoefficient->setValue( value ); }
                float getBlurCoefficient( void ) { return _uBlurCoefficient->getValue(); }
                
                void setFocusDistance( float value ) { _uFocusDistance->setValue( value ); }
                float getFocusDistance( void ) { return _uFocusDistance->getValue(); }
                
                void setPPM( float value ) { _uPPM->setValue( value ); }
                float getPPM( void ) { return _uPPM->getValue(); }
                
                void setNear( float value ) { _uNear->setValue( value ); }
                float getNear( void ) { return _uNear->getValue(); }
                
                void setFar( float value ) { _uFar->setValue( value ); }
                float getFar( void ) { return _uFar->getValue(); }
                
            private:
                SharedPointer< IntUniform > _uOrientation;
                SharedPointer< Vector2fUniform > _uTexelSize;
                SharedPointer< FloatUniform > _uBlurCoefficient;
                SharedPointer< FloatUniform > _uFocusDistance;
                SharedPointer< FloatUniform > _uPPM;
                SharedPointer< FloatUniform > _uNear;
                SharedPointer< FloatUniform > _uFar;
            };
            
            class DoFCompositeShaderProgram : public ShaderProgram {
            public:
                DoFCompositeShaderProgram( void );
                virtual ~DoFCompositeShaderProgram( void );
                
                void setBlurCoefficient( float value ) { _uBlurCoefficient->setValue( value ); }
                float getBlurCoefficient( void ) { return _uBlurCoefficient->getValue(); }
                
                void setFocusDistance( float value ) { _uFocusDistance->setValue( value ); }
                float getFocusDistance( void ) { return _uFocusDistance->getValue(); }
                
                void setPPM( float value ) { _uPPM->setValue( value ); }
                float getPPM( void ) { return _uPPM->getValue(); }
                
                void setNear( float value ) { _uNear->setValue( value ); }
                float getNear( void ) { return _uNear->getValue(); }
                
                void setFar( float value ) { _uFar->setValue( value ); }
                float getFar( void ) { return _uFar->getValue(); }
                
            private:
                SharedPointer< IntUniform > _uOrientation;
                SharedPointer< Vector2fUniform > _uTexelSize;
                SharedPointer< FloatUniform > _uBlurCoefficient;
                SharedPointer< FloatUniform > _uFocusDistance;
                SharedPointer< FloatUniform > _uPPM;
                SharedPointer< FloatUniform > _uNear;
                SharedPointer< FloatUniform > _uFar;
            };
            
        public:
			DepthOfFieldImageEffect( void );
            explicit DepthOfFieldImageEffect( int resolution );
            virtual ~DepthOfFieldImageEffect( void );
            
            virtual void compute( crimild::Renderer *renderer, Camera *camera ) override;
            virtual void apply( crimild::Renderer *renderer, crimild::Camera *camera ) override;

			int getResolution( void ) const { return _resolution; }

            float getFocalDistance( void ) { return _focalDistance->getValue(); }
            void setFocalDistance( float value ) { _focalDistance->setValue( value ); }
            
            float getFocusDistance( void ) { return _focusDistance->getValue(); }
            void setFocusDistance( float value ) { _focusDistance->setValue( value ); }

            float getFStop( void ) { return _fStop->getValue(); }
            void setFStop( float value ) { _fStop->setValue( value ); }
            
            float getAperture( void ) { return _aperture->getValue(); }
            void setAperture( float value ) { _aperture->setValue( value ); }
            
        private:
			int _resolution;
            SharedPointer< FloatUniform > _focalDistance; //< in millimeters
            SharedPointer< FloatUniform > _focusDistance; //< in millimeters
            SharedPointer< FloatUniform > _fStop; //< in millimeters
            SharedPointer< FloatUniform > _aperture; //< in millimeters
            
        private:
            DoFBlurShaderProgram *getBlurProgram( void ) { return crimild::get_ptr( _dofBlurProgram ); }
            DoFCompositeShaderProgram *getCompositeProgram( void ) { return crimild::get_ptr( _dofCompositeProgram ); }
            
        private:
            SharedPointer< DoFBlurShaderProgram > _dofBlurProgram;
            SharedPointer< DoFCompositeShaderProgram > _dofCompositeProgram;
            
        private:
            FrameBufferObject *getAuxFBO( int index ) { return crimild::get_ptr( _auxFBOs[ index ] ); }
            
        private:
            std::vector< SharedPointer< FrameBufferObject >> _auxFBOs;
        };
        
    }
    
}

#endif

