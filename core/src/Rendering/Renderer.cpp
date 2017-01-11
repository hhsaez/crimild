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
#include "Rendering/Material.hpp"
#include "Rendering/RenderQueue.hpp"
#include "Rendering/FrameBufferObject.hpp"
#include "Rendering/RenderPasses/RenderPass.hpp"
#include "Foundation/Log.hpp"
#include "Primitives/QuadPrimitive.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Light.hpp"
#include "Simulation/AssetManager.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/RenderStateComponent.hpp"

using namespace crimild;

constexpr const char *Renderer::FBO_AUX_256;
constexpr const char *Renderer::FBO_AUX_512;
constexpr const char *Renderer::FBO_AUX_1024;
constexpr const char *Renderer::FBO_AUX_COLOR_TARGET_NAME;
constexpr const char *Renderer::FBO_AUX_DEPTH_TARGET_NAME;

constexpr const char *Renderer::SHADER_PROGRAM_RENDER_PASS_FORWARD;
constexpr const char *Renderer::SHADER_PROGRAM_RENDER_PASS_STANDARD;
constexpr const char *Renderer::SHADER_PROGRAM_LIT_TEXTURE;
constexpr const char *Renderer::SHADER_PROGRAM_LIT_DIFFUSE;
constexpr const char *Renderer::SHADER_PROGRAM_UNLIT_TEXTURE;
constexpr const char *Renderer::SHADER_PROGRAM_UNLIT_DIFFUSE;
constexpr const char *Renderer::SHADER_PROGRAM_UNLIT_VERTEX_COLOR;
constexpr const char *Renderer::SHADER_PROGRAM_PARTICLE_SYSTEM;
constexpr const char *Renderer::SHADER_PROGRAM_TEXT_BASIC;
constexpr const char *Renderer::SHADER_PROGRAM_TEXT_SDF;
constexpr const char *Renderer::SHADER_PROGRAM_SCREEN_TEXTURE;
constexpr const char *Renderer::SHADER_PROGRAM_DEPTH;

Renderer::Renderer( void )
	: _lightCount( 0 ),
	  _screenPrimitive( crimild::alloc< QuadPrimitive >( 2.0f, 2.0f, VertexFormat::VF_P3_N3_UV2, Vector2f( 0.0f, 1.0f ), Vector2f( 1.0f, -1.0f ) ) ),
      _shaderProgramCatalog( crimild::alloc< Catalog< ShaderProgram >>() ),
	  _textureCatalog( crimild::alloc< Catalog< Texture >>() ),
	  _vertexBufferObjectCatalog( crimild::alloc< Catalog< VertexBufferObject >>() ),
	  _indexBufferObjectCatalog( crimild::alloc< Catalog< IndexBufferObject >>() ),
	  _frameBufferObjectCatalog( crimild::alloc< Catalog< FrameBufferObject >>() )
{
	_screenBuffer = crimild::alloc< FrameBufferObject >( 800, 600 );

 	generateAuxFBO( FBO_AUX_256, 256, 256 );
 	generateAuxFBO( FBO_AUX_512, 512, 512 );
 	generateAuxFBO( FBO_AUX_1024, 1024, 1024 );   
}

Renderer::~Renderer( void )
{
    _screenBuffer = nullptr;
    _screenPrimitive = nullptr;
    
    getShaderProgramCatalog()->unloadAll();
    getTextureCatalog()->unloadAll();
    getVertexBufferObjectCatalog()->unloadAll();
    getIndexBufferObjectCatalog()->unloadAll();
    getFrameBufferObjectCatalog()->unloadAll();
}

ShaderProgram *Renderer::getShaderProgram( std::string name )
{
    return AssetManager::getInstance()->get< ShaderProgram >( name );
}

void Renderer::setShaderProgram( std::string name, SharedPointer< ShaderProgram > const &program )
{
    // assets stored by the renderer are assumed to be persistent
    AssetManager::getInstance()->set( name, program, true );
}

void Renderer::setFrameBuffer( std::string name, SharedPointer< FrameBufferObject > const &fbo )
{
    // assets stored by the renderer are assumed to be persistent
    AssetManager::getInstance()->set( name, fbo );
}

FrameBufferObject *Renderer::getFrameBuffer( std::string name )
{
    return AssetManager::getInstance()->get< FrameBufferObject >( name );
}

SharedPointer< FrameBufferObject > Renderer::generateAuxFBO( std::string name, int width, int height )
{
    auto fbo = crimild::alloc< FrameBufferObject >( width, height );
    fbo->getRenderTargets().add( FBO_AUX_DEPTH_TARGET_NAME, crimild::alloc< RenderTarget >( RenderTarget::Type::DEPTH_24, RenderTarget::Output::RENDER, width, height ) );
    fbo->getRenderTargets().add( FBO_AUX_COLOR_TARGET_NAME, crimild::alloc< RenderTarget >( RenderTarget::Type::COLOR_RGBA, RenderTarget::Output::TEXTURE, width, height ) );
    AssetManager::getInstance()->set( name, fbo, true );
    return fbo;
}

void Renderer::beginRender( void )
{
    static const Rectf VIEWPORT( 0.0f, 0.0f, 1.0f, 1.0f );
    setViewport( VIEWPORT );
}

void Renderer::endRender( void )
{
    getShaderProgramCatalog()->cleanup();
    getTextureCatalog()->cleanup();
    getVertexBufferObjectCatalog()->cleanup();
    getIndexBufferObjectCatalog()->cleanup();
    getFrameBufferObjectCatalog()->cleanup();
}

void Renderer::presentFrame( void )
{
    auto sBuffer = getFrameBuffer( RenderPass::S_BUFFER_NAME );
    if ( sBuffer != nullptr ) {
        auto color = sBuffer->getRenderTargets().get( RenderPass::S_BUFFER_COLOR_TARGET_NAME );
        auto program = getShaderProgram( crimild::Renderer::SHADER_PROGRAM_SCREEN_TEXTURE );
        if ( program != nullptr ) {
            bindProgram( program );
            bindTexture( program->getStandardLocation( crimild::ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), color->getTexture() );
            drawScreenPrimitive( program );
            unbindTexture( program->getStandardLocation( crimild::ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), color->getTexture() );
            unbindProgram( program );
        }
    }
}

void Renderer::render( RenderQueue *renderQueue, RenderPass *renderPass )
{
    renderPass->render( this, renderQueue, renderQueue->getCamera() );
}

void Renderer::bindFrameBuffer( FrameBufferObject *fbo )
{
	getFrameBufferObjectCatalog()->bind( fbo );
}

void Renderer::unbindFrameBuffer( FrameBufferObject *fbo )
{
	getFrameBufferObjectCatalog()->unbind( fbo );
}

void Renderer::bindProgram( ShaderProgram *program )
{
	getShaderProgramCatalog()->bind( program );

    auto self = this;
	program->forEachUniform( [self]( ShaderUniform *uniform ) {
		if ( uniform != nullptr && uniform->getLocation() != nullptr ) {
			uniform->onBind( self );
		}
	});
}

void Renderer::unbindProgram( ShaderProgram *program )
{	
	getShaderProgramCatalog()->unbind( program );
}

void Renderer::bindMaterial( ShaderProgram *program, Material *material )
{
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_USE_COLOR_MAP_UNIFORM ), material->getColorMap() != nullptr );
	if ( material->getColorMap() != nullptr ) {
		auto loc = program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_COLOR_MAP_UNIFORM );
		bindTexture( loc, material->getColorMap() );
	}
	
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_USE_NORMAL_MAP_UNIFORM ), material->getNormalMap() != nullptr );
	if ( material->getNormalMap() != nullptr ) {
		auto loc = program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_NORMAL_MAP_UNIFORM );
		bindTexture( loc, material->getNormalMap() );
	}
	
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_USE_SPECULAR_MAP_UNIFORM ), material->getSpecularMap() != nullptr );
	if ( material->getSpecularMap() != nullptr ) {
		auto loc = program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_SPECULAR_MAP_UNIFORM );
		bindTexture( loc, material->getSpecularMap() );
	}
	
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_USE_EMISSIVE_MAP_UNIFORM ), material->getEmissiveMap() != nullptr );
	if ( material->getEmissiveMap() != nullptr ) {
		auto loc = program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_EMISSIVE_MAP_UNIFORM );
		bindTexture( loc, material->getEmissiveMap() );
	}
	
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_AMBIENT_UNIFORM ), material->getAmbient() );
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_DIFFUSE_UNIFORM ), material->getDiffuse() );
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_SPECULAR_UNIFORM ), material->getSpecular() );
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_SHININESS_UNIFORM ), material->getShininess() );
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_EMISSIVE_UNIFORM ), material->getEmissive() );

	setDepthState( material->getDepthState() );
	setAlphaState( material->getAlphaState() );
	setCullFaceState( material->getCullFaceState() );
	setColorMaskState( material->getColorMaskState() );
}

void Renderer::unbindMaterial( ShaderProgram *program, Material *material )
{
	unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_COLOR_MAP_UNIFORM ), material->getColorMap() );
	unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_NORMAL_MAP_UNIFORM ), material->getNormalMap() );
	unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_SPECULAR_MAP_UNIFORM ), material->getSpecularMap() );
	unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_EMISSIVE_MAP_UNIFORM ), material->getEmissiveMap() );
}

void Renderer::bindTexture( ShaderLocation *location, Texture *texture )
{
	getTextureCatalog()->bind( location, texture );
}

void Renderer::unbindTexture( ShaderLocation *location, Texture *texture )
{
	getTextureCatalog()->unbind( location, texture );
}

void Renderer::bindLight( ShaderProgram *program, Light *light )
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

void Renderer::unbindLight( ShaderProgram *program, Light *light )
{
	--_lightCount;
}

void Renderer::bindVertexBuffer( ShaderProgram *program, VertexBufferObject *vbo )
{
	getVertexBufferObjectCatalog()->bind( program, vbo );
}

void Renderer::unbindVertexBuffer( ShaderProgram *program, VertexBufferObject *vbo )
{
	getVertexBufferObjectCatalog()->unbind( program, vbo );
}

void Renderer::bindIndexBuffer( ShaderProgram *program, IndexBufferObject *ibo )
{
	getIndexBufferObjectCatalog()->bind( program, ibo );
}

void Renderer::unbindIndexBuffer( ShaderProgram *program, IndexBufferObject *ibo )
{
	getIndexBufferObjectCatalog()->unbind( program, ibo );
}

void Renderer::applyTransformations( ShaderProgram *program, Geometry *geometry, Camera *camera )
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

void Renderer::applyTransformations( ShaderProgram *program, const Matrix4f &projection, const Matrix4f &view, const Matrix4f &model, const Matrix4f &normal )
{
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM ), projection );
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM ), view );
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM ), model );
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::NORMAL_MATRIX_UNIFORM ), normal );
}

void Renderer::applyTransformations( ShaderProgram *program, const Matrix4f &projection, const Matrix4f &view, const Matrix4f &model )
{
    bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM ), projection );
    bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM ), view );
    bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM ), model );
}

void Renderer::restoreTransformations( ShaderProgram *program, Geometry* geometry, Camera *camera )
{

}

void Renderer::drawScreenPrimitive( ShaderProgram *program )
{
    // bind vertex and index buffers
    bindVertexBuffer( program, _screenPrimitive->getVertexBuffer() );
    bindIndexBuffer( program, _screenPrimitive->getIndexBuffer() );

    // draw primitive
    drawPrimitive( program, crimild::get_ptr( _screenPrimitive ) );
     
    // unbind primitive buffers
    unbindVertexBuffer( program, _screenPrimitive->getVertexBuffer() );
    unbindIndexBuffer( program, _screenPrimitive->getIndexBuffer() );     
}

