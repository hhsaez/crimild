#include "Success.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

Success::Success( void )
{

}

Success::~Success( void )
{

}

Behavior::State Success::step( BehaviorContext *context )
{
	return Behavior::State::SUCCESS;
}

void Success::encode( coding::Encoder &encoder )
{
	Behavior::encode( encoder );
}

void Success::decode( coding::Decoder &decoder )
{
	Behavior::decode( decoder );
}

