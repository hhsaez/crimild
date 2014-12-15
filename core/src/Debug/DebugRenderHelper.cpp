#include "DebugRenderHelper.hpp"

#include "Rendering/ShaderProgram.hpp"

#include "Primitives/BoxPrimitive.hpp"
#include "Primitives/SpherePrimitive.hpp"

#include "Foundation/Log.hpp"

using namespace crimild;

DepthStatePtr DebugRenderHelper::_depthState;
AlphaStatePtr DebugRenderHelper::_alphaState;

PrimitivePtr DebugRenderHelper::_boxPrimitive;
PrimitivePtr DebugRenderHelper::_spherePrimitive;

VertexBufferObjectPtr DebugRenderHelper::_linesVBO;

void DebugRenderHelper::init( void )
{
    _depthState = std::make_shared< DepthState >( false );
    _alphaState = std::make_shared< AlphaState >( false );

    _boxPrimitive = std::make_shared< BoxPrimitive >( 1.0f, 1.0f, 1.0f );
    _spherePrimitive = std::make_shared< SpherePrimitive >( 1.0f );

    _linesVBO = std::make_shared< VertexBufferObject >( VertexFormat::VF_P3, 10, nullptr );
}

void DebugRenderHelper::renderLine( RendererPtr const &renderer, CameraPtr const &camera, const Vector3f &from, const Vector3f &to, const RGBAColorf &color )
{
	Vector3f data[] = {
		from,
		to
	};

	renderLines( renderer, camera, data, 2, color );
}

void DebugRenderHelper::renderLines( RendererPtr const &renderer, CameraPtr const &camera, const Vector3f *data, unsigned int count, const RGBAColorf &color )
{
    auto program= renderer->getShaderProgram( "flat" );
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

	renderer->setDepthState( _depthState );

	_alphaState->setEnabled( color[ 3 ] < 1.0f );
	renderer->setAlphaState( _alphaState );

	if ( _linesVBO->getVertexCount() < count ) {
        _linesVBO = std::make_shared< VertexBufferObject >( VertexFormat::VF_P3, count, ( const float * ) &data[ 0 ] );
	}
	else {
		for ( int i = 0; i < count; i++ ) {
			_linesVBO->setPositionAt( i, data[ i ] );
		}
	}

	renderer->drawBuffers( program, Primitive::Type::LINES, _linesVBO, count );

	_linesVBO->unload();

	renderer->unbindProgram( program );
}

void DebugRenderHelper::renderBox( RendererPtr const &renderer, CameraPtr const &camera, const Vector3f &position, float scale, const RGBAColorf &color )
{
	TransformationImpl model;
	model.setTranslate( position );
	model.setScale( scale );

	render( renderer, camera, _boxPrimitive, model, color );
}

void DebugRenderHelper::renderSphere( RendererPtr const &renderer, CameraPtr const &camera, const Vector3f &position, float scale, const RGBAColorf &color )
{
	TransformationImpl model;
	model.setTranslate( position );
	model.setScale( scale );

	render( renderer, camera, _spherePrimitive, model, color );
}

void DebugRenderHelper::render( RendererPtr const &renderer, CameraPtr const &camera, PrimitivePtr const &primitive, const TransformationImpl &model, const RGBAColorf &color )
{
	auto program = renderer->getShaderProgram( "flat" );
	if ( program == nullptr ) {
		Log::Error << "No program found for debug rendering" << Log::End;
		return;
	}

	renderer->bindProgram( program );

	renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM ), camera->getProjectionMatrix() );
	renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM ), camera->getViewMatrix() );
	renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM ), model.computeModelMatrix() );

	renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_DIFFUSE_UNIFORM ), color );

	renderer->setDepthState( _depthState );

	_alphaState->setEnabled( color[ 3 ] < 1.0f );
	renderer->setAlphaState( _alphaState );

	renderer->bindVertexBuffer( program, primitive->getVertexBuffer() );
	renderer->bindIndexBuffer( program, primitive->getIndexBuffer() );

	renderer->drawPrimitive( program, primitive );

	renderer->unbindIndexBuffer( program, primitive->getIndexBuffer() );
	renderer->unbindVertexBuffer( program, primitive->getVertexBuffer() );

	renderer->unbindProgram( program );
}

