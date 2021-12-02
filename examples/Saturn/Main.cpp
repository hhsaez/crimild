/*
 * Copyright (c) 2002 - present, H. Hernan Saez
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

#include <Crimild.hpp>

using namespace crimild;

#define OPTION_NO_INSTANCING 0
#define OPTION_INSTANCING_LIT 1
#define OPTION_INSTANCING_UNLIT 2
#define OPTION_PARTICLES 3

SharedPointer< Node > buildPlanet( void )
{
    OBJLoader loader( FileSystem::getInstance().pathForResource( "assets/models/planet.obj" ) );
    auto model = loader.load();
    if ( model == nullptr ) {
        CRIMILD_LOG_ERROR( "Failed to load model" );
        return nullptr;
    }

    model->setLocal( translation( 0, 0, 0 ) * scale( 5 ) );
    return model;
}

SharedPointer< Node > buildAsteroids( void )
{
    auto modelFileName = "assets/models/rock.obj";
    OBJLoader loader( FileSystem::getInstance().pathForResource( modelFileName ) );
    auto asteroids = loader.load();

    auto particles = crimild::alloc< ParticleData >( 6000 );
    auto ps = asteroids->attachComponent< ParticleSystemComponent >( particles );
    ps->setEmitRate( particles->getParticleCount() );
    ps->setBurst( true );
    ps->addGenerator( crimild::alloc< OrbitPositionParticleGenerator >( 25.0f, 0.5f, 5.0f, Vector3f { 1.25f, 1.0f, 1.0f } ) );
    ps->addGenerator( crimild::alloc< UniformScaleParticleGenerator >( 0.025f, 0.15f ) );
    ps->addGenerator( crimild::alloc< EulerAnglesParticleGenerator >() );
    ps->addUpdater( crimild::alloc< CameraSortParticleUpdater >() );
    // ps->addRenderer( crimild::alloc< InstancedParticleRenderer >() );

    return asteroids;
}

SharedPointer< Node > buildAsteroids( int options )
{
    OBJLoader loader( FileSystem::getInstance().pathForResource( "assets/models/rock.obj" ) );
    auto model = loader.load();
    if ( model == nullptr ) {
        return nullptr;
    }

    auto group = crimild::alloc< Group >();

    crimild::Size count = 1000;

    auto radius = 50.0f;
    auto offset = 10.0f;

    auto material = crimild::alloc< materials::PrincipledBSDF >();

    for ( crimild::Size i = 0; i < count; ++i ) {
        // Transformation t;

        // translation
        auto theta = ( crimild::Real32 ) i / ( crimild::Real32 ) count * Numericf::TWO_PI;
        auto x = Numericf::sin( theta ) * radius + Random::generate< crimild::Real32 >( -offset, offset );
        auto y = 0.4f * Random::generate< crimild::Real32 >( -offset, offset );
        auto z = Numericf::cos( theta ) * radius + Random::generate< crimild::Real32 >( -offset, offset );
        // t.setTranslate( x, y, z );

        auto s = Random::generate< crimild::Real32 >( 0.05f, 0.5f );
        // t.setScale( s );

        auto angle = Random::generate< crimild::Real32 >( 0, Numericf::TWO_PI );
        // t.rotate().fromAxisAngle( Vector3f( 0.4f, 0.8f, 0.6f ).getNormalized(), angle );

        const auto T = translation( x, y, z ) * rotation( normalize( Vector3 { 0.4, 0.8, 0.6 } ), angle ) * scale( s );

        ShallowCopy copy;
        model->perform( copy );
        auto asteroid = copy.getResult< Node >();
        // auto asteroid = crimild::alloc< Geometry >();
        // asteroid->attachPrimitive( SpherePrimitive::UNIT_SPHERE );
        // asteroid->attachComponent< MaterialComponent >( material );
        asteroid->setLocal( T );
        group->attachNode( asteroid );
    }

    // if ( options == OPTION_NO_INSTANCING ) {
    //     for ( const auto &t : ts ) {
    //         ShallowCopy copy;
    //         model->perform( copy );
    //         auto asteroid = copy.getResult< Node >();
    //         asteroid->setLocal( t );
    //         group->attachNode( asteroid );
    //     }
    // } else {
    //     auto modelBO = crimild::alloc< Matrix4fInstancedBufferObject >( count, nullptr );
    //     for ( crimild::Size i = 0; i < count; i++ ) {
    //         modelBO->set( i, ts[ i ].computeModelMatrix() );
    //     }
    //     group->attachNode( model );

    //     group->perform( ApplyToGeometries( [ modelBO, options ]( Geometry *g ) {
    //         g->forEachPrimitive( [ modelBO ]( Primitive *p ) {
    //             p->setInstancedBuffer( modelBO );
    //         } );

    //         SharedPointer< ShaderProgram > program;
    //         if ( options == OPTION_INSTANCING_LIT ) {
    //             program = crimild::alloc< ForwardShadingShaderProgram >( true );
    //         } else {
    //             program = crimild::alloc< UnlitShaderProgram >( true );
    //         }

    //         g->getComponent< MaterialComponent >()->forEachMaterial( [ program ]( Material *m ) {
    //             m->setProgram( program );
    //         } );
    //         g->setCullMode( Node::CullMode::NEVER );
    //     } ) );
    // }

    return group;
}

SharedPointer< Group > buildScene( int options )
{
    auto scene = crimild::alloc< Group >();

    // if ( options == OPTION_PARTICLES ) {
    // scene->attachNode( buildAsteroids() );
    // } else {
    scene->attachNode( buildAsteroids( options ) );
    // }

    scene->attachNode( buildPlanet() );

    scene->attachNode(
        crimild::alloc< Skybox >(
            [] {
                auto texture = crimild::alloc< Texture >();
                texture->imageView = [ & ] {
                    auto imageView = crimild::alloc< ImageView >();
                    imageView->image = ImageManager::getInstance()->loadCubemap(
                        {
                            .filePaths = {
                                { .path = "assets/textures/right.tga" },
                                { .path = "assets/textures/left.tga" },
                                { .path = "assets/textures/bottom.tga" },
                                { .path = "assets/textures/top.tga" },
                                { .path = "assets/textures/back.tga" },
                                { .path = "assets/textures/front.tga" },
                            },
                        } );
                    return imageView;
                }();
                texture->sampler = [ & ] {
                    auto sampler = crimild::alloc< Sampler >();
                    sampler->setMinFilter( Sampler::Filter::LINEAR );
                    sampler->setMagFilter( Sampler::Filter::LINEAR );
                    sampler->setWrapMode( Sampler::WrapMode::CLAMP_TO_BORDER );
                    sampler->setCompareOp( CompareOp::NEVER );
                    return sampler;
                }();
                return texture;
            }() ) );

    scene->attachNode( [] {
        auto light = crimild::alloc< Light >( Light::Type::DIRECTIONAL );
        light->setColor( ColorRGBA { 1.0f, 1.0f, 0.8f, 1 } );
        // light->setLocal( rotation( -0.25f, -Numericf::HALF_PI, 0.0f ) );
        light->setLocal(
            lookAt(
                Point3 { 1, 1, 1 },
                Point3 { 0, 0, 0 },
                Vector3 { 0, 1, 0 } ) );
        light->setCastShadows( true );
        return light;
    }() );

    scene->attachNode( [] {
        auto camera = crimild::alloc< Camera >();
        camera->setLocal(
            lookAt(
                Point3 { 0, 30, 100 },
                Point3 { 0, 0, 0 },
                Vector3 { 0, 1, 0 } ) );
        camera->attachComponent< FreeLookCameraComponent >();

        /*
		auto graph = crimild::alloc< RenderGraph >();
        auto depthPass = graph->createPass< DepthPass >();
		auto scenePass = graph->createPass< passes::ForwardLightingPass >();
        auto shadowPass = graph->createPass< passes::ShadowPass >();
		auto skyboxPass = graph->createPass< SkyboxPass >();

        scenePass->setDepthInput( depthPass->getDepthOutput() );
		scenePass->setShadowInput( shadowPass->getShadowOutput() );
		skyboxPass->setDepthInput( depthPass->getDepthOutput() );
		
		auto blend = graph->createPass< BlendPass >();
		blend->addInput( skyboxPass->getColorOutput() );
		blend->addInput( scenePass->getColorOutput() );
		
		graph->setOutput( blend->getOutput() );

		auto debugMode = false;
		if ( debugMode ) {
			auto linearizeDepthPass = graph->createPass< LinearizeDepthPass >();
			linearizeDepthPass->setInput( depthPass->getDepthOutput() );
			
			auto shadowMap = graph->createPass< TextureColorPass >( TextureColorPass::Mode::RED );
			shadowMap->setInput( shadowPass->getShadowOutput() );
			
			auto debugPass = graph->createPass< FrameDebugPass >();
			debugPass->addInput( graph->getOutput() );
			debugPass->addInput( shadowMap->getOutput() );
			debugPass->addInput( scenePass->getColorOutput() );
			debugPass->addInput( linearizeDepthPass->getOutput() );
			debugPass->addInput( skyboxPass->getColorOutput() );
			graph->setOutput( debugPass->getOutput() );
		}
		
		camera->setRenderGraph( graph );
		*/

        return camera;
    }() );

    // scene->attachNode( crimild::alloc< Skybox >( ColorRGB { 0.5, 0.5, 0.5 } ) );

    scene->perform( StartComponents() );
    scene->perform( UpdateWorldState() );

    return scene;
}

// int main( int argc, char **argv )
// {
// 	crimild::init();

//     auto settings = crimild::alloc< Settings >( argc, argv );
// 	settings->set( "video.width", 1920 );
// 	settings->set( "video.height", 1080 );
// 	settings->set( "video.fullscreen", true );
//     settings->set( "video.show_frame_time", true );

//     CRIMILD_SIMULATION_LIFETIME auto sim = crimild::alloc< sdl::SDLSimulation >( "Saturn", settings );

//     auto scene = buildScene( OPTION_PARTICLES );
//     sim->setScene( scene );

// 	sim->registerMessageHandler< crimild::messaging::KeyReleased >( []( crimild::messaging::KeyReleased const &msg ) {
// 		switch ( msg.key ) {
// 			case CRIMILD_INPUT_KEY_U:
// 				crimild::concurrency::sync_frame( [] {
// 					Simulation::getInstance()->setScene( buildScene( OPTION_PARTICLES ) );
// 				});
// 				break;

// 			case CRIMILD_INPUT_KEY_I:
// 				crimild::concurrency::sync_frame( [] {
// 					Simulation::getInstance()->setScene( buildScene( OPTION_NO_INSTANCING ) );
// 				});
// 				break;

// 			case CRIMILD_INPUT_KEY_O:
// 				crimild::concurrency::sync_frame( [] {
// 					Simulation::getInstance()->setScene( buildScene( OPTION_INSTANCING_LIT ) );
// 				});
// 				break;

// 			case CRIMILD_INPUT_KEY_P:
// 				crimild::concurrency::sync_frame( [] {
// 					Simulation::getInstance()->setScene( buildScene( OPTION_INSTANCING_UNLIT ) );
// 				});
// 				break;

// 			default:
// 				break;
// 		}
// 	});

// 	return sim->run();
// }

class Example : public Simulation {
public:
    virtual void onStarted( void ) noexcept override
    {
        setScene( buildScene( OPTION_PARTICLES ) );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Saturn" );
