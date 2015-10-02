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

#include "Utils.hpp"

#if 0

#ifdef __APPLE__
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

using namespace crimild;

void gles::Utils::checkErrors( std::string prefix )
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

                /*
            case GL_STACK_OVERFLOW:
                errorDescription += "GL_STACK_OVERFLOW";
                break;
                
            case GL_STACK_UNDERFLOW:
                errorDescription += "GL_STACK_UNDERFLOW";
                break;
                 */
                
            case GL_OUT_OF_MEMORY:
                errorDescription += "GL_OUT_OF_MEMORY";
                break;
                
            default:
                errorDescription += "Unknown Error";
                break;
        }
        
        Log::Error << prefix << ": " << "(0x" << error << ") " << errorDescription << Log::End;
    }
}

VertexShaderPtr gles::Utils::getVertexShaderInstance( std::string source )
{
    return crimild::alloc< VertexShader >( source );
}

FragmentShaderPtr gles::Utils::getFragmentShaderInstance( std::string source )
{
    return crimild::alloc< FragmentShader >( source );
}

std::string gles::Utils::buildArrayShaderLocationName( std::string variable, int index )
{
    std::stringstream str;
    str << variable << "[" << index << "]";
    return str.str();
}

std::string gles::Utils::buildArrayShaderLocationName( std::string variable, int index, std::string member )
{
    std::stringstream str;
    str << variable << "[" << index << "]." << member;
    return str.str();
}

#endif

