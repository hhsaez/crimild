#include "Sequence.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::composites;

void Sequence::init( BehaviorContext *context )
{
    Composite::init( context );

    _currentBehavior = 0;

    if ( getBehaviorCount() > 0 ) {
        // init the first behavior
        getBehaviorAt( 0 )->init( context );
    }
}

Behavior::State Sequence::step( BehaviorContext *context )
{
    // keep looping as long as behavior succeed
    while ( _currentBehavior < getBehaviorCount() ) {
        auto current = getBehaviorAt( _currentBehavior );
        auto result = current->step( context );

        if ( result == Behavior::State::FAILURE || result == Behavior::State::RUNNING ) {
            return result;
        }

        _currentBehavior++;
        if ( _currentBehavior < getBehaviorCount() ) {
            // init the next behavior
            getBehaviorAt( _currentBehavior )->init( context );
        }
    }

    return Behavior::State::SUCCESS;
}
