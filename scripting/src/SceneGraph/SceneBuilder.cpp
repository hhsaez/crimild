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

SceneBuilder::SceneBuilder( std::string rootNodeName )
	: Scripted( true ),
	  _rootNodeName( rootNodeName )
{
	registerComponentBuilder< RigidBodyComponent >( std::bind( &SceneBuilder::buildRigidBodyComponent, this, std::placeholders::_1 ) );
	registerComponentBuilder< ColliderComponent >( std::bind( &SceneBuilder::buildColliderComponent, this, std::placeholders::_1 ) );
}

SceneBuilder::~SceneBuilder( void )
{

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

	Pointer< Group > scene( new Group() );

	getScriptContext().foreach( _rootNodeName + "." + GROUP_NODES, [&]( ScriptContext &c, ScriptContext::Iterable &it ) {
		buildNode( it, scene.get() );
	});

	return scene;
}

void SceneBuilder::buildNode( ScriptContext::Iterable &it, Group *parent ) 
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
	else if ( type == GROUP_TYPE ) {
		Log::Debug << "Building 'group' node" << Log::End;
		Pointer< Group > group( new Group() );
		it.foreach( GROUP_NODES, [&]( ScriptContext &c, ScriptContext::Iterable &childId ) {
			buildNode( childId, group.get() );
		});

		current = group;
	}
	else {
		Log::Debug << "Building node" << Log::End;
		std::string filename = it.eval< std::string >( NODE_FILENAME );
		if ( filename != "null" ) {
			OBJLoader loader( FileSystem::getInstance().pathForResource( filename ) );
			current = loader.load();
		}
		else {
			current = new Node();
		}
	}

	if ( current == nullptr ) {
		return;
	}

	setTransformation( it, current.get() );
	buildNodeComponents( it, current.get() );

	parent->attachNode( current.get() );
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

	if ( it.test( NODE_TRANSFORMATION_TRANSLATE ) ) {
		node->local().setTranslate( it.eval< Vector3f >( NODE_TRANSFORMATION_TRANSLATE ) );
	}

	if ( it.test( NODE_TRANSFORMATION_ROTATE ) ) {
		Vector4f axisAngle = it.eval< Vector4f >( NODE_TRANSFORMATION_ROTATE );
		node->local().rotate().fromAxisAngle( Vector3f( axisAngle[ 0 ], axisAngle[ 1 ], axisAngle[ 2 ] ), Numericf::DEG_TO_RAD * axisAngle[ 3 ] );
	}

	if ( it.test( NODE_TRANSFORMATION_LOOKAT ) ) {
		node->local().lookAt( it.eval< Vector3f >( NODE_TRANSFORMATION_LOOKAT ), Vector3f( 0.0f, 1.0f, 0.0f ) );
	}

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

Pointer< NodeComponent > SceneBuilder::buildRigidBodyComponent( ScriptContext::Iterable &it )
{
	Pointer< RigidBodyComponent > rigidBody( new RigidBodyComponent() );
	return rigidBody;
}

Pointer< NodeComponent > SceneBuilder::buildColliderComponent( ScriptContext::Iterable &it )
{
	Pointer< ColliderComponent > collider( new ColliderComponent() );
	return collider;
}

