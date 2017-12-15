#include "Behavior.hpp"

using namespace crimild;
using namespace crimild::behaviors;

BehaviorContext::BehaviorContext( void )
{

}

BehaviorContext::~BehaviorContext( void )
{
	removeAllTargets();
}

void BehaviorContext::reset( void )
{
	_agent = nullptr;
	_clock.reset();
	removeAllTargets();
}

void BehaviorContext::update( const crimild::Clock &c )
{
	_clock += c;
}

void BehaviorContext::addTarget( crimild::Node *target )
{
	// TODO: check repetitions? use set?
	_targets.push_back( target );
	++_targetCount;
}

crimild::Size BehaviorContext::getTargetCount( void ) const
{
	return _targetCount;
}

crimild::Node *BehaviorContext::getTargetAt( crimild::Size index )
{
	if ( getTargetCount() <= index ) {
		return nullptr;
	}

	return _targets[ index ];
}

void BehaviorContext::removeAllTargets( void )
{
	_targets.clear();
	_targetCount = 0;
}

void BehaviorContext::foreachTarget( BehaviorContext::TargetCallback const &callback )
{
	for ( auto &t : _targets ) {
		if ( t != nullptr ) {
			callback( t );
		}
	}
}

void BehaviorContext::dump( void ) const
{
	std::stringstream ss;

	for ( auto it : _values ) {
		ss << "\n\t\"" << it.first << "\" = \"" << it.second << "\"";
	}

	Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Behavior context dump: ", ss.str() );
}

