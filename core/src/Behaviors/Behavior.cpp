#include "Behavior.hpp"

using namespace crimild;
using namespace crimild::behaviors;

Behavior::Behavior( void )
{

}

Behavior::~Behavior( void )
{

}

void Behavior::init( BehaviorContext *context )
{

}

void Behavior::encode( coding::Encoder &encoder )
{
	Codable::encode( encoder );
}

void Behavior::decode( coding::Decoder &decoder )
{
	Codable::decode( decoder );
}

