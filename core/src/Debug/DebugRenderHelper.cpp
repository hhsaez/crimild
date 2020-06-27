#include "DebugRenderHelper.hpp"

#include "Rendering/ShaderProgram.hpp"
#include "Rendering/AlphaState.hpp"
#include "Rendering/DepthState.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Font.hpp"

#include "Primitives/BoxPrimitive.hpp"
#include "Primitives/SpherePrimitive.hpp"
#include "Primitives/ParametricSpherePrimitive.hpp"

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
#define CRIMILD_DEBUG_RENDER_HELPER_PRIMITIVE_WIREFRAME_SPHERE "debug/render_helper/primitive/wireframe_sphere"
#define CRIMILD_DEBUG_RENDER_HELPER_VBO_LINES "debug/render_helper/vbo/lines"
#define CRIMILD_DEBUG_RENDER_UNLIT_SHADER_PROGRAM "debug/render/programs/unlit"
#define CRIMILD_DEBUG_RENDER_VERTEX_COLOR_SHADER_PROGRAM "debug/render/programs/vertexColor"

void DebugRenderHelper::init( void )
{
    /*
    AssetManager::getInstance()->set( CRIMILD_DEBUG_RENDER_HELPER_DEPTH_STATE, crimild::alloc< DepthState >( false ), true );
    AssetManager::getInstance()->set( CRIMILD_DEBUG_RENDER_HELPER_ALPHA_STATE, crimild::alloc< AlphaState >( false ), true );
    AssetManager::getInstance()->set( CRIMILD_DEBUG_RENDER_HELPER_PRIMITIVE_BOX, crimild::alloc< BoxPrimitive >( 1.0f, 1.0f, 1.0f ), true );
    AssetManager::getInstance()->set( CRIMILD_DEBUG_RENDER_HELPER_PRIMITIVE_SPHERE, crimild::alloc< SpherePrimitive >( 1.0f ), true );
    AssetManager::getInstance()->set( CRIMILD_DEBUG_RENDER_HELPER_PRIMITIVE_SPHERE, crimild::alloc< ParametricSpherePrimitive >( Primitive::Type::LINES, 1.0f, VertexFormat::VF_P3, Vector2i( 8, 12 ) ), true );
    AssetManager::getInstance()->set( CRIMILD_DEBUG_RENDER_HELPER_VBO_LINES, crimild::alloc< VertexBufferObject >( VertexFormat::VF_P3, 10, nullptr ), true );

    AssetManager::getInstance()->set( CRIMILD_DEBUG_RENDER_UNLIT_SHADER_PROGRAM, crimild::alloc< UnlitShaderProgram >(), true );
    AssetManager::getInstance()->set( CRIMILD_DEBUG_RENDER_VERTEX_COLOR_SHADER_PROGRAM, crimild::alloc< VertexColorShaderProgram >(), true );
    */
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
    /*
    auto depthState = AssetManager::getInstance()->get< DepthState >( CRIMILD_DEBUG_RENDER_HELPER_DEPTH_STATE );
    auto alphaState = AssetManager::getInstance()->get< AlphaState >( CRIMILD_DEBUG_RENDER_HELPER_ALPHA_STATE );

    auto program = AssetManager::getInstance()->get< ShaderProgram >( CRIMILD_DEBUG_RENDER_UNLIT_SHADER_PROGRAM );
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

    auto vbo = crimild::alloc< VertexBufferObject >( VertexFormat::VF_P3, count );
    auto ibo = crimild::alloc< IndexBufferObject >( count );
    for ( int i = 0; i < count; i++ ) {
        vbo->setPositionAt( i, data[ i ] );
        ibo->setIndexAt( i, i );
    }

    auto primitive = crimild::alloc< Primitive >( Primitive::Type::LINES );
    primitive->setVertexBuffer( vbo );
    primitive->setIndexBuffer( ibo );

    auto p = crimild::get_ptr( primitive );
    renderer->bindPrimitive( nullptr, p );
    renderer->drawPrimitive( nullptr, p );
    renderer->unbindPrimitive( nullptr, p );

	renderer->unbindProgram( program );

    renderer->setDepthState( DepthState::ENABLED );
    renderer->setAlphaState( AlphaState::DISABLED );
    */
}

void DebugRenderHelper::renderLines( const Vector3f *data, unsigned int count, const RGBAColorf &color )
{
    /*
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
    */
}

void DebugRenderHelper::renderBox( Renderer *renderer, Camera *camera, const Vector3f &position, float scale, const RGBAColorf &color )
{
    /*
	Transformation model;
	model.setTranslate( position );
	model.setScale( scale );

    auto box = AssetManager::getInstance()->get< Primitive >( CRIMILD_DEBUG_RENDER_HELPER_PRIMITIVE_BOX );
	render( renderer, camera, box, model, color );
    */
}

void DebugRenderHelper::renderBox( Renderer *renderer, Camera *camera, const Vector3f &position, const Vector3f &size, const RGBAColorf &color )
{
    /*
	Transformation model;
	model.setTranslate( position );

	auto box = crimild::alloc< BoxPrimitive >( size[ 0 ], size[ 1 ], size[ 2 ] );

	render( renderer, camera, crimild::get_ptr( box ), model, color );
    */
}

void DebugRenderHelper::renderSphere( Renderer *renderer, Camera *camera, const Vector3f &position, float scale, const RGBAColorf &color )
{
    /*
	Transformation model;
	model.setTranslate( position );
	model.setScale( scale );

    auto sphere = AssetManager::getInstance()->get< Primitive >( CRIMILD_DEBUG_RENDER_HELPER_PRIMITIVE_SPHERE );
	render( renderer, camera, sphere, model, color );
    */
}

void DebugRenderHelper::renderWireframeSphere( Renderer *renderer, Camera *camera, const Vector3f &position, float scale, const RGBAColorf &color )
{
    /*
    Transformation model;
    model.setTranslate( position );
    model.setScale( scale );

    auto sphere = AssetManager::getInstance()->get< Primitive >( CRIMILD_DEBUG_RENDER_HELPER_PRIMITIVE_SPHERE );
    render( renderer, camera, sphere, model, color );
    */
}

void DebugRenderHelper::render( Renderer *renderer, Camera *camera, Primitive *primitive, const Transformation &model, const RGBAColorf &color )
{
    /*
    auto program = AssetManager::getInstance()->get< ShaderProgram >( CRIMILD_DEBUG_RENDER_UNLIT_SHADER_PROGRAM );
	if ( program == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "No program found for debug rendering" );
		return;
	}

    program->bindUniform( PROJECTION_MATRIX_UNIFORM, camera->getProjectionMatrix() );
    program->bindUniform( VIEW_MATRIX_UNIFORM, camera->getViewMatrix() );
    program->bindUniform( MODEL_MATRIX_UNIFORM, model.computeModelMatrix() );
    program->bindUniform( COLOR_UNIFORM, color );

	renderer->bindProgram( program );

    renderer->setDepthState( DepthState::DISABLED );
    renderer->setAlphaState( AlphaState::ENABLED );

	renderer->bindPrimitive( program, primitive );
	renderer->drawPrimitive( program, primitive );
	renderer->unbindPrimitive( program, primitive );

	renderer->unbindProgram( program );

    renderer->setDepthState( DepthState::ENABLED );
    renderer->setAlphaState( AlphaState::DISABLED );
    */
}

void DebugRenderHelper::render( const Transformation &transform, crimild::Real32 axisSize )
{
    /*
    auto p0 = transform.getTranslate();
    auto up = p0 + axisSize * transform.computeUp();
    auto direction = p0 + axisSize * transform.computeDirection();
    auto right = p0 + axisSize * transform.computeRight();
    auto mMatrix = Matrix4f::IDENTITY;

    VertexPrecision vertices[] = {
        // right axis
        p0.x(), p0.y(), p0.z(), 1.0f, 0.0f, 0.0f, 1.0f,
        right.x(), right.y(), right.z(), 1.0f, 0.0f, 0.0f, 1.0f,

        // up axis
        p0.x(), p0.y(), p0.z(), 0.0f, 1.0f, 0.0f, 1.0f,
        up.x(), up.y(), up.z(), 0.0f, 1.0f, 0.0f, 1.0f,

        // direction axis
        p0.x(), p0.y(), p0.z(), 0.0f, 0.0f, 1.0f, 1.0f,
        direction.x(), direction.y(), direction.z(), 0.0f, 0.0f, 1.0f, 1.0f,
    };

    auto vbo = crimild::alloc< VertexBufferObject >( VertexFormat::VF_P3_C4, 6, vertices );
    auto ibo = crimild::alloc< IndexBufferObject >( 6 );
    ibo->generateIncrementalIndices();

    auto primitive = crimild::alloc< Primitive >( Primitive::Type::LINES );
    primitive->setVertexBuffer( vbo );
    primitive->setIndexBuffer( ibo );

    auto renderer = Simulation::getInstance()->getRenderer();
    auto camera = Camera::getMainCamera();

    auto program = AssetManager::getInstance()->get< ShaderProgram >( CRIMILD_DEBUG_RENDER_VERTEX_COLOR_SHADER_PROGRAM );
    if ( program == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "No program found for debug rendering" );
        return;
    }

    program->bindUniform( PROJECTION_MATRIX_UNIFORM, camera->getProjectionMatrix() );
    program->bindUniform( VIEW_MATRIX_UNIFORM, camera->getViewMatrix() );
    program->bindUniform( MODEL_MATRIX_UNIFORM, mMatrix );
    program->bindUniform( COLOR_UNIFORM, RGBAColorf::ONE );

    renderer->bindProgram( program );

    renderer->setDepthState( DepthState::DISABLED );
    renderer->setAlphaState( AlphaState::ENABLED );

    renderer->bindPrimitive( program, crimild::get_ptr( primitive ) );
    renderer->drawPrimitive( program, crimild::get_ptr( primitive ) );
    renderer->unbindPrimitive( program, crimild::get_ptr( primitive ) );

    renderer->unbindProgram( program );

    renderer->setDepthState( DepthState::ENABLED );
    renderer->setAlphaState( AlphaState::DISABLED );
    */
}

void DebugRenderHelper::render( Geometry *geometry )
{
    /*
	auto renderer = Simulation::getInstance()->getRenderer();
	const auto SCREEN_WIDTH = renderer->getScreenBuffer()->getWidth();
	const auto SCREEN_HEIGHT = renderer->getScreenBuffer()->getHeight();
	const auto SCREEN_ASPECT = ( crimild::Real32 ) SCREEN_WIDTH / ( crimild::Real32 ) SCREEN_HEIGHT;

    static const auto P_MATRIX = Frustumf( -SCREEN_ASPECT, SCREEN_ASPECT, -1.0f, 1.0f, 0.01f, 100.0f ).computeOrthographicMatrix();
    static const auto V_MATRIX = Matrix4f::IDENTITY;

	auto ms = geometry->getComponent< MaterialComponent >();
	if ( ms != nullptr ) {
		ms->forEachMaterial( [renderer, geometry]( Material *material ) {
            auto program = AssetManager::getInstance()->get< ShaderProgram >( CRIMILD_DEBUG_RENDER_UNLIT_SHADER_PROGRAM );
			if ( program == nullptr ) {
		        Log::error( CRIMILD_CURRENT_CLASS_NAME, "No program found for debug rendering" );
				return;
			}

			renderer->bindProgram( program );

            renderer->bindUniform( program->getLocation( PROJECTION_MATRIX_UNIFORM ), P_MATRIX );
            renderer->bindUniform( program->getLocation( VIEW_MATRIX_UNIFORM ), V_MATRIX );
            renderer->bindUniform( program->getLocation( MODEL_MATRIX_UNIFORM ), geometry->getWorld().computeModelMatrix() );

            auto color = material->getDiffuse();
            auto colorMap = material->getColorMap();
            if ( colorMap == nullptr ) {
                colorMap = crimild::get_ptr( Texture::ZERO );
            }

            renderer->bindUniform( program->getLocation( COLOR_UNIFORM ), color );
            renderer->bindTexture( program->getLocation( COLOR_MAP_UNIFORM ), colorMap );

			renderer->setDepthState( DepthState::DISABLED );
			renderer->setAlphaState( AlphaState::ENABLED );

			geometry->forEachPrimitive( [ renderer ]( Primitive *primitive ) {
                renderer->bindPrimitive( nullptr, primitive );
                renderer->drawPrimitive( nullptr, primitive );
                renderer->unbindPrimitive( nullptr, primitive );
			});

            renderer->unbindTexture( program->getLocation( COLOR_MAP_UNIFORM ), colorMap );

			renderer->unbindProgram( program );

            renderer->setDepthState( DepthState::ENABLED );
            renderer->setAlphaState( AlphaState::DISABLED );
		});
	}
    */
}

void DebugRenderHelper::renderText( std::string str, const Vector3f &position, const RGBAColorf &color )
{
    /*
	auto font = AssetManager::getInstance()->get< Font >( AssetManager::FONT_SYSTEM );
	if ( font == nullptr ) {
		Log::warning( CRIMILD_CURRENT_CLASS_NAME, "No available system font" );
		return;
	}

	auto text = crimild::alloc< Text >();
	text->setFont( font );
	text->setSize( 0.05f );
	text->setTextColor( color );
	text->setHorizontalAlignment( Text::HorizontalAlignment::LEFT );

	text->setText( str );

    text->local().setTranslate( position );

    text->perform( UpdateWorldState() );
    text->perform( UpdateRenderState() );

	render( text->getGeometry() );
    */
}
