#include "Parallel.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::composites;

Parallel::Parallel( void )
{

}

Parallel::~Parallel( void )
{

}

void Parallel::init( BehaviorContext *context )
{
	Composite::init( context );

	_states.clear();

	// init all children together
	for ( int i = 0; i < getBehaviorCount(); i++ ) {
		getBehaviorAt( i )->init( context );
		_states.push_back( Behavior::State::RUNNING );
	}
}

Behavior::State Parallel::step( BehaviorContext *context )
{
	auto result = Behavior::State::SUCCESS;
	
	// execute all children together
	for ( int i = 0; i < getBehaviorCount(); i++ ) {
		if ( _states[ i ] == Behavior::State::RUNNING ) {
			auto b = getBehaviorAt( i );		
			auto r = b->step( context );
			_states[ i ] = r;
			
			if ( r == Behavior::State::FAILURE ) {
				return Behavior::State::FAILURE;
			}
			else if ( r == Behavior::State::RUNNING ) {
				result = r;
			}
		}
	}

	return result;
}

