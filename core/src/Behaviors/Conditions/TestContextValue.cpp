#include "TestContextValue.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::conditions;

TestContextValue::TestContextValue( std::string key, std::string value, std::string comparator )
	: _key( key ),
	  _value( value )
{
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

TestContextValue::~TestContextValue( void )
{

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

