#include "TestContextValue.hpp"
#include "Coding/Encoder.hpp"
#include "Coding/Decoder.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::conditions;

TestContextValue::TestContextValue( std::string key, std::string value, std::string comparator )
	: _key( key ),
	  _value( value )
{
	setComparator( comparator );
}

TestContextValue::~TestContextValue( void )
{

}

void TestContextValue::setComparator( std::string comparator )
{
	_comparatorName = comparator;
	
	if ( comparator == "greater" ) {
		_comparator = []( crimild::Real32 a, crimild::Real32 b ) -> crimild::Bool { return a > b; };
	}
	else if ( comparator == "gequal" ) {
		_comparator = []( crimild::Real32 a, crimild::Real32 b ) -> crimild::Bool { return a >= b; };
	}
	else if ( comparator == "lequal" ) {
		_comparator = []( crimild::Real32 a, crimild::Real32 b ) -> crimild::Bool { return a <= b; };
	}
	else if ( comparator == "lower" ) {
		_comparator = []( crimild::Real32 a, crimild::Real32 b ) -> crimild::Bool { return a < b; };
	}
	else {
		// no comparator uses std::string equality
		_comparator = nullptr;
	}
}

Behavior::State TestContextValue::step( BehaviorContext *context )
{
	if ( _comparator != nullptr ) {
		auto setting = context->getValue< crimild::Real32 >( _key );
		crimild::Real32 v;
		StringUtils::toValue( _value, v );
		return ( _comparator( setting, v ) ?  Behavior::State::SUCCESS : Behavior::State::FAILURE );
	}

	// no comparator case
	auto setting = context->getValue< std::string >( _key );
	return setting == _value ? Behavior::State::SUCCESS : Behavior::State::FAILURE;
}

void TestContextValue::encode( coding::Encoder &encoder )
{
	Behavior::encode( encoder );

	encoder.encode( "key", _key );
	encoder.encode( "value", _value );
	encoder.encode( "comparator", _comparatorName );
}

void TestContextValue::decode( coding::Decoder &decoder )
{
	Behavior::decode( decoder );

	decoder.decode( "key", _key );
	decoder.decode( "value", _value );

	std::string comparator;
	decoder.decode( "comparator", comparator );
	setComparator( comparator );
}

