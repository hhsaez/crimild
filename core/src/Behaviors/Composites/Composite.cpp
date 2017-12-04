#include "Composite.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::composites;

Composite::Composite( void )
{

}

Composite::~Composite( void )
{

}

void Composite::init( BehaviorContext *context )
{
	Behavior::init( context );
}

void Composite::attachBehavior( BehaviorPtr const &behavior )
{
	_behaviors.push_back( behavior );
}

size_t Composite::getBehaviorCount( void ) const
{
	return _behaviors.size();
}

Behavior *Composite::getBehaviorAt( size_t index )
{
	return _behaviors[ index ].get();
}

