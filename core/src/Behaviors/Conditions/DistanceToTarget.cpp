#include "DistanceToTarget.hpp"

#include "Mathematics/Distance.hpp"
#include "SceneGraph/Node.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::conditions;

DistanceToTarget::DistanceToTarget( crimild::Real32 value, std::string comparator )
	: _value( value )
{
	setComparator( comparator );
}

DistanceToTarget::~DistanceToTarget( void )
{

}

void DistanceToTarget::setComparator( std::string comparator )
{
	_comparatorName = comparator;
	
	if ( comparator == "greater" ) {
		_comparator = []( crimild::Real32 a, crimild::Real32 b ) -> crimild::Bool { return a > b; };
	}
	else if ( comparator == "gequal" ) {
		_comparator = []( crimild::Real32 a, crimild::Real32 b ) -> crimild::Bool { return a >= b; };
	}
	else if ( comparator == "equal" ) {
		_comparator = []( crimild::Real32 a, crimild::Real32 b ) -> crimild::Bool { return a == b; };
	}
	else if ( comparator == "lower" ) {
		_comparator = []( crimild::Real32 a, crimild::Real32 b ) -> crimild::Bool { return a < b; };
	}
	else {
		// lequal
		_comparator = []( crimild::Real32 a, crimild::Real32 b ) -> crimild::Bool { return a <= b; };
	}
}

Behavior::State DistanceToTarget::step( BehaviorContext *context )
{
	auto agent = context->getAgent();

	if ( !context->hasTargets() ) {
		Log::warning( CRIMILD_CURRENT_CLASS_NAME, "No target defined for behavior" );
		return Behavior::State::FAILURE;
	}
	auto target = context->getTargetAt( 0 );

	const auto diff = Distance::computeSquared( target->getWorld().getTranslate(), agent->getWorld().getTranslate() );

	return ( _comparator( diff, ( _value * _value ) ) ? Behavior::State::SUCCESS : Behavior::State::FAILURE );
}

void DistanceToTarget::encode( coding::Encoder &encoder )
{
	Behavior::encode( encoder );

	encoder.encode( "value", _value );
	encoder.encode( "comparator", _comparatorName );
}

void DistanceToTarget::decode( coding::Decoder &decoder )
{
	Behavior::decode( decoder );

	decoder.decode( "value", _value );

	std::string comparator;
	decoder.decode( "comparator", comparator );
	setComparator( comparator );
}

