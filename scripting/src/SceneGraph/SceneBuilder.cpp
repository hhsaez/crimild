#include "SceneBuilder.hpp"

using namespace crimild;
using namespace crimild::scripting;

#define SCENE_NODES "scene.nodes"

#define NODE_TYPE "type"
#define NODE_FILENAME "filename"
#define NODE_COMPONENTS "components"
#define NODE_COMPONENT_TYPE "type"
#define NODE_TRANSFORMATION "transformation"
#define NODE_TRANSFORMATION_TRANSLATE "transformation.translate"
#define NODE_TRANSFORMATION_ROTATE "transformation.rotate"
#define NODE_TRANSFORMATION_ROTATE_Q "transformation.rotate_q"
#define NODE_TRANSFORMATION_LOOKAT "transformation.lookAt"

#define GROUP_TYPE "group"
#define GROUP_NODES "nodes"

#define CAMERA_TYPE "camera"
#define CAMERA_RENDER_PASS "renderPass"
#define CAMERA_FRUSTUM "frustum"
#define CAMERA_FRUSTUM_FOV "frustum.fov"
#define CAMERA_FRUSTUM_ASPECT "frustum.aspect"
#define CAMERA_FRUSTUM_NEAR "frustum.near"
#define CAMERA_FRUSTUM_FAR "frustum.far"

#define LIGHT_TYPE "light"
#define LIGHT_CAST_SHADOWS "castShadows"
#define LIGHT_SHADOW_FAR_COEFF "shadowFarCoeff"
#define LIGHT_SHADOW_NEAR_COEFF "shadowNearCoeff"

#define TEXT_TYPE "text"
#define TEXT_FONT "font"
#define TEXT_SIZE "textSize"
#define TEXT_TEXT "text"

SceneBuilder::SceneBuilder( std::string rootNodeName )
	: Scripted( true ),
	  _rootNodeName( rootNodeName )
{

}

SceneBuilder::~SceneBuilder( void )
{

}

void SceneBuilder::reset( void )
{
	getScriptContext().reset();
}

Pointer< Node > SceneBuilder::fromFile( const std::string &filename )
{
	if ( !getScriptContext().load( filename ) ) {
		Log::Error << "Cannot open scene file " << filename << Log::End;
		return Pointer< Node >();
	}

	Log::Debug << "Loading scene from " << filename << Log::End;

	if ( !getScriptContext().test( _rootNodeName ) ) {
		Log::Error << "Cannot find root node named '" << _rootNodeName << "'" << Log::End;
		return Pointer< Node >();
	}

	ScriptContext::Iterable first( getScriptContext(), _rootNodeName, -1 );
	return buildNode( first, nullptr );
}

Pointer< Node > SceneBuilder::buildNode( ScriptContext::Iterable &it, Group *parent ) 
{
	Pointer< Node > current;

	std::string type = it.eval< std::string >( NODE_TYPE );
	if ( type == CAMERA_TYPE ) {
		Log::Debug << "Building 'camera' node" << Log::End;
		Pointer< Camera > camera( new Camera( 90.0f, 4.0f / 3.0f, 1.0f, 1000.0f ) );
		setupCamera( it, camera.get() );
		current = camera;
	}
	else if ( type == LIGHT_TYPE ) {
		Log::Debug << "Building 'light' node" << Log::End;
		Pointer< Light > light( new Light() );
		light->setCastShadows( it.eval< bool >( LIGHT_CAST_SHADOWS ) );

		if ( it.test( LIGHT_SHADOW_NEAR_COEFF ) ) {
			light->setShadowNearCoeff( it.eval< float >( LIGHT_SHADOW_NEAR_COEFF ) );
		}

		if ( it.test( LIGHT_SHADOW_FAR_COEFF ) ) {
			light->setShadowFarCoeff( it.eval< float >( LIGHT_SHADOW_FAR_COEFF ) );
		}

		current = light;
	}
	else if ( type == TEXT_TYPE ) {
		Log::Debug << "Building 'text' node" << Log::End;
		Pointer< Text > text( new Text() );

		std::string fontName = it.eval< std::string >( "font" );
		float textSize = it.eval< float >( "textSize" );

		std::string fontFileName = FileSystem::getInstance().pathForResource( fontName + "_sdf.tga" );
		std::string fontDefFileName = FileSystem::getInstance().pathForResource( fontName + ".txt" );
		Pointer< Font > font( new Font( fontFileName, fontDefFileName ) );

		text->setFont( font.get() );
		text->setSize( textSize );
		text->setText( it.eval< std::string >( TEXT_TEXT ) );
		if ( it.test( "renderOnScreen" ) ) text->getComponent< RenderStateComponent >()->setRenderOnScreen( it.eval< bool >( "renderOnScreen" ) );

		Material *material = text->getMaterial();
		material->setProgram( Simulation::getCurrent()->getRenderer()->getShaderProgram( "sdf" ) );
		if ( it.test( "textColor" ) ) material->setDiffuse( it.eval< RGBAColorf >( "textColor" ) );
		material->getDepthState()->setEnabled( false );
		if ( it.test( "enableDepthTest" ) ) material->getDepthState()->setEnabled( it.eval< bool >( "enableDepthTest" ) );

		current = text;
	}
	else {
		Pointer< Group > group;

		if ( it.test( NODE_FILENAME ) ) {
			Log::Debug << "Building node" << Log::End;
			std::string filename = it.eval< std::string >( NODE_FILENAME );
			
			if ( _sceneCache[ filename ] == nullptr ) {
				OBJLoader loader( FileSystem::getInstance().pathForResource( filename ) );				
				_sceneCache[ filename ] = loader.load();
			}

			ShallowCopy shallowCopy;
			_sceneCache[ filename ]->perform( shallowCopy );
			group.set( shallowCopy.getResult< Group >() );
		}
		else {
			Log::Debug << "Building 'group' node" << Log::End;
			group.set( new Group() );
		}

		it.foreach( GROUP_NODES, [&]( ScriptContext &c, ScriptContext::Iterable &childId ) {
			buildNode( childId, group.get() );
		});

		current = group;
	}

	if ( current != nullptr ) {
		setTransformation( it, current.get() );
		buildNodeComponents( it, current.get() );

		if ( parent != nullptr ) {
			parent->attachNode( current.get() );
		}
	}

	return current;
}

void SceneBuilder::setupCamera( ScriptContext::Iterable &it, Camera *camera ) 
{
	std::string renderPassType = it.eval< std::string >( CAMERA_RENDER_PASS );
	if ( renderPassType == "basic" ) {
		camera->setRenderPass( new BasicRenderPass() );
	}

	if ( it.test( CAMERA_FRUSTUM ) ) {
		float fov = 45.0f;
		float aspect = 4.0f / 3.0f;
		float near = 1.0f;
		float far = 1000.0f;

		if ( it.test( CAMERA_FRUSTUM_FOV ) ) fov = it.eval< float >( CAMERA_FRUSTUM_FOV );
		if ( it.test( CAMERA_FRUSTUM_ASPECT ) ) aspect = it.eval< float >( CAMERA_FRUSTUM_ASPECT );
		if ( it.test( CAMERA_FRUSTUM_NEAR ) ) near = it.eval< float >( CAMERA_FRUSTUM_NEAR );
		if ( it.test( CAMERA_FRUSTUM_FAR ) ) far = it.eval< float >( CAMERA_FRUSTUM_FAR );
		camera->setFrustum( Frustumf( fov, aspect, near, far ) );
	}
}

void SceneBuilder::setTransformation( ScriptContext::Iterable &it, Node *node ) 
{
	Log::Debug << "Setting node transformation" << Log::End;
	if ( it.test( NODE_TRANSFORMATION ) ) node->setLocal( it.eval< TransformationImpl >( NODE_TRANSFORMATION ) );
}

void SceneBuilder::buildNodeComponents( ScriptContext::Iterable &it, Node *node )
{
	it.foreach( NODE_COMPONENTS, [&]( ScriptContext &c, ScriptContext::Iterable &componentIt ) {
		std::string type = componentIt.eval< std::string >( NODE_COMPONENT_TYPE );
		Log::Debug << "Building component of type '" << type << "'" << Log::End;

		if ( type != "null" && _componentBuilders[ type ] != nullptr ) {
			Pointer< NodeComponent > cmp = _componentBuilders[ type ]( componentIt );
			if ( cmp != nullptr ) {
				node->attachComponent( cmp.get() );
			}
			else {
				Log::Error << "Cannot build component of type '" << type << "'" << Log::End;
			}
		}
		else {
			Log::Warning << "Cannot find component builder for type '" << type << "'" << Log::End;
		}
	});
}

