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

#ifndef CRIMILD_OPENGL_RENDERING_UTILS_
#define CRIMILD_OPENGL_RENDERING_UTILS_

#include <Foundation/Macros.hpp>
#include <Rendering/Shader.hpp>

#ifdef CRIMILD_PLATFORM_DESKTOP
    #define GLEW_STATIC 1
    #include <GL/glew.h>
    #if defined( CRIMILD_ENABLE_GLEW )
        #include <GLFW/glfw3.h>
    #elif defined( CRIMILD_PLATFORM_WIN32 )
		#include <GL/gl.h>
	#else
        #include <OpenGL/gl.h>
		#include <OpenGL/glext.h>
	#endif
#elif defined( CRIMILD_PLATFORM_EMSCRIPTEN )
    #include <GLES3/gl3.h>
#elif defined( __APPLE__ )
    #import <OpenGLES/ES3/gl.h>
    #import <OpenGLES/ES3/glext.h>
#else
    #include <GLES2/gl2.h>
#endif

#if defined( __ANDROID__ ) || defined( CRIMILD_PLATFORM_EMSCRIPTEN )
#define CRIMILD_FORCE_OPENGL_COMPATIBILITY_MODE 1
#endif

namespace crimild {
    
	namespace opengl {

		class OpenGLUtils {
		public:
			static void checkErrors( std::string prefix );

			static SharedPointer< VertexShader > getVertexShaderInstance( std::string source );
			
			static SharedPointer< FragmentShader > getFragmentShaderInstance( std::string source );

			static std::string buildArrayShaderLocationName( std::string variable, int index );

			static std::string buildArrayShaderLocationName( std::string variable, int index, std::string member );

        public:
            static const GLenum PRIMITIVE_TYPE[];
			static const GLenum TEXTURE_TARGET[];
            static const GLenum TEXTURE_WRAP_MODE_CLAMP[];
            static const GLenum TEXTURE_FILTER_MAP[];
            static const GLenum ALPHA_SRC_BLEND_FUNC[];
            static const GLenum ALPHA_DST_BLEND_FUNC[];
            static const GLenum DEPTH_COMPARE_FUNC[];
            static const GLenum CULL_FACE_MODE[];
		};

	}

}

#ifndef CRIMILD_ENABLE_CHECK_GL_ERRORS
#define CRIMILD_ENABLE_CHECK_GL_ERRORS 1
#endif

#if CRIMILD_ENABLE_CHECK_GL_ERRORS
    #define CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION crimild::opengl::OpenGLUtils::checkErrors( std::string( "Before " ) + CRIMILD_CURRENT_FUNCTION );
    #define CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION crimild::opengl::OpenGLUtils::checkErrors( std::string( "After " ) + CRIMILD_CURRENT_FUNCTION );
#else
    #define CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION
    #define CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION
#endif

#endif

