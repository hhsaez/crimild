#include "AnimateParticleSystem.hpp"
#include "Coding/Encoder.hpp"
#include "Coding/Decoder.hpp"

#include "SceneGraph/Group.hpp"
#include "ParticleSystem/ParticleSystemComponent.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

AnimateParticleSystem::AnimateParticleSystem( void )
{
	
}

AnimateParticleSystem::AnimateParticleSystem( crimild::Bool animate )
	: _animate( animate )
{

}

AnimateParticleSystem::~AnimateParticleSystem( void )
{
	
}

Behavior::State AnimateParticleSystem::step( BehaviorContext *context )
{
	auto agent = static_cast< Group * >( context->getAgent() );

	auto ps = agent->getComponent< ParticleSystemComponent >();
	if ( ps == nullptr ) {
		Log::error( CRIMILD_CURRENT_CLASS_NAME, "Behavior is not attached to particle system" );
		return Behavior::State::FAILURE;
	}

	ps->setAnimationEnabled( _animate );

	return Behavior::State::SUCCESS;
}

void AnimateParticleSystem::encode( coding::Encoder &encoder )
{
	Behavior::encode( encoder );

	encoder.encode( "animate", _animate );
}

void AnimateParticleSystem::decode( coding::Decoder &decoder )
{
	Behavior::decode( decoder );

	decoder.decode( "animate", _animate );
}

