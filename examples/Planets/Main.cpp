/*
 * Copyright (c) 2013, Hernan Saez
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
#include <Crimild_SDL.hpp>

using namespace crimild;
using namespace crimild::sdl;

static float SCALE_FACTOR = 1.0f / 1000000.0f;
static float SIMULATION_SPEED = 1.0f / 60.0f;

static float EARTH_EQUATORIAL_ROTATION_SPEED = SIMULATION_SPEED / 86.400f;
static float EARTH_SIDERAL_PERIOD = 365.0f;
static float EARTH_SIDERAL_ROTATION_SPEED = EARTH_EQUATORIAL_ROTATION_SPEED / EARTH_SIDERAL_PERIOD;
static float EARTH_RADIUS = 6371.0f * SCALE_FACTOR;
static float AU = 149597.871f * SCALE_FACTOR / EARTH_RADIUS;
static float EARTH_DISTANCE = AU;

static float MOON_EQUATORIAL_ROTATION_SPEED = 1.04f * EARTH_EQUATORIAL_ROTATION_SPEED;
static float MOON_ORBITAL_PERIOD = 27.321f;
static float MOON_SIDERAL_ROTATION_SPEED = EARTH_EQUATORIAL_ROTATION_SPEED / MOON_ORBITAL_PERIOD;
static float MOON_RADIUS = 0.273f * EARTH_RADIUS;
static float MOON_DISTANCE = 0.005f * AU;

static float SUN_EQUATORIAL_ROTATION_SPEED = 1.0f * EARTH_EQUATORIAL_ROTATION_SPEED;
static float SUN_RADIUS = 110.0f * EARTH_RADIUS;

static float JUPITER_EQUATORIAL_ROTATION_SPEED = 398.88f * EARTH_EQUATORIAL_ROTATION_SPEED;
static float JUPITER_ORBITAL_PERIOD = 4332.59f;
static float JUPITER_SIDERAL_ROTATION_SPEED = EARTH_EQUATORIAL_ROTATION_SPEED / JUPITER_ORBITAL_PERIOD;
static float JUPITER_RADIUS = 11.209f * EARTH_RADIUS;
static float JUPITER_DISTANCE = 5.458104f * AU;

static float IO_EQUATORIAL_ROTATION_SPEED = 0.1618490205f * EARTH_EQUATORIAL_ROTATION_SPEED; // TODO: find this
static float IO_ORBITAL_PERIOD = 1.769137786f;
static float IO_SIDERAL_ROTATION_SPEED = EARTH_EQUATORIAL_ROTATION_SPEED / IO_ORBITAL_PERIOD;
static float IO_RADIUS = 0.286f * EARTH_RADIUS;
static float IO_DISTANCE = 1.25f * 0.002819f * AU;

static float EUROPA_EQUATORIAL_ROTATION_SPEED = 0.1618490205f * EARTH_EQUATORIAL_ROTATION_SPEED; // TODO: find this
static float EUROPA_ORBITAL_PERIOD = 3.551181f;
static float EUROPA_SIDERAL_ROTATION_SPEED = EARTH_EQUATORIAL_ROTATION_SPEED / EUROPA_ORBITAL_PERIOD;
static float EUROPA_RADIUS = 0.245f * EARTH_RADIUS;
static float EUROPA_DISTANCE = 2.0f * IO_DISTANCE;

static float GANYMEDE_EQUATORIAL_ROTATION_SPEED = 0.1618490205f * EARTH_EQUATORIAL_ROTATION_SPEED; // TODO: find this
static float GANYMEDE_ORBITAL_PERIOD = 7.15455296f;
static float GANYMEDE_SIDERAL_ROTATION_SPEED = EARTH_EQUATORIAL_ROTATION_SPEED / GANYMEDE_ORBITAL_PERIOD;
static float GANYMEDE_RADIUS = 0.413f * EARTH_RADIUS;
static float GANYMEDE_DISTANCE = 4.0f * IO_DISTANCE;

class RotationController : public NodeComponent {
public:
	RotationController( float rotationSpeed )
		: _rotationSpeed( rotationSpeed )
	{

	}

	virtual ~RotationController( void )
	{

	}

	virtual void update( const Clock &t )
	{
		getNode()->local().rotate().fromAxisAngle( Vector3f( 0.0f, 1.0f, 0.0f ), _time * Numericf::TWO_PI );
		_time += _rotationSpeed * SIMULATION_SPEED * t.getDeltaTime();
	}

private:
	float _time = 0.0f;
	float _rotationSpeed = 1.0f;
};

SharedPointer< Node > buildSkybox( float size, std::string colorMap )
{
	float halfSize = 0.5f * size;
	float vertices[] = {
		// back
		-halfSize, +halfSize, -halfSize, 0.25f, 0.333f,
		-halfSize, -halfSize, -halfSize, 0.25f, 0.667f,
		+halfSize, -halfSize, -halfSize, 0.5f, 0.667f,
		+halfSize, +halfSize, -halfSize, 0.5f, 0.333f,

		// left
		-halfSize, +halfSize, +halfSize, 0.0f, 0.333f,
		-halfSize, -halfSize, +halfSize, 0.0f, 0.667f,
		-halfSize, -halfSize, -halfSize, 0.25f, 0.667f,
		-halfSize, +halfSize, -halfSize, 0.25f, 0.333f,

		// right
		+halfSize, +halfSize, -halfSize, 0.5f, 0.333f,
		+halfSize, -halfSize, -halfSize, 0.5f, 0.667f,
		+halfSize, -halfSize, +halfSize, 0.75f, 0.667f,
		+halfSize, +halfSize, +halfSize, 0.75f, 0.333f,

		// front
		+halfSize, +halfSize, +halfSize, 0.75f, 0.333f,
		+halfSize, -halfSize, +halfSize, 0.75f, 0.667f,
		-halfSize, -halfSize, +halfSize, 1.0f, 0.667f,
		-halfSize, +halfSize, +halfSize, 1.0f, 0.333f,

		// top
		-halfSize, +halfSize, +halfSize, 0.25f, 0.0f,
		-halfSize, +halfSize, -halfSize, 0.25f, 0.333f,
		+halfSize, +halfSize, -halfSize, 0.5f, 0.333f,
		+halfSize, +halfSize, +halfSize, 0.5f, 0.0f,

		// bottom
		-halfSize, -halfSize, -halfSize, 0.25f, 0.667f,
		-halfSize, -halfSize, +halfSize, 0.25f, 1.0f,
		+halfSize, -halfSize, +halfSize, 0.5f, 1.0f,
		+halfSize, -halfSize, -halfSize, 0.5f, 0.667f,
	};

	unsigned short indices[] = {
		// back
		0, 1, 2,
		0, 2, 3,

		// left
		4, 5, 6,
		4, 6, 7,

		// right
		8, 9, 10,
		8, 10, 11,

		// front
		12, 13, 14,
		12, 14, 15,

		// top
		16, 17, 18,
		16, 18, 19,

		// bottom
		20, 21, 22,
		20, 22, 23,
	};

	auto primitive = crimild::alloc< Primitive >( Primitive::Type::TRIANGLES );
	primitive->setVertexBuffer( crimild::alloc< VertexBufferObject >( VertexFormat::VF_P3_UV2, 24, vertices ) );
	primitive->setIndexBuffer( crimild::alloc< IndexBufferObject >( 36, indices ) );

	auto geometry = crimild::alloc< Geometry >();
	geometry->attachPrimitive( primitive );

	auto material = crimild::alloc< Material >();
	material->setColorMap( crimild::alloc< Texture >( crimild::alloc< ImageTGA >( FileSystem::getInstance().pathForResource( colorMap ) ) ) );
	material->setEmissive( 1.0f );
	// material->setDiffuse( RGBAColorf( 1.0f, 0.0f, 0.0f, 1.0f ) );
	geometry->getComponent< MaterialComponent >()->attachMaterial( material );

	return geometry;
}

SharedPointer< Node > buildOrbit( float radius )
{
	std::vector< float > vertices;
	for ( float phi = 0.0f; phi <= Numericf::TWO_PI; phi += Numericf::TWO_PI / 60.0f ) {
		float x = radius * Numericf::cos( phi );
		float y = 0.0f;
		float z = radius * Numericf::sin( phi );
		vertices.push_back( x );
		vertices.push_back( y );
		vertices.push_back( z );
	}

	int vertexCount = vertices.size() / 3;
	std::vector< unsigned short > indices( vertexCount );
	for ( int i = 0; i < vertexCount; i++ ) {
		indices[ i ] = i;
	}

	auto geometry = crimild::alloc< Geometry >();

	auto primitive = crimild::alloc< Primitive >( Primitive::Type::LINE_LOOP );
	primitive->setVertexBuffer( crimild::alloc< VertexBufferObject >( VertexFormat::VF_P3, vertices.size() / 3, &vertices[ 0 ] ) );
	primitive->setIndexBuffer( crimild::alloc< IndexBufferObject >( indices.size(), &indices[ 0 ] ) );
	geometry->attachPrimitive( primitive );

	auto material = crimild::alloc< Material >();
	material->setDiffuse( RGBAColorf( 1.0f, 1.0f, 0.0f, 1.0f ) );
	material->setEmissive( 1.0f );
	geometry->getComponent< MaterialComponent >()->attachMaterial( material );

	return geometry;
}

SharedPointer< Group > buildPlanet( std::string name, float radius, std::string diffuse, float equatorialRotationSpeed, float orbitSize, float sideralRotationSpeed )
{
	auto geometry = crimild::alloc< Geometry >();
	geometry->attachPrimitive( crimild::alloc< SpherePrimitive >( radius, VertexFormat::VF_P3_N3_UV2 ) );

	auto planetRotationPivot = crimild::alloc< Group >();
	planetRotationPivot->attachComponent( crimild::alloc< RotationController >( equatorialRotationSpeed ) );
	planetRotationPivot->attachNode( geometry );

	auto material = crimild::alloc< Material >();
	material->setColorMap( crimild::alloc< Texture >( crimild::alloc< ImageTGA >( FileSystem::getInstance().pathForResource( diffuse ) ) ) );
	material->setSpecular( RGBAColorf( 0.0f, 0.0f, 0.0f, 0.0f ) );
	geometry->getComponent< MaterialComponent >()->attachMaterial( material );

	auto planet = crimild::alloc< Group >( name );
	planet->attachNode( planetRotationPivot );
	planet->local().setTranslate( orbitSize, 0.0f, 0.0f );

	auto planetPivot = crimild::alloc< Group >();
	planetPivot->attachComponent( crimild::alloc< RotationController >( sideralRotationSpeed ) );
	planetPivot->attachNode( planet );
	planetPivot->attachNode( buildOrbit( orbitSize ) );

	return planetPivot;
}

SharedPointer< Group > buildMoon( void )
{
	return buildPlanet( "Moon", MOON_RADIUS, "moon.tga", 0.0f, MOON_DISTANCE, MOON_SIDERAL_ROTATION_SPEED );
}

SharedPointer< Group > buildEarth( void )
{
	return buildPlanet( "Earth", EARTH_RADIUS, "earth.tga", EARTH_EQUATORIAL_ROTATION_SPEED, EARTH_DISTANCE, EARTH_SIDERAL_ROTATION_SPEED );
}

SharedPointer< Group > buildJupiter( void )
{
	return buildPlanet( "Jupiter", JUPITER_RADIUS, "jupiter.tga", JUPITER_EQUATORIAL_ROTATION_SPEED, JUPITER_DISTANCE, JUPITER_SIDERAL_ROTATION_SPEED );
}

SharedPointer< Group > buildIo( void )
{
	return buildPlanet( "Io", IO_RADIUS, "io.tga", IO_EQUATORIAL_ROTATION_SPEED, IO_DISTANCE, IO_SIDERAL_ROTATION_SPEED );
}

SharedPointer< Group > buildEuropa( void )
{
	return buildPlanet( "Europa", EUROPA_RADIUS, "europa.tga", EUROPA_EQUATORIAL_ROTATION_SPEED, EUROPA_DISTANCE, EUROPA_SIDERAL_ROTATION_SPEED );
}

SharedPointer< Group > buildGanymede( void )
{
	return buildPlanet( "Ganymede", GANYMEDE_RADIUS, "ganymede.tga", GANYMEDE_EQUATORIAL_ROTATION_SPEED, GANYMEDE_DISTANCE, GANYMEDE_SIDERAL_ROTATION_SPEED );
}

SharedPointer< Group > buildSun( void )
{
	auto sun = crimild::alloc< Group >( "Earth" );

	auto geometry = crimild::alloc< Geometry >();
	geometry->attachPrimitive( crimild::alloc< SpherePrimitive >( SUN_RADIUS, VertexFormat::VF_P3_N3_UV2 ) );
	geometry->attachComponent( crimild::alloc< RotationController >( SUN_EQUATORIAL_ROTATION_SPEED ) );

	auto material = crimild::alloc< Material >();
	material->setColorMap( crimild::alloc< Texture >( crimild::alloc< ImageTGA >( FileSystem::getInstance().pathForResource( "sun.tga" ) ) ) );
	material->setEmissive( 1.0f );
	geometry->getComponent< MaterialComponent >()->attachMaterial( material );

	sun->attachNode( geometry );

    auto light = crimild::alloc< Light >();
    sun->attachNode( light );

	return sun;
}

int main( int argc, char **argv )
{
	auto sim = crimild::alloc< SDLSimulation >( "Planets", crimild::alloc< Settings >( argc, argv ) );

	auto scene = crimild::alloc< Group >();

	auto sun = buildSun();
	scene->attachNode( sun );
	
	auto earth = buildEarth();
	sun->attachNode( earth );

	auto moon = buildMoon();
	earth->getNodeAt< Group >( 0 )->attachNode( moon );

	auto jupiter = buildJupiter();
	sun->attachNode( jupiter );

	auto io = buildIo();
	jupiter->getNodeAt< Group >( 0 )->attachNode( io );

	auto europa = buildEuropa();
	jupiter->getNodeAt< Group >( 0 )->attachNode( europa );

	auto ganymede = buildGanymede();
	jupiter->getNodeAt< Group >( 0 )->attachNode( ganymede );

	auto camera = crimild::alloc< Group >();
	auto actualCamera = crimild::alloc< Camera >( 45.0f, 4.0f / 3.0f, 0.0001f, 30.0f * AU );
	camera->attachNode( actualCamera );
	// scene->attachNode( buildSkybox( 30.0f * AU * Numericf::cos( Numericf::PI / 4.0f ), "milky_way.tga" ) );
	camera->local().setTranslate( 0.0f, 0.0f, 1.15f * AU );
	camera->local().lookAt( Vector3f( 0.0f, 0.0f, 0.0f ) );
	auto deferredRenderPass = crimild::alloc< DeferredRenderPass >();
	//deferredRenderPass->getImageEffects().add( crimild::alloc< gl3::BloomImageEffect >( 0.025f, 16.0f, 0.1f, 4.0 ) );
	//actualCamera->setRenderPass( deferredRenderPass );
	scene->attachNode( camera );

    camera->local().makeIdentity();
    camera->local().setTranslate( 0.0f, 0.25f * AU, 10.5f * AU );
    camera->local().lookAt( Vector3f( 0.0f, 0.0f, 0.0f ) );

	camera->attachComponent( crimild::alloc< LambdaComponent >( [=]( Node *, const Clock & ) {
		if ( Input::getInstance()->isKeyDown( '1' ) ) {
			camera->detachFromParent();
			camera->local().makeIdentity();
			camera->local().setTranslate( 0.0f, 0.25f * AU, 7.5f * AU );
			camera->local().lookAt( Vector3f( 0.0f, 0.0f, 0.0f ) );
			scene->attachNode( camera );
			scene->perform( UpdateWorldState() );
			scene->perform( UpdateRenderState() );
		}
		else if ( Input::getInstance()->isKeyDown( '2' ) ) {
			camera->detachFromParent();
			camera->local().makeIdentity();
			camera->local().setTranslate( 0.5f * SUN_RADIUS, 1.0f * SUN_RADIUS, 2.0f * SUN_RADIUS );
			sun->attachNode( camera );
			sun->perform( UpdateWorldState() );
			sun->perform( UpdateRenderState() );
		}
		else if ( Input::getInstance()->isKeyDown( '3' ) ) {
			camera->detachFromParent();
			camera->local().makeIdentity();
			camera->local().setTranslate( 0.1f * EARTH_RADIUS, 0.1 * EARTH_RADIUS, 2.0f * MOON_DISTANCE );
			earth->getNodeAt< Group >( 0 )->getNodeAt< Group >( 0 )->attachNode( camera );
			earth->perform( UpdateWorldState() );
			earth->perform( UpdateRenderState() );
		}
		else if ( Input::getInstance()->isKeyDown( '4' ) ) {
			camera->detachFromParent();
			camera->local().makeIdentity();
			camera->local().rotate().fromAxisAngle( Vector3f( 0.0f, 1.0f, 0.0f ), Numericf::HALF_PI );
			camera->local().setTranslate( 2.0f * MOON_RADIUS, 1.15f * MOON_RADIUS, -1.1f * MOON_RADIUS );
			moon->getNodeAt< Group >( 0 )->attachNode( camera );
			moon->perform( UpdateWorldState() );
			moon->perform( UpdateRenderState() );
		}
		else if ( Input::getInstance()->isKeyDown( '5' ) ) {
			camera->detachFromParent();
			camera->local().makeIdentity();
			camera->local().setTranslate( 0.1f * JUPITER_RADIUS, 0.1 * JUPITER_RADIUS, -2.0f * GANYMEDE_DISTANCE );
			camera->local().rotate().fromAxisAngle( Vector3f( 0.0f, 1.0f, 0.0f ), Numericf::PI );
			jupiter->getNodeAt< Group >( 0 )->attachNode( camera );
			jupiter->perform( UpdateWorldState() );
			jupiter->perform( UpdateRenderState() );
		}
		else if ( Input::getInstance()->isKeyDown( '6' ) ) {
			camera->detachFromParent();
			camera->local().makeIdentity();
			camera->local().setTranslate( 1.1f * IO_RADIUS, 1.1f * IO_RADIUS, 2.0f * IO_RADIUS );
			io->getNodeAt< Group >( 0 )->getNodeAt< Group >( 0 )->attachNode( camera );
			io->perform( UpdateWorldState() );
			io->perform( UpdateRenderState() );
		}
		else if ( Input::getInstance()->isKeyDown( '7' ) ) {
			camera->detachFromParent();
			camera->local().makeIdentity();
			camera->local().setTranslate( 1.1f * EUROPA_RADIUS, 1.1f * EUROPA_RADIUS, 2.0f * EUROPA_RADIUS );
			europa->getNodeAt< Group >( 0 )->getNodeAt< Group >( 0 )->attachNode( camera );
			europa->perform( UpdateWorldState() );
			europa->perform( UpdateRenderState() );
		}
		else if ( Input::getInstance()->isKeyDown( '8' ) ) {
			camera->detachFromParent();
			camera->local().makeIdentity();
			camera->local().setTranslate( 1.1f * GANYMEDE_RADIUS, 1.1f * GANYMEDE_RADIUS, 2.0f * GANYMEDE_RADIUS );
			ganymede->getNodeAt< Group >( 0 )->getNodeAt< Group >( 0 )->attachNode( camera );
			ganymede->perform( UpdateWorldState() );
			ganymede->perform( UpdateRenderState() );
		}
		else if ( Input::getInstance()->isKeyDown( '0' ) ) {
			deferredRenderPass->enableDebugMode( !deferredRenderPass->isDebugModeEnabled() );
		}
	}));

	scene->attachComponent( crimild::alloc< LambdaComponent >( [=]( Node *, const Clock & ) {
		if ( Input::getInstance()->isKeyDown( '-' ) ) {
			SIMULATION_SPEED -= Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_LEFT_SHIFT ) ? 10.0 : 0.001f;
		}
		else if ( Input::getInstance()->isKeyDown( '=' ) ) {
			SIMULATION_SPEED += Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_LEFT_SHIFT ) ? 10.0 : 0.001f;
		}
		else if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_SPACE ) ) {
			SIMULATION_SPEED = 0.0f;
		}
		else if ( Input::getInstance()->isKeyDown( 'R' ) ) {
			SIMULATION_SPEED = 1.0f / 60.0f;
		}
	}));

	sim->setScene( scene );

	return sim->run();
}

