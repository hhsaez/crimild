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

#include "Rendering/Renderer.hpp"
#include "Rendering/VisibilitySet.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/RenderQueue.hpp"
#include "Rendering/FrameBufferObject.hpp"

#include "Primitives/QuadPrimitive.hpp"

#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Light.hpp"

#include "Components/MaterialComponent.hpp"
#include "Components/RenderStateComponent.hpp"

using namespace crimild;

Renderer::Renderer( void )
	: _lightCount( 0 ),
	  _screenPrimitive( crimild::alloc< QuadPrimitive >( 2.0f, 2.0f, VertexFormat::VF_P3_UV2, Vector2f( 0.0f, 1.0f ), Vector2f( 1.0f, -1.0f ) ) ),
      _shaderProgramCatalog( crimild::alloc< Catalog< ShaderProgram >>() ),
	  _textureCatalog( crimild::alloc< Catalog< Texture >>() ),
	  _vertexBufferObjectCatalog( crimild::alloc< Catalog< VertexBufferObject >>() ),
	  _indexBufferObjectCatalog( crimild::alloc< Catalog< IndexBufferObject >>() ),
	  _frameBufferObjectCatalog( crimild::alloc< Catalog< FrameBufferObject >>() )

{
    
}

Renderer::~Renderer( void )
{
    
}

void Renderer::addFrameBuffer( std::string name, FrameBufferObjectPtr const &fbo )
{
	_framebuffers[ name ] = fbo;
}

FrameBufferObjectPtr Renderer::getFrameBuffer( std::string name )
{
	return _framebuffers[ name ];
}

void Renderer::beginRender( void )
{

}

void Renderer::endRender( void )
{
    getShaderProgramCatalog()->cleanup();
    getTextureCatalog()->cleanup();
    getVertexBufferObjectCatalog()->cleanup();
    getIndexBufferObjectCatalog()->cleanup();
    getFrameBufferObjectCatalog()->cleanup();
}

void Renderer::render( RenderQueuePtr const &renderQueue, RenderPassPtr const &renderPass )
{
    renderPass->render( getShared< Renderer >(), renderQueue, renderQueue->getCamera() );
}

void Renderer::bindFrameBuffer( FrameBufferObjectPtr const &fbo )
{
	getFrameBufferObjectCatalog()->bind( fbo );
}

void Renderer::unbindFrameBuffer( FrameBufferObjectPtr const &fbo )
{
	getFrameBufferObjectCatalog()->unbind( fbo );
}

void Renderer::bindProgram( ShaderProgramPtr const &program )
{
	getShaderProgramCatalog()->bind( program );

	program->foreachUniform( [&]( ShaderUniformPtr const &uniform ) {
		if ( uniform != nullptr && uniform->getLocation() != nullptr ) {
			uniform->onBind( getShared< Renderer >() );
		}
	});
}

void Renderer::unbindProgram( ShaderProgramPtr const &program )
{	
	getShaderProgramCatalog()->unbind( program );
}

void Renderer::bindMaterial( ShaderProgramPtr const &program, MaterialPtr const &material )
{
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_USE_COLOR_MAP_UNIFORM ), material->getColorMap() != nullptr );
	if ( material->getColorMap() ) {
		bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_COLOR_MAP_UNIFORM ), material->getColorMap() );
	}
	
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_USE_NORMAL_MAP_UNIFORM ), material->getNormalMap() != nullptr );
	if ( material->getNormalMap() ) {
		bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_NORMAL_MAP_UNIFORM ), material->getNormalMap() );
	}
	
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_USE_SPECULAR_MAP_UNIFORM ), material->getSpecularMap() != nullptr );
	if ( material->getSpecularMap() ) {
		bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_SPECULAR_MAP_UNIFORM ), material->getSpecularMap() );
	}
	
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_USE_EMISSIVE_MAP_UNIFORM ), material->getEmissiveMap() != nullptr );
	if ( material->getEmissiveMap() ) {
		bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_EMISSIVE_MAP_UNIFORM ), material->getEmissiveMap() );
	}
	
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_AMBIENT_UNIFORM ), material->getAmbient() );
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_DIFFUSE_UNIFORM ), material->getDiffuse() );
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_SPECULAR_UNIFORM ), material->getSpecular() );
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_SHININESS_UNIFORM ), material->getShininess() );

	setDepthState( material->getDepthState() );
	setAlphaState( material->getAlphaState() );
}

void Renderer::unbindMaterial( ShaderProgramPtr const &program, MaterialPtr const &material )
{
	unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_COLOR_MAP_UNIFORM ), material->getColorMap() );
	unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_NORMAL_MAP_UNIFORM ), material->getNormalMap() );
	unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_SPECULAR_MAP_UNIFORM ), material->getSpecularMap() );
	unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_EMISSIVE_MAP_UNIFORM ), material->getEmissiveMap() );
}

void Renderer::bindTexture( ShaderLocationPtr const &location, TexturePtr const &texture )
{
	getTextureCatalog()->bind( location, texture );
}

void Renderer::unbindTexture( ShaderLocationPtr const &location, TexturePtr const &texture )
{
	getTextureCatalog()->unbind( location, texture );
}

void Renderer::bindLight( ShaderProgramPtr const &program, LightPtr const &light )
{
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::LIGHT_POSITION_UNIFORM + _lightCount ), light->getPosition() );
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::LIGHT_ATTENUATION_UNIFORM + _lightCount ), light->getAttenuation() );
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::LIGHT_DIRECTION_UNIFORM + _lightCount ), light->getDirection() );
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::LIGHT_COLOR_UNIFORM + _lightCount ), light->getColor() );
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::LIGHT_OUTER_CUTOFF_UNIFORM + _lightCount ), light->getOuterCutoff() );
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::LIGHT_INNER_CUTOFF_UNIFORM + _lightCount ), light->getInnerCutoff() );
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::LIGHT_EXPONENT_UNIFORM + _lightCount ), light->getExponent() );
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::LIGHT_AMBIENT_UNIFORM + _lightCount ), light->getAmbient() );

	++_lightCount;
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::LIGHT_COUNT_UNIFORM ), _lightCount );
}

void Renderer::unbindLight( ShaderProgramPtr const &program, LightPtr const &light )
{
	--_lightCount;
}

void Renderer::bindVertexBuffer( ShaderProgramPtr const &program, VertexBufferObjectPtr const &vbo )
{
	getVertexBufferObjectCatalog()->bind( program, vbo );
}

void Renderer::unbindVertexBuffer( ShaderProgramPtr const &program, VertexBufferObjectPtr const &vbo )
{
	getVertexBufferObjectCatalog()->unbind( program, vbo );
}

void Renderer::bindIndexBuffer( ShaderProgramPtr const &program, IndexBufferObjectPtr const &ibo )
{
	getIndexBufferObjectCatalog()->bind( program, ibo );
}

void Renderer::unbindIndexBuffer( ShaderProgramPtr const &program, IndexBufferObjectPtr const &ibo )
{
	getIndexBufferObjectCatalog()->unbind( program, ibo );
}

void Renderer::applyTransformations( ShaderProgramPtr const &program, GeometryPtr const &geometry, CameraPtr const &camera )
{
    const Matrix4f &projection = camera->getProjectionMatrix();
    const Matrix4f &view = camera->getViewMatrix();
    Matrix4f model = geometry->getWorld().computeModelMatrix();
    Matrix4f normal = model;
	normal[ 12 ] = 0.0f;
	normal[ 13 ] = 0.0f;
	normal[ 14 ] = 0.0f;
    
    applyTransformations( program, projection, view, model, normal );
}

void Renderer::applyTransformations( ShaderProgramPtr const &program, const Matrix4f &projection, const Matrix4f &view, const Matrix4f &model, const Matrix4f &normal )
{
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM ), projection );
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM ), view );
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM ), model );
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::NORMAL_MATRIX_UNIFORM ), normal );
}

void Renderer::applyTransformations( ShaderProgramPtr const &program, const Matrix4f &projection, const Matrix4f &view, const Matrix4f &model )
{
    bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM ), projection );
    bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM ), view );
    bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM ), model );
}

void Renderer::restoreTransformations( ShaderProgramPtr const &program, GeometryPtr const &geometry, CameraPtr const &camera )
{

}

void Renderer::drawScreenPrimitive( ShaderProgramPtr const &program )
{
    // bind vertex and index buffers
    bindVertexBuffer( program, _screenPrimitive->getVertexBuffer() );
    bindIndexBuffer( program, _screenPrimitive->getIndexBuffer() );

    // draw primitive
    drawPrimitive( program, _screenPrimitive );
     
    // unbind primitive buffers
    unbindVertexBuffer( program, _screenPrimitive->getVertexBuffer() );
    unbindIndexBuffer( program, _screenPrimitive->getIndexBuffer() );     
}

