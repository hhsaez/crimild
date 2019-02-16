/*
 * Copyright (c) 2002-present, H. Hernan Saez
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
#include "Rendering/RenderTarget.hpp"
#include "Rendering/RenderGraph/RenderGraph.hpp"
#include "Rendering/RenderGraph/RenderGraphAttachment.hpp"
#include "Rendering/ShaderGraph/Constants.hpp"
#include "Rendering/Programs/ScreenTextureShaderProgram.hpp"
#include "Foundation/Log.hpp"
#include "Primitives/QuadPrimitive.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Light.hpp"
#include "Simulation/AssetManager.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/RenderStateComponent.hpp"
#include "Animation/Skeleton.hpp"

using namespace crimild;
using namespace crimild::rendergraph;
using namespace crimild::shadergraph::locations;

Renderer::Renderer( void )
	: _lightCount( 0 ),
	  _screenPrimitive( crimild::alloc< QuadPrimitive >( 2.0f, 2.0f, VertexFormat::VF_P3_N3_UV2, Vector2f( 0.0f, 1.0f ), Vector2f( 1.0f, -1.0f ) ) ),
      _shaderProgramCatalog( crimild::alloc< Catalog< ShaderProgram >>() ),
	  _textureCatalog( crimild::alloc< Catalog< Texture >>() ),
	  _vertexBufferObjectCatalog( crimild::alloc< Catalog< VertexBufferObject >>() ),
	  _indexBufferObjectCatalog( crimild::alloc< Catalog< IndexBufferObject >>() ),
	  _frameBufferObjectCatalog( crimild::alloc< Catalog< FrameBufferObject >>() ),
	  _renderTargetCatalog( crimild::alloc< Catalog< RenderTarget >>() ),
	  _primitiveCatalog( crimild::alloc< Catalog< Primitive >>() )
{
	_screenBuffer = crimild::alloc< FrameBufferObject >( 800, 600 );
}

Renderer::~Renderer( void )
{
    _screenBuffer = nullptr;
    _screenPrimitive = nullptr;
    
    getShaderProgramCatalog()->unloadAll();
    getTextureCatalog()->unloadAll();
    getVertexBufferObjectCatalog()->unloadAll();
    getIndexBufferObjectCatalog()->unloadAll();
	getPrimitiveCatalog()->unloadAll();
	getRenderTargetCatalog()->unloadAll();
    getFrameBufferObjectCatalog()->unloadAll();
}

void Renderer::setScreenViewport( const Rectf &viewport )
{
    auto screen = getScreenBuffer();
    auto w = screen->getWidth();
    auto h = screen->getHeight();

    setViewport(
        Rectf(
            w * viewport.getX(),
            h * viewport.getY(),
            w * viewport.getWidth(),
            h * viewport.getHeight()
        )
    );
}

void Renderer::beginRender( void )
{
    static const Rectf VIEWPORT( 0.0f, 0.0f, 1.0f, 1.0f );
    setScreenViewport( VIEWPORT );
}

void Renderer::endRender( void )
{
    getShaderProgramCatalog()->cleanup();
    getTextureCatalog()->cleanup();
    getVertexBufferObjectCatalog()->cleanup();
    getIndexBufferObjectCatalog()->cleanup();
	getPrimitiveCatalog()->cleanup();
	getRenderTargetCatalog()->cleanup();
    getFrameBufferObjectCatalog()->cleanup();
}

void Renderer::presentFrame( void )
{

}

void Renderer::render( RenderQueue *renderQueue, RenderGraph *renderGraph )
{
    renderGraph->execute( this, renderQueue );

    auto output = renderGraph->getOutput();
    if ( output == nullptr ) {
        CRIMILD_LOG_ERROR( "No output provided for render graph" );
        return;
    }

    auto texture = output->getTexture();
    if ( texture == nullptr ) {
        CRIMILD_LOG_ERROR( "No valid texture for render graph output" );
        return;
    }

    auto program = AssetManager::getInstance()->get< ScreenTextureShaderProgram >();
    assert( program && "No valid program to render texture" );

    program->bindUniform( COLOR_MAP_UNIFORM, texture );

    bindProgram( program );
    drawScreenPrimitive( program );
    unbindProgram( program );
}

void Renderer::bindRenderTarget( RenderTarget *target )
{
	getRenderTargetCatalog()->bind( target );
}

void Renderer::unbindRenderTarget( RenderTarget *target )
{
	getRenderTargetCatalog()->unbind( target );
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
	program->willBind( this );
	getShaderProgramCatalog()->bind( program );
	program->didBind( this );
}

void Renderer::unbindProgram( ShaderProgram *program )
{
	program->willUnbind( this );
	getShaderProgramCatalog()->unbind( program );
	program->didUnbind( this );
}

void Renderer::bindMaterial( ShaderProgram *program, Material *material )
{
	bindUniform(
		program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_USE_COLOR_MAP_UNIFORM ),
		material->getColorMap() != nullptr
	);
	bindTexture(
		program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_COLOR_MAP_UNIFORM ),
		material->getColorMap()
	);
	
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
	unbindTexture(
		program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_COLOR_MAP_UNIFORM ),
		material->getColorMap()
	);
	
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
	float lightType = 0;
	switch ( light->getType() ) {
		case Light::Type::POINT:
			lightType = 0;
			break;

		case Light::Type::DIRECTIONAL:
			lightType = 1;
			break;

		case Light::Type::SPOT:
			lightType = 2;
			break;

		case Light::Type::AMBIENT:
			lightType = 3;
			break;
	}

	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::LIGHT_TYPE_UNIFORM + _lightCount ), lightType );
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

void Renderer::bindPrimitive( ShaderProgram *, Primitive *primitive )
{
	getPrimitiveCatalog()->bind( primitive );
}

void Renderer::unbindPrimitive( ShaderProgram *, Primitive *primitive )
{
	getPrimitiveCatalog()->unbind( primitive );
}

void Renderer::bindVertexBuffer( ShaderProgram *program, VertexBufferObject *vbo )
{
	if ( vbo == nullptr ) {
		return;
	}
	
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

void Renderer::drawGeometry( Geometry *geometry, ShaderProgram *program, const Matrix4f &modelMatrix )
{
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM ), modelMatrix );
	
	auto rc = geometry->getComponent< RenderStateComponent >();
	bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::SKINNED_MESH_JOINT_COUNT_UNIFORM ), 0 );
	if ( auto skeleton = rc->getSkeleton() ) {
		bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::SKINNED_MESH_JOINT_COUNT_UNIFORM ), ( int ) skeleton->getJoints().size() );
		skeleton->getJoints().each( [ this, program ]( const std::string &, SharedPointer< animation::Joint > const &joint ) {
			bindUniform(
				program->getStandardLocation( ShaderProgram::StandardLocation::SKINNED_MESH_JOINT_POSE_UNIFORM + joint->getId() ),
				joint->getPoseMatrix() );
		});
	}
	
	geometry->forEachPrimitive( [ this, program ]( Primitive *primitive ) {
		bindPrimitive( program, primitive );
		drawPrimitive( program, primitive );
		unbindPrimitive( program, primitive );
	});
}

void Renderer::drawScreenPrimitive( ShaderProgram *program )
{
	auto primitive = crimild::get_ptr( _screenPrimitive );

	bindPrimitive( program, primitive );
    drawPrimitive( program, crimild::get_ptr( _screenPrimitive ) );
	unbindPrimitive( program, primitive );
}

