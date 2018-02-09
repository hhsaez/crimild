#include "LuaSceneBuilder.hpp"

#include "SceneGraph/Builders/Components/LuaFreeLookCameraComponentBuilder.hpp"
#include "SceneGraph/Builders/Components/LuaOrbitComponentBuilder.hpp"
#include "SceneGraph/Builders/Components/LuaBehaviorControllerBuilder.hpp"

#include "SceneGraph/Builders/ParticleSystem/LuaParticleSystemComponentBuilder.hpp"

#include "SceneGraph/Builders/ParticleSystem/Generators/LuaBoxPositionParticleGeneratorBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Generators/LuaGridPositionParticleGeneratorBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Generators/LuaSpherePositionParticleGeneratorBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Generators/LuaSphereVelocityParticleGeneratorBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Generators/LuaRandomVector3fParticleGeneratorBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Generators/LuaRandomReal32ParticleGeneratorBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Generators/LuaDefaultVector3fParticleGeneratorBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Generators/LuaDefaultReal32ParticleGeneratorBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Generators/LuaColorParticleGeneratorBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Generators/LuaTimeParticleGeneratorBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Generators/LuaNodePositionParticleGeneratorBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Updaters/LuaEulerParticleUpdaterBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Updaters/LuaPositionVelocityParticleUpdaterBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Updaters/LuaTimeParticleUpdaterBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Updaters/LuaFloorParticleUpdaterBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Updaters/LuaCameraSortParticleUpdaterBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Updaters/LuaZSortParticleUpdaterBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Updaters/LuaSetVector3fValueParticleUpdaterBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Updaters/LuaAttractorParticleUpdaterBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Updaters/LuaUniformScaleParticleUpdaterBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Updaters/LuaColorParticleUpdaterBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Renderers/LuaPointSpriteParticleRendererBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Renderers/LuaOrientedQuadParticleRendererBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Renderers/LuaNodeParticleRendererBuilder.hpp"
#include "SceneGraph/Builders/ParticleSystem/Renderers/LuaAnimatedSpriteParticleRendererBuilder.hpp"

#include "SceneGraph/Builders/Navigation/LuaNavigationControllerBuilder.hpp"
#include "SceneGraph/Builders/Navigation/LuaNavigationMeshContainerBuilder.hpp"

#include "SceneGraph/Builders/Behaviors/Actions/LuaAnimateContextValueBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Actions/LuaAnimateParticleSystemBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Actions/LuaAnimateSettingValueBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Actions/LuaClearTargetsBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Actions/LuaCopyTransformFromTargetBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Actions/LuaEnableNodeBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Actions/LuaExecuteBehaviorBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Actions/LuaExecuteBehaviorOnTargetBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Actions/LuaFindTargetBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Actions/LuaLoadSceneBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Actions/LuaLookAtBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Actions/LuaPrintMessageBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Actions/LuaResetNavigationBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Actions/LuaSetContextValueBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Actions/LuaSetSettingValueBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Actions/LuaSuccessBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Actions/LuaTransformBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Actions/LuaTriggerBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Actions/LuaWaitBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Composites/LuaParallelBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Composites/LuaSelectorBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Composites/LuaSequenceBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Conditions/LuaDistanceToTargetBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Conditions/LuaHasTargetsBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Conditions/LuaIsAtTargetBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Conditions/LuaTestContextValueBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Conditions/LuaTestInputAxisBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Conditions/LuaTestSettingValueBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Decorators/LuaInverterBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Decorators/LuaRepeatBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Decorators/LuaRepeatUntilFailBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Decorators/LuaRepeatUntilSuccessBuilder.hpp"
#include "SceneGraph/Builders/Behaviors/Decorators/LuaSucceederBuilder.hpp"

#include "SceneGraph/Builders/Debug/LuaDebugRenderComponentBuilder.hpp"

#include <Boundings/SphereBoundingVolume.hpp>
#include <Components/RenderStateComponent.hpp>
#include <SceneGraph/Node.hpp>
#include <SceneGraph/Camera.hpp>
#include <SceneGraph/Light.hpp>
#include <SceneGraph/Group.hpp>
#include <Simulation/AssetManager.hpp>
#include <Simulation/FileSystem.hpp>
#include <Streaming/FileStream.hpp>
#include <Loaders/OBJLoader.hpp>
#include <Rendering/ShadowMap.hpp>
#include <Behaviors/Actions/MotionSeek.hpp>
#include <Behaviors/Actions/MotionApply.hpp>
#include <Behaviors/Actions/MotionReset.hpp>
#include <Behaviors/Actions/MotionAvoidWalls.hpp>
#include <Behaviors/Actions/MotionAvoidOthers.hpp>
#include <Behaviors/Actions/MotionComputePathToTarget.hpp>

using namespace crimild;
using namespace crimild::scripting;

#ifdef CRIMILD_ENABLE_IMPORT
    #include <Crimild_Import.hpp>

    using namespace crimild::import;
#endif

#ifdef CRIMILD_ENABLE_PHYSICS
    #include <Crimild_Physics.hpp>

    using namespace crimild::physics;
#endif

#define SCENE_NODES "scene.nodes"

#define NODE_TYPE "type"
#define NODE_FILENAME "filename"
#define NODE_RADIUS "radius"
#define NODE_COMPONENTS "components"
#define NODE_COMPONENT_TYPE "type"
#define NODE_TRANSFORMATION "transformation"
#define NODE_TRANSFORMATION_TRANSLATE "transformation.translate"
#define NODE_TRANSFORMATION_ROTATE "transformation.rotate"
#define NODE_TRANSFORMATION_ROTATE_Q "transformation.rotate_q"
#define NODE_TRANSFORMATION_LOOKAT "transformation.lookAt"

#define GROUP_TYPE "crimild::Group"
#define GROUP_NODES "nodes"

#define CAMERA_TYPE "crimild::Camera"
#define CAMERA_MAIN_CAMERA "isMainCamera"
#define CAMERA_RENDER_PASS "renderPass"
#define CAMERA_FRUSTUM "frustum"
#define CAMERA_FRUSTUM_FOV "frustum.fov"
#define CAMERA_FRUSTUM_ASPECT "frustum.aspect"
#define CAMERA_FRUSTUM_NEAR "frustum.near"
#define CAMERA_FRUSTUM_FAR "frustum.far"
#define CAMERA_CULLING_ENABLED "enableCulling"

#define LIGHT_TYPE "crimild::Light"
#define LIGHT_CAST_SHADOWS "castShadows"
#define LIGHT_SHADOW_FAR_COEFF "shadowFarCoeff"
#define LIGHT_SHADOW_NEAR_COEFF "shadowNearCoeff"
#define LIGHT_ATTENUATION "attenuation"
#define LIGHT_LIGHT_TYPE "lightType"
#define LIGHT_COLOR "color"
#define LIGHT_AMBIENT "ambient"

#define TEXT_TYPE "crimild::Text"
#define TEXT_FONT "font"
#define TEXT_SIZE "textSize"
#define TEXT_TEXT "text"

#define PARTICLE_SYSTEM_TYPE "crimild::ParticleSystem"

#define PHYSICS_RIGID_BODY_TYPE "crimild::physics::RigidBody"
#define PHYSICS_CHARACTER_CONTROLLER_TYPE "crimild::physics::CharacterController"
#define PHYSICS_BOX_COLLIDER_TYPE "crimild::physics::BoxCollider"
#define PHYSICS_MESH_COLLIDER_TYPE "crimild::physics::MeshCollider"
#define PHYSICS_CAPSULE_COLLIDER_TYPE "crimild::physics::CapsuleCollider"
#define PHYSICS_CONVEX_HULL_COLLIDER_TYPE "crimild::physics::ConvexHullCollider"

#define CRIMILD_SCRIPTING_REGISTER_DEFAULT_BUILDER( X ) \
		crimild::scripting::LuaObjectBuilderRegistry::getInstance()->registerCustomBuilder( \
			#X,\
			[]( ScriptEvaluator const & ) -> SharedPointer< X > { return crimild::alloc< X >(); } \
			);

LuaNodeBuilderRegistry::LuaNodeBuilderRegistry( void )
{
    
}

LuaNodeBuilderRegistry::~LuaNodeBuilderRegistry( void )
{
    
}

void LuaNodeBuilderRegistry::flush( void )
{
#if CRIMILD_SCRIPTING_LOG_VERBOSE
    std::stringstream str;
    str << "Available node builders:\n";
    for ( auto it : _nodeBuilders ) {
        if ( it.second != nullptr ) {
            str << "\t" << it.first << "\n";
        }
    }
    Log::debug( CRIMILD_CURRENT_CLASS_NAME, str.str() );
#endif
}

LuaComponentBuilderRegistry::LuaComponentBuilderRegistry( void )
{
    
}

LuaComponentBuilderRegistry::~LuaComponentBuilderRegistry( void )
{
    
}

void LuaComponentBuilderRegistry::flush( void )
{
#if CRIMILD_SCRIPTING_LOG_VERBOSE
    std::stringstream str;
    str << "Available component builders:\n";
    for ( auto it : _componentBuilders ) {
        if ( it.second != nullptr ) {
            str << "\t" << it.first << "\n";
        }
    }
    Log::debug( CRIMILD_CURRENT_CLASS_NAME, str.str() );
#endif
}

LuaObjectBuilderRegistry::LuaObjectBuilderRegistry( void )
{
    
}

LuaObjectBuilderRegistry::~LuaObjectBuilderRegistry( void )
{
    
}

void LuaObjectBuilderRegistry::flush( void )
{
#if CRIMILD_SCRIPTING_LOG_VERBOSE
    std::stringstream str;
    str << "Available object builders:\n";
    for ( auto it : _builders ) {
        if ( it.second != nullptr ) {
            str << "\t" << it.first << "\n";
        }
    }
    Log::debug( CRIMILD_CURRENT_CLASS_NAME, str.str() );
#endif
}

LuaSceneBuilder::LuaSceneBuilder( std::string rootNodeName )
	: Scripted( true ),
	  _rootNodeName( rootNodeName )
{
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::FreeLookCameraComponent, LuaFreeLookCameraComponentBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::OrbitComponent, LuaOrbitComponentBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::BehaviorController, LuaBehaviorControllerBuilder::build );
	
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::ParticleSystemComponent, LuaParticleSystemComponentBuilder::build );
	
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::BoxPositionParticleGenerator, LuaBoxPositionParticleGeneratorBuilder::build );
    CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::GridPositionParticleGenerator, LuaGridPositionParticleGeneratorBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::SpherePositionParticleGenerator, LuaSpherePositionParticleGeneratorBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::SphereVelocityParticleGenerator, LuaSphereVelocityParticleGeneratorBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::RandomVector3fParticleGenerator, LuaRandomVector3fParticleGeneratorBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::RandomReal32ParticleGenerator, LuaRandomReal32ParticleGeneratorBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::DefaultVector3fParticleGenerator, LuaDefaultVector3fParticleGeneratorBuilder::build );
    CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::DefaultReal32ParticleGenerator, LuaDefaultReal32ParticleGeneratorBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::ColorParticleGenerator, LuaColorParticleGeneratorBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::TimeParticleGenerator, LuaTimeParticleGeneratorBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::NodePositionParticleGenerator, LuaNodePositionParticleGeneratorBuilder::build );

	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::EulerParticleUpdater, LuaEulerParticleUpdaterBuilder::build );
    CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::PositionVelocityParticleUpdater, LuaPositionVelocityParticleUpdaterBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::TimeParticleUpdater, LuaTimeParticleUpdaterBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::FloorParticleUpdater, LuaFloorParticleUpdaterBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::CameraSortParticleUpdater, LuaCameraSortParticleUpdaterBuilder::build );
    CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::ZSortParticleUpdater, LuaZSortParticleUpdaterBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::AttractorParticleUpdater, LuaAttractorParticleUpdaterBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::SetVector3fValueParticleUpdater, LuaSetVector3fValueParticleUpdaterBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::UniformScaleParticleUpdater, LuaUniformScaleParticleUpdaterBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::ColorParticleUpdater, LuaColorParticleUpdaterBuilder::build );

	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::PointSpriteParticleRenderer, LuaPointSpriteParticleRendererBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::OrientedQuadParticleRenderer, LuaOrientedQuadParticleRendererBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::NodeParticleRenderer, LuaNodeParticleRendererBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::AnimatedSpriteParticleRenderer, LuaAnimatedSpriteParticleRendererBuilder::build );

    CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::navigation::NavigationController, LuaNavigationControllerBuilder::build );
    CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::navigation::NavigationMeshContainer, LuaNavigationMeshContainerBuilder::build );

	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::DebugRenderComponent, LuaDebugRenderComponentBuilder::build );

	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::actions::AnimateContextValue, LuaAnimateContextValueBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::actions::AnimateParticleSystem, LuaAnimateParticleSystemBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::actions::AnimateSettingValue, LuaAnimateSettingValueBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::actions::ClearTargets, LuaClearTargetsBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::actions::CopyTransformFromTarget, LuaCopyTransformFromTargetBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::actions::ResetNavigation, LuaResetNavigationBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::actions::EnableNode, LuaEnableNodeBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::actions::ExecuteBehavior, LuaExecuteBehaviorBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::actions::ExecuteBehaviorOnTarget, LuaExecuteBehaviorOnTargetBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::actions::FindTarget, LuaFindTargetBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::actions::LoadScene, LuaLoadSceneBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::actions::LookAt, LuaLookAtBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_DEFAULT_BUILDER( crimild::behaviors::actions::MotionApply );
	CRIMILD_SCRIPTING_REGISTER_DEFAULT_BUILDER( crimild::behaviors::actions::MotionAvoidOthers );
	CRIMILD_SCRIPTING_REGISTER_DEFAULT_BUILDER( crimild::behaviors::actions::MotionAvoidWalls );
	CRIMILD_SCRIPTING_REGISTER_DEFAULT_BUILDER( crimild::behaviors::actions::MotionComputePathToTarget );
	CRIMILD_SCRIPTING_REGISTER_DEFAULT_BUILDER( crimild::behaviors::actions::MotionReset );
	CRIMILD_SCRIPTING_REGISTER_DEFAULT_BUILDER( crimild::behaviors::actions::MotionSeek );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::actions::PrintMessage, LuaPrintMessageBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::actions::SetContextValue, LuaSetContextValueBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::actions::SetSettingValue, LuaSetSettingValueBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::actions::Success, LuaSuccessBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::actions::Transform, LuaTransformBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::actions::Trigger, LuaTriggerBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::actions::Wait, LuaWaitBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::composites::Parallel, LuaParallelBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::composites::Selector, LuaSelectorBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::composites::Sequence, LuaSequenceBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::conditions::DistanceToTarget, LuaDistanceToTargetBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::conditions::HasTargets, LuaHasTargetsBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::conditions::IsAtTarget, LuaIsAtTargetBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::conditions::TestContextValue, LuaTestContextValueBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::conditions::TestInputAxis, LuaTestInputAxisBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::conditions::TestSettingValue, LuaTestSettingValueBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::decorators::Inverter, LuaInverterBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::decorators::Repeat, LuaRepeatBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::decorators::RepeatUntilFail, LuaRepeatUntilFailBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::decorators::RepeatUntilSuccess, LuaRepeatUntilSuccessBuilder::build );
	CRIMILD_SCRIPTING_REGISTER_CUSTOM_BUILDER( crimild::behaviors::decorators::Succeeder, LuaSucceederBuilder::build );

    auto self = this;
    
    // TODO: Use RTTI for getting class type name
    LuaNodeBuilderRegistry::getInstance()->registerCustomNodeBuilder( GROUP_TYPE, [self]( ScriptEvaluator &eval ) -> SharedPointer< Node > {
        SharedPointer< Group > group;

        std::string filename;
        if ( eval.getPropValue( NODE_FILENAME, filename ) && filename != "" ) {
#ifdef CRIMILD_SCRIPTING_LOG_VERBOSE
            Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Building node" );
#endif
            
            auto scene = AssetManager::getInstance()->get< Group >( filename );
            if ( scene == nullptr ) {
                SharedPointer< Group > tmp;
                if ( StringUtils::getFileExtension( filename ) == ".crimild" ) {
                    FileStream is( FileSystem::getInstance().pathForResource( filename ), FileStream::OpenMode::READ );
                    is.load();
                    if ( is.getObjectCount() > 0 ) {
                        tmp = is.getObjectAt< Group >( 0 );
                    }
                }
                else if ( StringUtils::getFileExtension( filename ) == ".obj" ) {
                    OBJLoader loader( FileSystem::getInstance().pathForResource( filename ) );
                    tmp = loader.load();
                }
#ifdef CRIMILD_ENABLE_IMPORT
                else {
                    SceneImporter importer;
                    tmp = importer.import( FileSystem::getInstance().pathForResource( filename ) );
                }
#endif
                AssetManager::getInstance()->set( filename, tmp );
                scene = crimild::get_ptr( tmp );
            }

            // always copy assets from the cache
            ShallowCopy shallowCopy;
            scene->perform( shallowCopy );
            group = shallowCopy.getResult< Group >();
        }
        else {
#ifdef CRIMILD_SCRIPTING_LOG_VERBOSE
            Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Building 'group' node" );
#endif
            group = std::make_shared< Group >();
        }

		crimild::Real32 radius;
		if ( eval.getPropValue( NODE_RADIUS, radius ) ) {
			group->setLocalBound( crimild::alloc< SphereBoundingVolume >( Sphere3f( Vector3f::ZERO, radius ) ) );
			group->setWorldBound( crimild::alloc< SphereBoundingVolume >( Sphere3f( Vector3f::ZERO, radius ) ) );
		}
        
        eval.foreach( GROUP_NODES, [&]( ScriptEvaluator &childEval, int ) {
            self->buildNode( childEval, crimild::get_ptr( group ) );
        });
        
        return group;
    });
    
    // TODO: Use RTTI for getting class type name
    LuaNodeBuilderRegistry::getInstance()->registerCustomNodeBuilder( CAMERA_TYPE, [self]( ScriptEvaluator &eval ) -> SharedPointer< Node > {
        auto camera = crimild::alloc< Camera >();
        
//        std::string renderPassType;
//        eval.getPropValue( CAMERA_RENDER_PASS, renderPassType, "forward" );
//        if ( renderPassType == "basic" ) {
//            camera->setRenderPass( crimild::alloc< BasicRenderPass >() );
//        }
        
        float fov = 45.0f;
        float aspect = 4.0f / 3.0f;
        float near = 0.1f;
        float far = 1000.0f;
		bool isMainCamera = false;
        
        eval.getPropValue( CAMERA_FRUSTUM_FOV, fov );
        eval.getPropValue( CAMERA_FRUSTUM_ASPECT, aspect );
        eval.getPropValue( CAMERA_FRUSTUM_NEAR, near );
        eval.getPropValue( CAMERA_FRUSTUM_FAR, far );

        camera->setFrustum( Frustumf( fov, aspect, near, far ) );
        
		eval.getPropValue( CAMERA_MAIN_CAMERA, isMainCamera );
		camera->setIsMainCamera( isMainCamera );

        eval.foreach( GROUP_NODES, [self, camera]( ScriptEvaluator &child, int ) {
            self->buildNode( child, crimild::get_ptr( camera ) );
        });

        bool cullingEnabled = true;
        if ( eval.getPropValue( CAMERA_CULLING_ENABLED, cullingEnabled ) ) {
            camera->setCullingEnabled( cullingEnabled );
        }
        
        return camera;
    });
    
    // TODO: Use RTTI for getting class type name
    LuaNodeBuilderRegistry::getInstance()->registerCustomNodeBuilder( LIGHT_TYPE, [self]( ScriptEvaluator &eval ) -> SharedPointer< Node > {
        Light::Type lightType = Light::Type::POINT;
        std::string lightTypeStr;
        if ( eval.getPropValue( LIGHT_LIGHT_TYPE, lightTypeStr ) ) {
            if ( lightTypeStr == "directional" ) {
                lightType = Light::Type::DIRECTIONAL;
            }
            else if ( lightTypeStr == "spot" ) {
                lightType = Light::Type::SPOT;
            }
        }

        auto light = crimild::alloc< Light >( lightType );

        bool castShadows;
        if ( eval.getPropValue( LIGHT_CAST_SHADOWS, castShadows ) ) {
			light->setShadowMap( crimild::alloc< ShadowMap >() );
        }

		/*
        float shadowNearCoeff;
        if ( eval.getPropValue( LIGHT_SHADOW_NEAR_COEFF, shadowNearCoeff ) ) {
            light->setShadowNearCoeff( shadowNearCoeff );
        }
        
        float shadowFarCoeff;
        if ( eval.getPropValue( LIGHT_SHADOW_FAR_COEFF, shadowFarCoeff ) ) {
            light->setShadowFarCoeff( shadowFarCoeff );
        }
		*/
        
        Vector3f attenuation;
        if ( eval.getPropValue( LIGHT_ATTENUATION, attenuation ) ) {
            light->setAttenuation( attenuation );
        }

        RGBAColorf color;
        if ( eval.getPropValue( LIGHT_COLOR, color ) ) light->setColor( color );
        
        RGBAColorf ambient;
        if ( eval.getPropValue( LIGHT_AMBIENT, ambient ) ) light->setAmbient( ambient );

        return light;
    });

    // TODO: Use RTTI for getting class type name
    LuaNodeBuilderRegistry::getInstance()->registerCustomNodeBuilder( TEXT_TYPE, [self]( ScriptEvaluator &eval ) -> SharedPointer< Node > {
        auto text = crimild::alloc< Text >();
        
        std::string fontName;
        eval.getPropValue( "font", fontName, "" );
        
        float textSize;
        eval.getPropValue( "textSize", textSize );
        
        std::string fontDefFileName = FileSystem::getInstance().pathForResource( fontName + ".txt" );
        auto font = crimild::alloc< Font >( fontDefFileName );
        
        text->setFont( font );
        text->setSize( textSize );
        
        std::string content;
        if ( eval.getPropValue( TEXT_TEXT, content ) ) {
            text->setText( content );
        }
        
        bool renderOnScreen;
        if ( eval.getPropValue( "renderOnScreen", renderOnScreen ) ) {
            text->getComponent< RenderStateComponent >()->setRenderOnScreen( renderOnScreen );
        }
        
        RGBAColorf textColor;
        if ( eval.getPropValue( "textColor", textColor ) ) {
            text->setTextColor( textColor );
        }
        
        bool enableDepthTest;
        if ( eval.getPropValue( "enableDepthTest", enableDepthTest, true ) ) {
            text->setDepthTestEnabled( enableDepthTest );
        }
        
        std::string anchor;
        if ( eval.getPropValue( "textAnchor", anchor, "left" ) ) {
            if ( anchor != "left" ) {
                text->updateModelBounds();
                auto min = text->getLocalBound()->getMin();
                auto max = text->getLocalBound()->getMax();
                auto diff = max - min;
            
                if ( anchor == "center" ) {
                    text->local().translate() += Vector3f( -0.5f * diff[ 0 ], 0.0f, 0.0f );
                }
                else if ( anchor == "right" ) {
                    text->local().translate() += Vector3f( -diff[ 0 ], 0.0f, 0.0f );
                }
            }
        }
        
        return text;
    });

#if 0
    // TODO: Use RTTI for getting class type name
    LuaNodeBuilderRegistry::getInstance()->registerCustomNodeBuilder( PARTICLE_SYSTEM_TYPE, [self]( ScriptEvaluator &eval ) -> SharedPointer< Node > {
        auto ps = crimild::alloc< ParticleSystem >();

        float maxParticles;
        if ( eval.getPropValue( "maxParticles", maxParticles ) ) ps->setMaxParticles( maxParticles );

        float particleLifetime;
        if ( eval.getPropValue( "particleLifetime", particleLifetime ) ) ps->setParticleLifetime( particleLifetime );

        float particleSpeed;
        if ( eval.getPropValue( "particleSpeed", particleSpeed ) ) ps->setParticleSpeed( particleSpeed );

        float particleStartSize;
        if ( eval.getPropValue( "particleStartSize", particleStartSize ) ) ps->setParticleStartSize( particleStartSize );

        float particleEndSize;
        if ( eval.getPropValue( "particleEndSize", particleEndSize ) ) ps->setParticleEndSize( particleEndSize );

        RGBAColorf particleStartColor;
        if ( eval.getPropValue( "particleStartColor", particleStartColor ) ) ps->setParticleStartColor( particleStartColor );

        RGBAColorf particleEndColor;
        if ( eval.getPropValue( "particleEndColor", particleEndColor ) ) ps->setParticleEndColor( particleEndColor );

        bool useWorldSpace;
        if ( eval.getPropValue( "useWorldSpace", useWorldSpace ) ) ps->setUseWorldSpace( useWorldSpace );

        std::string emitterType;
        if ( eval.getPropValue( "emitter.type", emitterType ) ) {
            SharedPointer< ParticleEmitter > emitter;
            if ( emitterType == "cone" ) {
                float height = 1.0f;
                eval.getPropValue( "emitter.height", height );

                float radius = 1.0f;
                eval.getPropValue( "emitter.radius", radius );

                emitter = crimild::alloc< ConeParticleEmitter >( height, radius );
            }
            else if ( emitterType == "cylinder" ) {
                float height = 1.0f;
                eval.getPropValue( "emitter.height", height );

                float radius = 1.0f;
                eval.getPropValue( "emitter.radius", radius );

                emitter = crimild::alloc< CylinderParticleEmitter >( height, radius );
            }
            else if ( emitterType == "sphere" ) {
                float radius = 1.0f;
                eval.getPropValue( "emitter.radius", radius );

                emitter = crimild::alloc< SphereParticleEmitter >( radius );
            }

            if ( emitter != nullptr ) {
                Transformation t;
                if ( eval.getPropValue( "emitter.transformation", t ) ) emitter->setTransformation( t );

                ps->setEmitter( emitter );
            }
        }

        bool precomputeParticles;
        if ( eval.getPropValue( "precomputeParticles", precomputeParticles ) ) ps->setPreComputeParticles( precomputeParticles );

        std::string textureFileName;
        if ( eval.getPropValue( "texture", textureFileName ) ) {
            ps->setTexture( crimild::retain( AssetManager::getInstance()->get< Texture >( textureFileName ) ) );
        }

        // auto psEmitter = crimild::alloc< ConeParticleEmitter >( 1.0f, 0.25f );
        // Transformation t;
        // t.rotate().fromAxisAngle( Vector3f( 1.0f, 0.0f, 0.0f ), Numericf::PI );
        // psEmitter->setTransformation( t );
        // auto psEmitter = crimild::alloc< CylinderParticleEmitter >( 0.1f, 0.5f );
        // auto psEmitter = crimild::alloc< SphereParticleEmitter >( 1.0f );

        ps->generate();

        return ps;
    });

#endif

#ifdef CRIMILD_ENABLE_PHYSICS
    // TODO: Use RTTI for getting class type name
    LuaComponentBuilderRegistry::getInstance()->registerCustomComponentBuilder( PHYSICS_RIGID_BODY_TYPE, [self]( ScriptEvaluator &eval ) -> SharedPointer< NodeComponent > {
        auto rigidBody = crimild::alloc< RigidBodyComponent >();

        float mass = 0.0f;
        eval.getPropValue( "mass", mass );
        rigidBody->setMass( mass );

        bool kinematic = false;
        eval.getPropValue( "kinematic", kinematic );
        rigidBody->setKinematic( kinematic );

        Vector3f linearVelocity( 0.0f, 0.0f, 0.0f );
        eval.getPropValue( "linearVelocity", linearVelocity );
        rigidBody->setLinearVelocity( linearVelocity );

        Vector3f linearFactor( 1.0f, 1.0f, 1.0f );
        eval.getPropValue( "linearFactor", linearFactor );
        rigidBody->setLinearFactor( linearFactor );

        Vector3f angularFactor( 0.0f, 0.0f, 0.0f );
        eval.getPropValue( "angularFactor", angularFactor );
        rigidBody->setAngularFactor( angularFactor );

        bool constraintVelocity = false;
        eval.getPropValue( "constraintVelocity", constraintVelocity );
        rigidBody->setConstraintVelocity( constraintVelocity );

        float restitution;
        if ( eval.getPropValue( "restitution", restitution ) ) {
            rigidBody->setRestitution( restitution );
        }

        float friction;
        if ( eval.getPropValue( "friction", friction ) ) {
            rigidBody->setFriction( friction );
        }

        return rigidBody;
    });

    // TODO: Use RTTI for getting class type name
    LuaComponentBuilderRegistry::getInstance()->registerCustomComponentBuilder( PHYSICS_BOX_COLLIDER_TYPE, [self]( ScriptEvaluator &eval ) -> SharedPointer< NodeComponent > {
        auto boxCollider = crimild::alloc< BoxCollider >();

        Vector3f boxHalfExtents;
        if ( eval.getPropValue( "boxHalfExtents", boxHalfExtents ) ) {
            boxCollider->setHalfExtents( boxHalfExtents );
        }

        Vector3f offset;
        if ( eval.getPropValue( "offset", offset ) ) {
            boxCollider->setOffset( offset );
        }

        return boxCollider;
    });

    // TODO: Use RTTI for getting class type name
    LuaComponentBuilderRegistry::getInstance()->registerCustomComponentBuilder( PHYSICS_MESH_COLLIDER_TYPE, [self]( ScriptEvaluator &eval ) -> SharedPointer< NodeComponent > {
        auto collider = crimild::alloc< MeshCollider >();
        return collider;
    });

    // TODO: Use RTTI for getting class type name
    LuaComponentBuilderRegistry::getInstance()->registerCustomComponentBuilder( PHYSICS_CONVEX_HULL_COLLIDER_TYPE, [self]( ScriptEvaluator &eval ) -> SharedPointer< NodeComponent > {
        auto collider = crimild::alloc< ConvexHullCollider >();
        return collider;
    });

    // TODO: Use RTTI for getting class type name
    LuaComponentBuilderRegistry::getInstance()->registerCustomComponentBuilder( PHYSICS_CAPSULE_COLLIDER_TYPE, [self]( ScriptEvaluator &eval ) -> SharedPointer< NodeComponent > {
        auto collider = crimild::alloc< CapsuleCollider >();

        float width;
        if ( eval.getPropValue( "width", width ) ) {
            collider->setWidth( width );
        }

        float height;
        if ( eval.getPropValue( "height", height ) ) {
            collider->setHeight( height );
        }

        Vector3f offset;
        if ( eval.getPropValue( "offset", offset ) ) {
            collider->setOffset( offset );
        }

        return collider;
    });

    // TODO: Use RTTI for getting class type name
    LuaComponentBuilderRegistry::getInstance()->registerCustomComponentBuilder( PHYSICS_CHARACTER_CONTROLLER_TYPE, [self]( ScriptEvaluator &eval ) -> SharedPointer< NodeComponent > {
        auto characterController = crimild::alloc< CharacterController >();
        return characterController;
    });
#endif

    LuaNodeBuilderRegistry::getInstance()->flush();
    LuaComponentBuilderRegistry::getInstance()->flush();
	LuaObjectBuilderRegistry::getInstance()->flush();
}

LuaSceneBuilder::~LuaSceneBuilder( void )
{

}

void LuaSceneBuilder::reset( void )
{
	getScriptContext().reset();
}

SharedPointer< Node > LuaSceneBuilder::fromFile( const std::string &filename )
{
	if ( !getScriptContext().load( filename ) ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot open scene file ", filename );
        return nullptr;
	}

    Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Loading scene from ", filename );

    ScriptEvaluator eval( &getScriptContext(), _rootNodeName );
    
	return buildNode( eval, nullptr );
}

SharedPointer< Node > LuaSceneBuilder::buildNode( ScriptEvaluator &eval, Group *parent )
{
    std::string type;
    eval.getPropValue( NODE_TYPE, type, "crimild::Group" ); // build Group instances by default
    
    auto builder = LuaNodeBuilderRegistry::getInstance()->getBuilder( type );
    if ( builder == nullptr ) {
        Log::warning( CRIMILD_CURRENT_CLASS_NAME, "No registered builder for type '", type, "'" );
        return nullptr;
    }
    
    Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Building node with type '", type, "'" );
    auto current = builder( eval );
    if ( current == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot builder node of type '", type, "'" );
        return nullptr;
    }

    std::string name;
    if ( eval.getPropValue( "name", name ) ) {
        current->setName( name );
    }
    
    setTransformation( eval, current );
    buildNodeComponents( eval, current );

    if ( parent != nullptr ) {
        parent->attachNode( current );
    }

	return current;
}

void LuaSceneBuilder::setTransformation( ScriptEvaluator &eval, SharedPointer< Node > const &node )
{
    Transformation t;
    if ( eval.getPropValue( NODE_TRANSFORMATION, t ) ) {
        // this is related with Text nodes and their anchors
        node->local().translate() += t.translate();
        node->local().setRotate( t.getRotate() );
        node->local().setScale( t.getScale() );
    }
}

void LuaSceneBuilder::buildNodeComponents( ScriptEvaluator &eval, SharedPointer< Node > const &node )
{
	eval.foreach( NODE_COMPONENTS, [&]( ScriptEvaluator &componentEval, int ) {
        std::string type;
        if ( componentEval.getPropValue( NODE_COMPONENT_TYPE, type ) ) {
#ifdef CRIMILD_SCRIPTING_LOG_VERBOSE
            Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Building component of type '", type, "'" );
#endif

			SharedPointer< NodeComponent > cmp;
			
            auto builder = LuaComponentBuilderRegistry::getInstance()->getBuilder( type );
            if ( builder != nullptr ) {
				cmp = builder( componentEval );
			}
			else {
				auto objBuilder = LuaObjectBuilderRegistry::getInstance()->getBuilder( type );
				if ( objBuilder != nullptr ) {
					cmp = crimild::cast_ptr< NodeComponent >( objBuilder( componentEval ) );
				}
				else {
					Log::warning( CRIMILD_CURRENT_CLASS_NAME, "Cannot find component builder for type '", type, "'" );
					return;
				}
            }
            
            if ( cmp == nullptr ) {
                Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot build component of type '", type, "'" );
                return;
            }
            
            node->attachComponent( cmp );
        }
        else {
            Log::error( CRIMILD_CURRENT_CLASS_NAME, "No component type provided" );
        }
	});
}

