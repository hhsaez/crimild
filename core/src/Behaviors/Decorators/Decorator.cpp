#include "Decorator.hpp"

#include "Crimild_Coding.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::decorators;

Decorator::Decorator( void )
{
}

Decorator::~Decorator( void )
{
}

void Decorator::init( BehaviorContext *context )
{
    Behavior::init( context );

    if ( getBehavior() != nullptr ) {
        getBehavior()->init( context );
    }
}

Behavior::State Decorator::step( BehaviorContext *context )
{
    if ( getBehavior() == nullptr ) {
        return Behavior::State::FAILURE;
    }

    return getBehavior()->step( context );
}

void Decorator::encode( coding::Encoder &encoder )
{
    Behavior::encode( encoder );

    encoder.encode( "behavior", _decoratedBehavior );
}

void Decorator::decode( coding::Decoder &decoder )
{
    Behavior::decode( decoder );

    decoder.decode( "behavior", _decoratedBehavior );
}
