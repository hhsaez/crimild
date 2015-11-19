#include "LuaSceneBuilder.hpp"

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

#define GROUP_TYPE "crimild::Group"
#define GROUP_NODES "nodes"

#define CAMERA_TYPE "crimild::Camera"
#define CAMERA_RENDER_PASS "renderPass"
#define CAMERA_FRUSTUM "frustum"
#define CAMERA_FRUSTUM_FOV "frustum.fov"
#define CAMERA_FRUSTUM_ASPECT "frustum.aspect"
#define CAMERA_FRUSTUM_NEAR "frustum.near"
#define CAMERA_FRUSTUM_FAR "frustum.far"

#define LIGHT_TYPE "crimild::Light"
#define LIGHT_CAST_SHADOWS "castShadows"
#define LIGHT_SHADOW_FAR_COEFF "shadowFarCoeff"
#define LIGHT_SHADOW_NEAR_COEFF "shadowNearCoeff"
#define LIGHT_ATTENUATION "attenuation"

#define TEXT_TYPE "crimild::Text"
#define TEXT_FONT "font"
#define TEXT_SIZE "textSize"
#define TEXT_TEXT "text"

LuaNodeBuilderRegistry::LuaNodeBuilderRegistry( void )
{
    
}

LuaNodeBuilderRegistry::~LuaNodeBuilderRegistry( void )
{
    
}

void LuaNodeBuilderRegistry::flush( void )
{
    std::cout << "Available node builders: " << std::endl;
    for ( auto it : _nodeBuilders ) {
        if ( it.second != nullptr ) {
            std::cout << "\t" << it.first << std::endl;
        }
    }
}

LuaComponentBuilderRegistry::LuaComponentBuilderRegistry( void )
{
    
}

LuaComponentBuilderRegistry::~LuaComponentBuilderRegistry( void )
{
    
}

void LuaComponentBuilderRegistry::flush( void )
{
    std::cout << "Available component builders: " << std::endl;
    for ( auto it : _componentBuilders ) {
        if ( it.second != nullptr ) {
            std::cout << "\t" << it.first << std::endl;
        }
    }
}

LuaSceneBuilder::LuaSceneBuilder( std::string rootNodeName )
	: Scripted( true ),
	  _rootNodeName( rootNodeName )
{
    auto self = this;
    
    // TODO: Use RTTI for getting class type name
    LuaNodeBuilderRegistry::getInstance()->registerCustomNodeBuilder( GROUP_TYPE, [self]( ScriptEvaluator &eval ) -> SharedPointer< Node > {
        SharedPointer< Group > group;
        
        std::string filename;
        if ( eval.getPropValue( NODE_FILENAME, filename ) && filename != "" ) {
            Log::Debug << "Building node" << Log::End;
            
            auto scene = AssetManager::getInstance()->get< Group >( filename );
            if ( scene == nullptr ) {
                OBJLoader loader( FileSystem::getInstance().pathForResource( filename ) );
                auto tmp = loader.load();
                AssetManager::getInstance()->set( filename, tmp );
                group = tmp;
            }
            else {
                ShallowCopy shallowCopy;
                scene->perform( shallowCopy );
                group = shallowCopy.getResult< Group >();
            }
        }
        else {
            Log::Debug << "Building 'group' node" << Log::End;
            group = std::make_shared< Group >();
        }
        
        eval.foreach( GROUP_NODES, [&]( ScriptEvaluator &childEval, int ) {
            self->buildNode( childEval, crimild::get_ptr( group ) );
        });
        
        return group;
    });
    
    // TODO: Use RTTI for getting class type name
    LuaNodeBuilderRegistry::getInstance()->registerCustomNodeBuilder( CAMERA_TYPE, [self]( ScriptEvaluator &eval ) -> SharedPointer< Node > {
        auto camera = crimild::alloc< Camera >();
        
        std::string renderPassType;
        eval.getPropValue( CAMERA_RENDER_PASS, renderPassType, "forward" );
        if ( renderPassType == "basic" ) {
            camera->setRenderPass( crimild::alloc< BasicRenderPass >() );
        }
        
        float fov = 45.0f;
        float aspect = 4.0f / 3.0f;
        float near = 1.0f;
        float far = 1000.0f;
        
        eval.getPropValue( CAMERA_FRUSTUM_FOV, fov );
        eval.getPropValue( CAMERA_FRUSTUM_ASPECT, aspect );
        eval.getPropValue( CAMERA_FRUSTUM_NEAR, near );
        eval.getPropValue( CAMERA_FRUSTUM_FAR, far );
        
        camera->setFrustum( Frustumf( fov, aspect, near, far ) );
        
        eval.foreach( GROUP_NODES, [self, camera]( ScriptEvaluator &child, int ) {
            self->buildNode( child, crimild::get_ptr( camera ) );
        });
        
        return camera;
    });
    
    // TODO: Use RTTI for getting class type name
    LuaNodeBuilderRegistry::getInstance()->registerCustomNodeBuilder( LIGHT_TYPE, [self]( ScriptEvaluator &eval ) -> SharedPointer< Node > {
        auto light = crimild::alloc< Light >();
        
        bool castShadows;
        if ( eval.getPropValue( LIGHT_CAST_SHADOWS, castShadows ) ) {
            light->setCastShadows( castShadows );
        }
        
        float shadowNearCoeff;
        if ( eval.getPropValue( LIGHT_SHADOW_NEAR_COEFF, shadowNearCoeff ) ) {
            light->setShadowNearCoeff( shadowNearCoeff );
        }
        
        float shadowFarCoeff;
        if ( eval.getPropValue( LIGHT_SHADOW_FAR_COEFF, shadowFarCoeff ) ) {
            light->setShadowFarCoeff( shadowFarCoeff );
        }
        
        Vector3f attenuation;
        if ( eval.getPropValue( LIGHT_ATTENUATION, attenuation ) ) {
            light->setAttenuation( attenuation );
        }
        
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
        if ( eval.getPropValue( "enableDepthTest", enableDepthTest, false ) ) {
            text->setDepthTestEnabled( enableDepthTest );
        }
        
        std::string anchor;
        if ( eval.getPropValue( "textAnchor", anchor, "left" ) ) {
            auto min = text->getLocalBound()->getMin();
            auto max = text->getLocalBound()->getMax();
            auto diff = max - min;
            
            if ( anchor == "left" ) {
                // do nothing?
            }
            else if ( anchor == "center" ) {
                text->local().translate() += Vector3f( -0.5f * diff[ 0 ], 0.0f, 0.0f );
            }
            else if ( anchor == "right" ) {
                text->local().translate() += Vector3f( -diff[ 0 ], 0.0f, 0.0f );
            }
        }
        
        return text;
    });
    
    LuaNodeBuilderRegistry::getInstance()->flush();
    LuaComponentBuilderRegistry::getInstance()->flush();
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
		Log::Error << "Cannot open scene file " << filename << Log::End;
        return nullptr;
	}

	Log::Debug << "Loading scene from " << filename << Log::End;

    ScriptEvaluator eval( &getScriptContext(), _rootNodeName );
    
	return buildNode( eval, nullptr );
}

SharedPointer< Node > LuaSceneBuilder::buildNode( ScriptEvaluator &eval, Group *parent )
{
    std::string type;
    eval.getPropValue( NODE_TYPE, type, "crimild::Group" ); // build Group instances by default
    
    auto builder = LuaNodeBuilderRegistry::getInstance()->getBuilder( type );
    if ( builder == nullptr ) {
        Log::Warning << "No registered builder for type '" << type << "'" << Log::End;
        return nullptr;
    }
    
    Log::Debug << "Building node with type '" << type << "'" << Log::End;
    auto current = builder( eval );
    if ( current == nullptr ) {
        Log::Error << "Cannot builder node of type '" << type << "'" << Log::End;
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
	Log::Debug << "Setting node transformation" << Log::End;
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
            Log::Debug << "Building component of type '" << type << "'" << Log::End;
            auto builder = LuaComponentBuilderRegistry::getInstance()->getBuilder( type );
            if ( builder == nullptr ) {
                Log::Warning << "Cannot find component builder for type '" << type << "'" << Log::End;
                return;
            }
            
            auto cmp = builder( componentEval );
            if ( cmp == nullptr ) {
                Log::Error << "Cannot build component of type '" << type << "'" << Log::End;
                return;
            }
            
            node->attachComponent( cmp );
        }
        else {
            Log::Error << "No component type provided" << Log::End;
        }
	});
}

