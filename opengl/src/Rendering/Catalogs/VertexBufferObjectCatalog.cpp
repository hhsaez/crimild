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

#include "VertexBufferObjectCatalog.hpp"

#include "Rendering/OpenGLUtils.hpp"

using namespace crimild;
using namespace crimild::opengl;

VertexBufferObjectCatalog::VertexBufferObjectCatalog( void )
{

}

VertexBufferObjectCatalog::~VertexBufferObjectCatalog( void )
{

}

int VertexBufferObjectCatalog::getNextResourceId( void )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
	GLuint vaoId;
	glGenVertexArrays( 1, &vaoId );

	glBindVertexArray( vaoId );

	GLuint vboId;    
    glGenBuffers( 1, &vboId );
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;

    return composeId( vaoId, vboId );
}

int VertexBufferObjectCatalog::composeId( unsigned int vaoId, unsigned int vboId )
{
	return vaoId * 1000 + vboId;
}

bool VertexBufferObjectCatalog::extractId( int compositeId, unsigned int &vaoId, unsigned int &vboId )
{
	vaoId = compositeId / 1000;
	vboId = compositeId % 1000;
	return true;
}

void VertexBufferObjectCatalog::bind( ShaderProgram *program, VertexBufferObject *vbo )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	GLuint vaoId, vboId;

	if ( vbo->getCatalog() == nullptr ) {
		Catalog< VertexBufferObject >::bind( program, vbo );
    }

    extractId( vbo->getCatalogId(), vaoId, vboId );

    glBindVertexArray( vaoId );

    glBindBuffer( GL_ARRAY_BUFFER, vboId );
    float *baseOffset = 0;

    const VertexFormat &format = vbo->getVertexFormat();

    auto positionLocation = program->getStandardLocation( ShaderProgram::StandardLocation::POSITION_ATTRIBUTE );
    if ( positionLocation && positionLocation->isValid() ) {
        if ( format.hasPositions() ) {
            glEnableVertexAttribArray( positionLocation->getLocation() );
            glVertexAttribPointer( positionLocation->getLocation(),
                                   format.getPositionComponents(),
                                   GL_FLOAT,
                                   GL_FALSE,
                                   format.getVertexSizeInBytes(),
                                   ( const GLvoid * )( baseOffset + format.getPositionsOffset() ) );
        }
    }

    auto normalLocation = program->getStandardLocation( ShaderProgram::StandardLocation::NORMAL_ATTRIBUTE );
    if ( normalLocation && normalLocation->isValid() ) {
        if ( format.hasNormals() ) {
            glEnableVertexAttribArray( normalLocation->getLocation() );
            glVertexAttribPointer( normalLocation->getLocation(),
                                   format.getNormalComponents(),
                                   GL_FLOAT,
                                   GL_FALSE,
                                   format.getVertexSizeInBytes(),
                                   ( const GLvoid * )( baseOffset + format.getNormalsOffset() ) );
        }
    }

    auto tangentLocation = program->getStandardLocation( ShaderProgram::StandardLocation::TANGENT_ATTRIBUTE );
    if ( tangentLocation && tangentLocation->isValid() ) {
        if ( format.hasTangents() ) {
            glEnableVertexAttribArray( tangentLocation->getLocation() );
            glVertexAttribPointer( tangentLocation->getLocation(),
                                   format.getTangentComponents(),
                                   GL_FLOAT,
                                   GL_FALSE,
                                   format.getVertexSizeInBytes(),
                                   ( const GLvoid * )( baseOffset + format.getTangentsOffset() ) );
        }
    }

    auto colorLocation = program->getStandardLocation( ShaderProgram::StandardLocation::COLOR_ATTRIBUTE );
    if ( colorLocation && colorLocation->isValid() ) {
        if ( format.hasColors() ) {
            glEnableVertexAttribArray( colorLocation->getLocation() );
            glVertexAttribPointer( colorLocation->getLocation(),
                                   format.getColorComponents(),
                                   GL_FLOAT,
                                   GL_FALSE,
                                   format.getVertexSizeInBytes(),
                                   ( const GLvoid * )( baseOffset + format.getColorsOffset() ) );
        }
    }

    auto uvLocation = program->getStandardLocation( ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE );
    if ( uvLocation && uvLocation->isValid() ) {
        if ( format.hasTextureCoords() ) {
            glEnableVertexAttribArray( uvLocation->getLocation() );
            glVertexAttribPointer( uvLocation->getLocation(),
                                   format.getTextureCoordComponents(),
                                   GL_FLOAT,
                                   GL_FALSE,
                                   format.getVertexSizeInBytes(),
                                   ( const GLvoid * )( baseOffset + format.getTextureCoordsOffset() ) );
        }
    }

    auto boneIdLocation = program->getStandardLocation( ShaderProgram::StandardLocation::BONE_IDS_ATTRIBUTE );
    if ( boneIdLocation && boneIdLocation->isValid() ) {
    	if ( format.hasBoneIds() ) {
    		glEnableVertexAttribArray( boneIdLocation->getLocation() );
    		glVertexAttribPointer( boneIdLocation->getLocation(),
    							   format.getBoneIdComponents(),
    							   GL_FLOAT,
    							   GL_FALSE,
    							   format.getVertexSizeInBytes(),
    							   ( const GLvoid * )( baseOffset + format.getBoneIdsOffset() ) );
    	}
    }

    auto boneWeightLocation = program->getStandardLocation( ShaderProgram::StandardLocation::BONE_WEIGHTS_ATTRIBUTE );
    if ( boneWeightLocation && boneWeightLocation->isValid() ) {
    	if ( format.hasBoneWeights() ) {
    		glEnableVertexAttribArray( boneWeightLocation->getLocation() );
    		glVertexAttribPointer( boneWeightLocation->getLocation(),
    							   format.getBoneWeightComponents(),
    							   GL_FLOAT,
    							   GL_FALSE,
    							   format.getVertexSizeInBytes(),
    							   ( const GLvoid * )( baseOffset + format.getBoneWeightsOffset() ) );
    	}
    }

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void VertexBufferObjectCatalog::unbind( ShaderProgram *program, VertexBufferObject *vbo )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

    glBindVertexArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    
	Catalog< VertexBufferObject >::unbind( program, vbo );

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void VertexBufferObjectCatalog::load( VertexBufferObject *vbo )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	Catalog< VertexBufferObject >::load( vbo );

	GLuint vaoId, vboId;
	extractId( vbo->getCatalogId(), vaoId, vboId );

	glBindVertexArray( vaoId );	

    glBindBuffer( GL_ARRAY_BUFFER, vboId );
    glBufferData( GL_ARRAY_BUFFER,
         vbo->getVertexFormat().getVertexSizeInBytes() * vbo->getVertexCount(),
         vbo->getData(),
         GL_STATIC_DRAW );

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void VertexBufferObjectCatalog::unload( VertexBufferObject *vbo )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
    if ( vbo->getCatalogId() > 0 ) {
      	_unusedVBOIds.push_back( vbo->getCatalogId() );
    }

    Catalog< VertexBufferObject >::unload( vbo );
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void VertexBufferObjectCatalog::cleanup( void )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
    for ( auto id : _unusedVBOIds ) {
        GLuint vaoId, vboId;
        extractId( id, vaoId, vboId );
        
        glDeleteBuffers( 1, &vboId );
        glDeleteVertexArrays( 1, &vaoId );
    }
    
    _unusedVBOIds.clear();
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

