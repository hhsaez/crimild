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

#include "ShaderProgramCatalog.hpp"
#include "Rendering/OpenGLUtils.hpp"

#include <Foundation/Log.hpp>
#include <Rendering/ShaderProgram.hpp>

using namespace crimild;
using namespace crimild::opengl;

ShaderProgramCatalog::ShaderProgramCatalog( void )
{

}

ShaderProgramCatalog::~ShaderProgramCatalog( void )
{

}

int ShaderProgramCatalog::getNextResourceId( ShaderProgram * )
{
	return glCreateProgram();
}

void ShaderProgramCatalog::bind( ShaderProgram *program )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	Catalog< ShaderProgram >::bind( program );

	glUseProgram( program->getCatalogId() );

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void ShaderProgramCatalog::unbind( ShaderProgram *program )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	Catalog< ShaderProgram >::unbind( program );

	glUseProgram( 0 );

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void ShaderProgramCatalog::load( ShaderProgram *program )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	Catalog< ShaderProgram >::load( program );

	int programId = program->getCatalogId();
	if ( programId > 0 ) {
		int vsId = compileShader( program->getVertexShader(), GL_VERTEX_SHADER );
		int fsId = compileShader( program->getFragmentShader(), GL_FRAGMENT_SHADER );

        if ( vsId > 0 && fsId > 0 ) {
    		glAttachShader( programId, vsId );
    		glAttachShader( programId, fsId );

            glLinkProgram( programId );

            glDetachShader( programId, vsId );
            glDeleteShader( vsId );

            glDetachShader( programId, fsId );
            glDeleteShader( fsId );

            GLint linkStatus = GL_FALSE;
            glGetProgramiv( programId, GL_LINK_STATUS, &linkStatus );
            if ( linkStatus == GL_FALSE ) {
                GLint bufLength = 0;
                glGetProgramiv( programId, GL_INFO_LOG_LENGTH, &bufLength );
                if ( bufLength ) {
                    char *buf = ( char * ) malloc( bufLength );
                    if ( buf ) {
                        glGetProgramInfoLog( programId, bufLength, NULL, buf );
                        Log::fatal( CRIMILD_CURRENT_CLASS_NAME, "Could not link shader program. Reason: ", buf );
                        free( buf );
                        exit( 1 );
                    }
                }

                glDeleteProgram( programId );
                programId = 0;
            }

            program->forEachLocation( [&]( ShaderLocation *loc ) mutable {
                switch ( loc->getType() ) {
					case ShaderLocation::Type::ATTRIBUTE:
						fetchAttributeLocation( program, loc );
						break;

					case ShaderLocation::Type::UNIFORM:
						fetchUniformLocation( program, loc );
						break;

					case ShaderLocation::Type::UNIFORM_BLOCK:
						fetchUniformBlockLocation( program, loc );
						break;

					default:
						break;
				}
            });
        }
	}

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void ShaderProgramCatalog::unload( ShaderProgram *program )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	int programId = program->getCatalogId();
	if ( programId > 0 ) {
        _shaderIdsToDelete.push_back( programId );
	}

	Catalog< ShaderProgram >::unload( program );

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void ShaderProgramCatalog::cleanup( void )
{
    for ( auto id : _shaderIdsToDelete ) {
        glDeleteProgram( id );
    }
    _shaderIdsToDelete.clear();
}

int ShaderProgramCatalog::compileShader( Shader *shader, int type )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	GLuint shaderId = glCreateShader( type );
    if ( shaderId > 0 ) {
		const char *src = shader->getSource().c_str();
        const GLint srcLength = ( GLint ) shader->getSource().length();
		glShaderSource( shaderId, 1, ( const GLchar ** ) &src, &srcLength );
        glCompileShader( shaderId );

        GLint compiled = GL_FALSE;
        glGetShaderiv( shaderId, GL_COMPILE_STATUS, &compiled );
    
        if ( compiled == GL_FALSE ) {
            GLint infoLen = 0;
            glGetShaderiv( shaderId, GL_INFO_LOG_LENGTH, &infoLen );
            if ( infoLen ) {
                char* buf = ( char * ) malloc( infoLen );
				if ( buf ) {
                    glGetShaderInfoLog( shaderId, infoLen, NULL, buf );
                    Log::fatal( CRIMILD_CURRENT_CLASS_NAME, "Could not compile ",
                               ( type == GL_VERTEX_SHADER ? "vertex" : "fragment" ), " shader. ",
					            "\nSource: ", src,
					            "\nReason: ", buf );
                    free( buf );
                    exit( 1 );
                }
            }

            glDeleteShader( shaderId );
            shaderId = 0;
        }
    }
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;

    return shaderId;
}

void ShaderProgramCatalog::fetchAttributeLocation( ShaderProgram *program, ShaderLocation *location )
{
	location->setLocation( glGetAttribLocation( program->getCatalogId(), location->getName().c_str() ) );
}

void ShaderProgramCatalog::fetchUniformLocation( ShaderProgram *program, ShaderLocation *location )
{
	location->setLocation( glGetUniformLocation( program->getCatalogId(), location->getName().c_str() ) );
}

void ShaderProgramCatalog::fetchUniformBlockLocation( ShaderProgram *program, ShaderLocation *location )
{
    auto value = glGetUniformBlockIndex( program->getCatalogId(), location->getName().c_str() );
	location->setLocation( value );
}

