#include "DebugRenderHelper.hpp"

#include "Rendering/ShaderProgram.hpp"
#include "Rendering/AlphaState.hpp"
#include "Rendering/DepthState.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/FrameBufferObject.hpp"

#include "Primitives/BoxPrimitive.hpp"
#include "Primitives/SpherePrimitive.hpp"

#include "SceneGraph/Camera.hpp"

#include "Simulation/AssetManager.hpp"
#include "Simulation/Simulation.hpp"

#include "Foundation/Log.hpp"

#include "Visitors/UpdateWorldState.hpp"
#include "Visitors/UpdateRenderState.hpp"

#include "Components/MaterialComponent.hpp"

using namespace crimild;

#define CRIMILD_DEBUG_RENDER_HELPER_DEPTH_STATE "debug/render_helper/depth_state"
#define CRIMILD_DEBUG_RENDER_HELPER_ALPHA_STATE "debug/render_helper/alpha_state"
#define CRIMILD_DEBUG_RENDER_HELPER_PRIMITIVE_BOX "debug/render_helper/primitive/box"
#define CRIMILD_DEBUG_RENDER_HELPER_PRIMITIVE_SPHERE "debug/render_helper/primitive/sphere"
#define CRIMILD_DEBUG_RENDER_HELPER_VBO_LINES "debug/render_helper/vbo/lines"

void DebugRenderHelper::init( void )
{
    AssetManager::getInstance()->set( CRIMILD_DEBUG_RENDER_HELPER_DEPTH_STATE, crimild::alloc< DepthState >( false ), true );
    AssetManager::getInstance()->set( CRIMILD_DEBUG_RENDER_HELPER_ALPHA_STATE, crimild::alloc< AlphaState >( false ), true );

    AssetManager::getInstance()->set( CRIMILD_DEBUG_RENDER_HELPER_PRIMITIVE_BOX, crimild::alloc< BoxPrimitive >( 1.0f, 1.0f, 1.0f ), true );
    AssetManager::getInstance()->set( CRIMILD_DEBUG_RENDER_HELPER_PRIMITIVE_SPHERE, crimild::alloc< SpherePrimitive >( 1.0f ), true );

    AssetManager::getInstance()->set( CRIMILD_DEBUG_RENDER_HELPER_VBO_LINES, crimild::alloc< VertexBufferObject >( VertexFormat::VF_P3, 10, nullptr ), true );
}

void DebugRenderHelper::renderLine( Renderer *renderer, Camera *camera, const Vector3f &from, const Vector3f &to, const RGBAColorf &color )
{
	Vector3f data[] = {
		from,
		to
	};

	renderLines( renderer, camera, data, 2, color );
}

void DebugRenderHelper::renderLines( Renderer *renderer, Camera *camera, const Vector3f *data, unsigned int count, const RGBAColorf &color )
{
    auto depthState = AssetManager::getInstance()->get< DepthState >( CRIMILD_DEBUG_RENDER_HELPER_DEPTH_STATE );
    auto alphaState = AssetManager::getInstance()->get< AlphaState >( CRIMILD_DEBUG_RENDER_HELPER_ALPHA_STATE );
    auto linesVBO = AssetManager::getInstance()->get< VertexBufferObject >( CRIMILD_DEBUG_RENDER_HELPER_VBO_LINES );
    
    auto program = renderer->getShaderProgram( Renderer::SHADER_PROGRAM_UNLIT_DIFFUSE );
	if ( program == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "No program found for debug rendering" );
		return;
	}

	renderer->bindProgram( program );

	Matrix4f model;
	model.makeIdentity();

	renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM ), camera->getProjectionMatrix() );
	renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM ), camera->getViewMatrix() );
	renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM ), model );

	renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_DIFFUSE_UNIFORM ), color );

	renderer->setDepthState( depthState );

	alphaState->setEnabled( color[ 3 ] < 1.0f );
	renderer->setAlphaState( alphaState );

	if ( linesVBO->getVertexCount() < count ) {
        auto vbo = crimild::alloc< VertexBufferObject >( VertexFormat::VF_P3, count, ( const float * ) &data[ 0 ] );
        AssetManager::getInstance()->set( CRIMILD_DEBUG_RENDER_HELPER_VBO_LINES, vbo );
        linesVBO = crimild::get_ptr( vbo );
	}
	else {
		for ( int i = 0; i < count; i++ ) {
			linesVBO->setPositionAt( i, data[ i ] );
		}
	}

	renderer->drawBuffers( program, Primitive::Type::LINES, linesVBO, count );

	linesVBO->unload();

	renderer->unbindProgram( program );
}

void DebugRenderHelper::renderLines( const Vector3f *data, unsigned int count, const RGBAColorf &color )
{
	auto vbo = crimild::alloc< VertexBufferObject >( VertexFormat::VF_P3, count );
	auto ibo = crimild::alloc< IndexBufferObject >( count );
	for ( int i = 0; i < count; i++ ) {
		vbo->setPositionAt( i, data[ i ] );
		ibo->setIndexAt( i, i );
	}

	auto primitive = crimild::alloc< Primitive >( Primitive::Type::LINES );
	primitive->setVertexBuffer( vbo );
	primitive->setIndexBuffer( ibo );

	auto geometry = crimild::alloc< Geometry >();
	geometry->attachPrimitive( primitive );

	auto material = crimild::alloc< Material >();
	material->setDiffuse( color );
	geometry->getComponent< MaterialComponent >()->attachMaterial( material );

	render( crimild::get_ptr( geometry ) );
}

void DebugRenderHelper::renderBox( Renderer *renderer, Camera *camera, const Vector3f &position, float scale, const RGBAColorf &color )
{
	Transformation model;
	model.setTranslate( position );
	model.setScale( scale );

    auto box = AssetManager::getInstance()->get< Primitive >( CRIMILD_DEBUG_RENDER_HELPER_PRIMITIVE_BOX );
	render( renderer, camera, box, model, color );
}

void DebugRenderHelper::renderBox( Renderer *renderer, Camera *camera, const Vector3f &position, const Vector3f &size, const RGBAColorf &color )
{
	Transformation model;
	model.setTranslate( position );

	auto box = crimild::alloc< BoxPrimitive >( size[ 0 ], size[ 1 ], size[ 2 ] );

	render( renderer, camera, crimild::get_ptr( box ), model, color );
}

void DebugRenderHelper::renderSphere( Renderer *renderer, Camera *camera, const Vector3f &position, float scale, const RGBAColorf &color )
{
	Transformation model;
	model.setTranslate( position );
	model.setScale( scale );

    auto sphere = AssetManager::getInstance()->get< Primitive >( CRIMILD_DEBUG_RENDER_HELPER_PRIMITIVE_SPHERE );
	render( renderer, camera, sphere, model, color );
}

void DebugRenderHelper::render( Renderer *renderer, Camera *camera, Primitive *primitive, const Transformation &model, const RGBAColorf &color )
{
    auto depthState = AssetManager::getInstance()->get< DepthState >( CRIMILD_DEBUG_RENDER_HELPER_DEPTH_STATE );
    auto alphaState = AssetManager::getInstance()->get< AlphaState >( CRIMILD_DEBUG_RENDER_HELPER_ALPHA_STATE );

    auto program = renderer->getShaderProgram( Renderer::SHADER_PROGRAM_UNLIT_DIFFUSE );
	if ( program == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "No program found for debug rendering" );
		return;
	}

	renderer->bindProgram( program );

	renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM ), camera->getProjectionMatrix() );
	renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM ), camera->getViewMatrix() );
	renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM ), model.computeModelMatrix() );

	renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_DIFFUSE_UNIFORM ), color );

	renderer->setDepthState( depthState );

	alphaState->setEnabled( color[ 3 ] < 1.0f );
	renderer->setAlphaState( alphaState );

	renderer->bindVertexBuffer( program, primitive->getVertexBuffer() );
	renderer->bindIndexBuffer( program, primitive->getIndexBuffer() );

	renderer->drawPrimitive( program, primitive );

	renderer->unbindIndexBuffer( program, primitive->getIndexBuffer() );
	renderer->unbindVertexBuffer( program, primitive->getVertexBuffer() );

	renderer->unbindProgram( program );
}

void DebugRenderHelper::render( Geometry *geometry )
{
	auto renderer = Simulation::getInstance()->getRenderer();
	const auto SCREEN_WIDTH = renderer->getScreenBuffer()->getWidth();
	const auto SCREEN_HEIGHT = renderer->getScreenBuffer()->getHeight();
	const auto SCREEN_ASPECT = ( crimild::Real32 ) SCREEN_WIDTH / ( crimild::Real32 ) SCREEN_HEIGHT;

	geometry->local().translate().x() *= SCREEN_ASPECT;

	geometry->perform( UpdateWorldState() );
	geometry->perform( UpdateRenderState() );

	const float top = 1.0f;
	const float bottom = -1.0f;
	const float right = SCREEN_ASPECT;
	const float left = -SCREEN_ASPECT;
	const float far = 1.0f;
	const float near = -1.0f;

	static const auto P_MATRIX = Matrix4f( 
		2.0f / ( right - left ), 0.0f, 0.0f, - ( right + left ) / ( right - left ),
		0.0f, 2.0f / ( top - bottom ), 0.0f, - ( top + bottom ) / ( top - bottom ),
		0.0f, 0.0f, -2.0f / ( far - near ), - ( far + near ) / ( far - near ),
		0.0f, 0.0f, 0.0f, 1.0f
	);

	static const auto V_MATRIX = Matrix4f(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	auto ms = geometry->getComponent< MaterialComponent >();
	if ( ms != nullptr ) {
		ms->forEachMaterial( [renderer, geometry]( Material *material ) {
		    auto program = renderer->getShaderProgram( Renderer::SHADER_PROGRAM_UNLIT_TEXTURE );

			if ( material->getProgram() != nullptr ) {
				program = material->getProgram();
			}
			else if ( material->getColorMap() == nullptr ) {
				program = renderer->getShaderProgram( Renderer::SHADER_PROGRAM_UNLIT_DIFFUSE );
			}

			if ( program == nullptr ) {
		        Log::error( CRIMILD_CURRENT_CLASS_NAME, "No program found for debug rendering" );
				return;
			}

			renderer->bindProgram( program );

			renderer->bindMaterial( program, material );

			renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM ), P_MATRIX );
			renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM ), V_MATRIX );
			renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM ), geometry->getWorld().computeModelMatrix() );

			renderer->setDepthState( DepthState::DISABLED );
			renderer->setAlphaState( AlphaState::ENABLED );

			geometry->forEachPrimitive( [renderer, program]( Primitive *primitive ) {
				auto vbo = primitive->getVertexBuffer();
				if ( vbo == nullptr ) {
					return;
				}

				auto ibo = primitive->getIndexBuffer();
				if ( ibo == nullptr ) {
					return;
				}
				
		        renderer->bindVertexBuffer( program, vbo );
		        renderer->bindIndexBuffer( program, ibo );
		        
		        renderer->drawPrimitive( program, primitive );
		        
		        renderer->unbindVertexBuffer( program, vbo );
		        renderer->unbindIndexBuffer( program, ibo );
			});

			renderer->unbindMaterial( program, material );

			renderer->unbindProgram( program );
		});
	}	
}

void DebugRenderHelper::renderText( std::string str, const Vector3f &position, const RGBAColorf &color )
{
	auto font = AssetManager::getInstance()->get< Font >( AssetManager::FONT_SYSTEM );
	if ( font == nullptr ) {
		Log::warning( CRIMILD_CURRENT_CLASS_NAME, "No available system font" );
		return;
	}

	auto text = crimild::alloc< Text >();
	text->setFont( font );
	text->setSize( 0.05f );
	text->setTextColor( color );

	text->setText( str );

	text->local().setTranslate( position );

    const auto min = text->getLocalBound()->getMin();
    const auto max = text->getLocalBound()->getMax();
    const auto diff = max - min;
    const auto center = text->getLocalBound()->getCenter();
            
	auto box = crimild::alloc< BoxPrimitive >( diff[ 0 ], diff[ 1 ], Numericf::max( 0.1f, max[ 2 ] ) );
	auto background = crimild::alloc< Geometry >();
	background->attachPrimitive( box );
	background->local().setTranslate( position + Vector3f( 0.28f * diff[ 0 ], center[ 1 ], center[ 1 ] ) );
	auto m = crimild::alloc< Material >();
	m->setDiffuse( RGBAColorf( 0.0f, 0.0f, 0.0f, 0.75f ) );
	background->getComponent< MaterialComponent >()->attachMaterial( m );

	render( crimild::get_ptr( background ) );
	render( text->getGeometry() );
}

