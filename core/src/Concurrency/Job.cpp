#include "Job.hpp"

using namespace crimild;
using namespace crimild::concurrency;

Job::Job( void )
	: _callback( nullptr ),
      _parent( nullptr ),
	  _childCount( 0 )
{

}

Job::Job( JobCallback const &callback )
	: _callback( callback ),
      _parent( nullptr ),
	  _childCount( 1 )
{

}

Job::Job( Job *parent, JobCallback const &callback )
	: _callback( callback ),
      _parent( parent ),
	  _childCount( 1 )
{
	if ( _parent != nullptr ) {
		_parent->increaseChildCount();
	}
}

Job::~Job( void )
{

}

void Job::increaseChildCount( void )
{
	++_childCount;
}

void Job::decreaseChildCount( void )
{
	if ( _childCount > 0 ) {
		--_childCount;
	}
}

void Job::attachContinuation( JobContinuationCallback const &callback )
{
	_continuations.push_back( callback );
}

void Job::execute( void )
{
	if ( _callback != nullptr ) {
		_callback();
	}

	finish();
}

void Job::finish( void )
{
	decreaseChildCount();
	if ( isCompleted() ) {
		if ( _parent != nullptr ) {
			_parent->finish();
		}
	}
}

