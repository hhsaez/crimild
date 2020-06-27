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

#if 0

#include <Rendering/ShaderProgram.hpp>
#include <Rendering/VertexBufferObject.hpp>

using namespace crimild;
using namespace crimild::opengl;

VertexBufferObjectCatalog::VertexBufferObjectCatalog( void )
{

}

VertexBufferObjectCatalog::~VertexBufferObjectCatalog( void )
{

}

int VertexBufferObjectCatalog::getNextResourceId( VertexBufferObject * )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	GLuint vboId;
    glGenBuffers( 1, &vboId );

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;

    return vboId;
}

void VertexBufferObjectCatalog::bind( VertexBufferObject *vbo )
{
	if ( vbo == nullptr ) return;

	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	if ( vbo->getCatalog() == nullptr ) {
		Catalog< VertexBufferObject >::bind( vbo );
    }

    GLuint vboId = vbo->getCatalogId();

    glBindBuffer( GL_ARRAY_BUFFER, vboId );
    float *baseOffset = 0;

    const VertexFormat &format = vbo->getVertexFormat();

	if ( format.hasPositions() ) {
		glEnableVertexAttribArray( VertexFormat::LayoutLocation::POSITION );
		glVertexAttribPointer(
			VertexFormat::LayoutLocation::POSITION,
			format.getPositionComponents(),
			GL_FLOAT,
			GL_FALSE,
			format.getVertexSizeInBytes(),
			( const GLvoid * )( baseOffset + format.getPositionsOffset() ) );
	}

	if ( format.hasNormals() ) {
		glEnableVertexAttribArray( VertexFormat::LayoutLocation::NORMAL );
		glVertexAttribPointer(
			VertexFormat::LayoutLocation::NORMAL,
			format.getNormalComponents(),
			GL_FLOAT,
			GL_FALSE,
			format.getVertexSizeInBytes(),
			( const GLvoid * )( baseOffset + format.getNormalsOffset() ) );
	}

	if ( format.hasTangents() ) {
		glEnableVertexAttribArray( VertexFormat::LayoutLocation::TANGENT );
		glVertexAttribPointer(
			VertexFormat::LayoutLocation::TANGENT,
			format.getTangentComponents(),
			GL_FLOAT,
			GL_FALSE,
			format.getVertexSizeInBytes(),
			( const GLvoid * )( baseOffset + format.getTangentsOffset() ) );
    }

	if ( format.hasColors() ) {
		glEnableVertexAttribArray( VertexFormat::LayoutLocation::COLOR );
		glVertexAttribPointer(
			VertexFormat::LayoutLocation::COLOR,
			format.getColorComponents(),
			GL_FLOAT,
			GL_FALSE,
			format.getVertexSizeInBytes(),
			( const GLvoid * )( baseOffset + format.getColorsOffset() ) );
    }

	if ( format.hasTextureCoords() ) {
		glEnableVertexAttribArray( VertexFormat::LayoutLocation::TEXTURE_COORD );
		glVertexAttribPointer(
			VertexFormat::LayoutLocation::TEXTURE_COORD,
			format.getTextureCoordComponents(),
			GL_FLOAT,
			GL_FALSE,
			format.getVertexSizeInBytes(),
			( const GLvoid * )( baseOffset + format.getTextureCoordsOffset() ) );
	}

	if ( format.hasBoneIds() ) {
		glEnableVertexAttribArray( VertexFormat::LayoutLocation::BONE_ID );
		glVertexAttribPointer(
			VertexFormat::LayoutLocation::BONE_ID,
			format.getBoneIdComponents(),
			GL_FLOAT,
			GL_FALSE,
			format.getVertexSizeInBytes(),
			( const GLvoid * )( baseOffset + format.getBoneIdsOffset() ) );
    }

	if ( format.hasBoneWeights() ) {
		glEnableVertexAttribArray( VertexFormat::LayoutLocation::BONE_WEIGHT );
		glVertexAttribPointer(
			VertexFormat::LayoutLocation::BONE_WEIGHT,
			format.getBoneWeightComponents(),
			GL_FLOAT,
			GL_FALSE,
			format.getVertexSizeInBytes(),
			( const GLvoid * )( baseOffset + format.getBoneWeightsOffset() ) );
	}

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void VertexBufferObjectCatalog::unbind( VertexBufferObject *vbo )
{
	if ( vbo == nullptr ) return;

	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

    const VertexFormat &format = vbo->getVertexFormat();

	if ( format.hasPositions() ) glDisableVertexAttribArray( VertexFormat::LayoutLocation::POSITION );
	if ( format.hasNormals() ) glDisableVertexAttribArray( VertexFormat::LayoutLocation::NORMAL );
	if ( format.hasTangents() ) glDisableVertexAttribArray( VertexFormat::LayoutLocation::TANGENT );
	if ( format.hasColors() ) glDisableVertexAttribArray( VertexFormat::LayoutLocation::COLOR );
	if ( format.hasTextureCoords() ) glDisableVertexAttribArray( VertexFormat::LayoutLocation::TEXTURE_COORD );
	if ( format.hasBoneIds() ) glDisableVertexAttribArray( VertexFormat::LayoutLocation::BONE_ID );
	if ( format.hasBoneWeights() ) glDisableVertexAttribArray( VertexFormat::LayoutLocation::BONE_WEIGHT );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );

	Catalog< VertexBufferObject >::unbind( vbo );

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void VertexBufferObjectCatalog::load( VertexBufferObject *vbo )
{
	if ( vbo == nullptr ) return;

	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	Catalog< VertexBufferObject >::load( vbo );

	GLuint vboId = vbo->getCatalogId();

    glBindBuffer( GL_ARRAY_BUFFER, vboId );
    glBufferData( GL_ARRAY_BUFFER,
         vbo->getVertexFormat().getVertexSizeInBytes() * vbo->getVertexCount(),
         vbo->getData(),
         GL_STATIC_DRAW );

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void VertexBufferObjectCatalog::unload( VertexBufferObject *vbo )
{
	if ( vbo == nullptr ) return;

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
        GLuint vboId = id;
        glDeleteBuffers( 1, &vboId );
    }

    _unusedVBOIds.clear();

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

#endif
