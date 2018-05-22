#include "MotionAvoidOthers.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

MotionAvoidOthers::MotionAvoidOthers( void )
{

}

MotionAvoidOthers::~MotionAvoidOthers( void )
{

}

Behavior::State MotionAvoidOthers::step( BehaviorContext *context )
{
	// TODO
	return Behavior::State::SUCCESS;
}

void MotionAvoidOthers::encode( coding::Encoder &encoder )
{
	Behavior::encode( encoder );
}

void MotionAvoidOthers::decode( coding::Decoder &decoder )
{
	Behavior::decode( decoder );
}

