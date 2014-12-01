#include "DebugRenderHelper.hpp"

#include "Primitives/BoxPrimitive.hpp"
#include "Primitives/SpherePrimitive.hpp"

#include "Foundation/Log.hpp"

using namespace crimild;

Pointer< DepthState > DebugRenderHelper::_depthState;
Pointer< AlphaState > DebugRenderHelper::_alphaState;

Pointer< Primitive > DebugRenderHelper::_boxPrimitive;
Pointer< Primitive > DebugRenderHelper::_spherePrimitive;

Pointer< VertexBufferObject > DebugRenderHelper::_linesVBO;

void DebugRenderHelper::init( void )
{
	_depthState.set( new DepthState( false ) );
	_alphaState.set( new AlphaState( false ) );

	_boxPrimitive.set( new BoxPrimitive( 1.0f, 1.0f, 1.0f ) );
	_spherePrimitive.set( new SpherePrimitive( 1.0f ) );

	_linesVBO = new VertexBufferObject( VertexFormat::VF_P3, 10, nullptr );
}

void DebugRenderHelper::renderLine( crimild::Renderer *renderer, Camera *camera, const Vector3f &from, const Vector3f &to, const RGBAColorf &color )
{
	Vector3f data[] = {
		from,
		to
	};

	renderLines( renderer, camera, data, 2, color );
}

void DebugRenderHelper::renderLines( crimild::Renderer *renderer, Camera *camera, const Vector3f *data, unsigned int count, const RGBAColorf &color )
{
	static Pointer< VertexBufferObject > vbo;

	ShaderProgram *program = renderer->getShaderProgram( "flat" );
	if ( program == nullptr ) {
		Log::Error << "No program found for debug rendering" << Log::End;
		return;
	}

	renderer->bindProgram( program );

	Matrix4f model;
	model.makeIdentity();

	renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM ), camera->getProjectionMatrix() );
	renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM ), camera->getViewMatrix() );
	renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM ), model );

	renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_DIFFUSE_UNIFORM ), color );

	renderer->setDepthState( _depthState.get() );

	_alphaState->setEnabled( color[ 3 ] < 1.0f );
	renderer->setAlphaState( _alphaState.get() );

	if ( _linesVBO->getVertexCount() < count ) {
		_linesVBO = new VertexBufferObject( VertexFormat::VF_P3, count, ( const float * ) &data[ 0 ] );
	}
	else {
		for ( int i = 0; i < count; i++ ) {
			_linesVBO->setPositionAt( i, data[ i ] );
		}
	}

	renderer->drawBuffers( program, Primitive::Type::LINES, _linesVBO.get(), count );

	_linesVBO->unload();

	renderer->unbindProgram( program );
}

void DebugRenderHelper::renderBox( crimild::Renderer *renderer, Camera *camera, const Vector3f &position, float scale, const RGBAColorf &color )
{
	TransformationImpl model;
	model.setTranslate( position );
	model.setScale( scale );

	render( renderer, camera, _boxPrimitive.get(), model, color );
}

void DebugRenderHelper::renderSphere( crimild::Renderer *renderer, Camera *camera, const Vector3f &position, float scale, const RGBAColorf &color )
{
	TransformationImpl model;
	model.setTranslate( position );
	model.setScale( scale );

	render( renderer, camera, _spherePrimitive.get(), model, color );
}

void DebugRenderHelper::render( crimild::Renderer *renderer, Camera *camera, Primitive *primitive, const TransformationImpl &model, const RGBAColorf &color )
{
	ShaderProgram *program = renderer->getShaderProgram( "flat" );
	if ( program == nullptr ) {
		Log::Error << "No program found for debug rendering" << Log::End;
		return;
	}

	renderer->bindProgram( program );

	renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM ), camera->getProjectionMatrix() );
	renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM ), camera->getViewMatrix() );
	renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM ), model.computeModelMatrix() );

	renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_DIFFUSE_UNIFORM ), color );

	renderer->setDepthState( _depthState.get() );

	_alphaState->setEnabled( color[ 3 ] < 1.0f );
	renderer->setAlphaState( _alphaState.get() );

	renderer->bindVertexBuffer( program, primitive->getVertexBuffer() );
	renderer->bindIndexBuffer( program, primitive->getIndexBuffer() );

	renderer->drawPrimitive( program, primitive );

	renderer->unbindIndexBuffer( program, primitive->getIndexBuffer() );
	renderer->unbindVertexBuffer( program, primitive->getVertexBuffer() );

	renderer->unbindProgram( program );
}

