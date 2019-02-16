/*
 * Copyright (c) 2002 - present, H. Hernan Saez
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

#include "OpenGLUtils.hpp"

#include <Foundation/Log.hpp>

#include <sstream>

using namespace crimild;
using namespace crimild::opengl;

const GLenum OpenGLUtils::PRIMITIVE_TYPE[] = {
	GL_POINTS,
	GL_LINES,
	GL_LINE_LOOP,
	GL_LINE_STRIP,
	GL_TRIANGLES,
	GL_TRIANGLE_STRIP,
	GL_TRIANGLE_FAN
};

const GLenum OpenGLUtils::TEXTURE_TARGET[] = {
	GL_TEXTURE_2D,
	GL_TEXTURE_CUBE_MAP
};

const GLenum OpenGLUtils::TEXTURE_WRAP_MODE_CLAMP[] = {
	GL_REPEAT,
	GL_CLAMP_TO_EDGE,
};

const GLenum OpenGLUtils::TEXTURE_FILTER_MAP[] = {
    GL_NEAREST,
    GL_LINEAR,
    GL_NEAREST_MIPMAP_NEAREST,
    GL_NEAREST_MIPMAP_LINEAR,
    GL_LINEAR_MIPMAP_NEAREST,
    GL_LINEAR_MIPMAP_LINEAR
};

const GLenum OpenGLUtils::ALPHA_SRC_BLEND_FUNC[] = {
	GL_ZERO,
	GL_ONE,
	GL_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_DST_COLOR,
	GL_ONE_MINUS_DST_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_SRC_ALPHA_SATURATE
};

const GLenum OpenGLUtils::ALPHA_DST_BLEND_FUNC[] = {
	GL_ZERO,
	GL_ONE,
	GL_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA
};

const GLenum OpenGLUtils::DEPTH_COMPARE_FUNC[] = {
    GL_NEVER,
    GL_LESS,
    GL_EQUAL,
    GL_LEQUAL,
    GL_GREATER,
    GL_NOTEQUAL,
    GL_GEQUAL,
    GL_ALWAYS
};

const GLenum OpenGLUtils::CULL_FACE_MODE[] = {
	GL_BACK,
	GL_FRONT,
	GL_FRONT_AND_BACK
};

void OpenGLUtils::checkErrors( std::string prefix )
{
    for ( GLint error = glGetError(); error; error = glGetError() ) {

    	std::string errorDescription;
    	switch (error) {
			case GL_INVALID_ENUM:
				errorDescription += "GL_INVALID_ENUM";
				break;

			case GL_INVALID_VALUE:
				errorDescription += "GL_INVALID_VALUE";
				break;

			case GL_INVALID_OPERATION:
				errorDescription += "GL_INVALID_OPERATION";
				break;

#ifdef GL_STACK_OVERFLOW
			case GL_STACK_OVERFLOW:
				errorDescription += "GL_STACK_OVERFLOW";  
				break;
#endif

#ifdef GL_STACK_UNDERFLOW
			case GL_STACK_UNDERFLOW: 
				errorDescription += "GL_STACK_UNDERFLOW";
				break;
#endif
			case GL_OUT_OF_MEMORY: 
				errorDescription += "GL_OUT_OF_MEMORY";
				break;

			default:
				errorDescription += "Unknown Error";
				break;
    	}

    	Log::error( CRIMILD_CURRENT_CLASS_NAME, prefix,
                   ": (0x", error, ") ",
                   errorDescription
#ifdef CRIMILD_PLATFORM_DESKTOP
                   ," ", glewGetErrorString( error )
#endif
                   );
    }
}

std::string OpenGLUtils::buildArrayShaderLocationName( std::string variable, int index )
{
	std::stringstream str;
	str << variable << "[" << index << "]";
	return str.str();
}

std::string OpenGLUtils::buildArrayShaderLocationName( std::string variable, int index, std::string member )
{
	std::stringstream str;
	str << variable << "[" << index << "]." << member;
	return str.str();
}

