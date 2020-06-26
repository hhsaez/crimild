#include "BehaviorController.hpp"
#include "Behavior.hpp"
#include "BehaviorTree.hpp"
#include "BehaviorContext.hpp"

#include "Simulation/Simulation.hpp"
#include "Concurrency/Async.hpp"

using namespace crimild;
using namespace crimild::messaging;
using namespace crimild::behaviors;

constexpr const crimild::Char *BehaviorController::DEFAULT_BEHAVIOR_NAME;
constexpr const crimild::Char *BehaviorController::SCENE_STARTED_BEHAVIOR_NAME;
constexpr const crimild::Char *BehaviorController::SETTINGS_EXECUTE_BEHAVIORS;

BehaviorController::BehaviorController( void )
	: _context( crimild::alloc< BehaviorContext >() )
{

}

BehaviorController::~BehaviorController( void )
{

}

void BehaviorController::onAttach( void )
{
	_context->setAgent( getNode() );
}

void BehaviorController::onDetach( void )
{

}

void BehaviorController::start( void )
{
	registerMessageHandler< BehaviorEvent >( [ this ]( BehaviorEvent const &m ) {
		auto eventName = m.name;
		crimild::concurrency::sync_frame( [ this, eventName ] {
			executeBehaviorTree( eventName );
		});
	});

	// init the default behavior, if any
	executeBehaviorTree( DEFAULT_BEHAVIOR_NAME );

	// attempt to execute the start behavior, if any
	executeBehaviorTree( SCENE_STARTED_BEHAVIOR_NAME );
}

void BehaviorController::update( const Clock &c )
{
	if ( !Simulation::getInstance()->getSettings()->get< bool >( SETTINGS_EXECUTE_BEHAVIORS, true ) ) {
		return;
	}

	_context->update( c );

	if ( getCurrentBehaviorTree() == nullptr ) {
		setCurrentBehaviorTree( crimild::get_ptr( _behaviors[ DEFAULT_BEHAVIOR_NAME ] ) );
		_currentEvent = DEFAULT_BEHAVIOR_NAME;
	}

	if ( auto tree = getCurrentBehaviorTree() ) {
		if ( auto behavior = tree->getRootBehavior() ) {
			// TODO: what if the behavior finishes (either fails or succeedes)?
			auto result = behavior->step( getContext() );
			if ( result != Behavior::State::RUNNING ) {
				setCurrentBehaviorTree( nullptr );
			}
		}
	}
}

void BehaviorController::attachBehaviorTree( SharedPointer< BehaviorTree > const &behaviorTree )
{
	_behaviors[ behaviorTree->getName() ] = behaviorTree;
}

void BehaviorController::attachBehaviorTree( std::string eventName, SharedPointer< BehaviorTree > const &behaviorTree )
{
	_behaviors[ eventName ] = behaviorTree;
}

bool BehaviorController::executeBehaviorTree( std::string eventName )
{
	if ( eventName == _currentEvent ) {
		// behavior already running
		return true;
	}
	
	auto tree = _behaviors[ eventName ];
	if ( tree == nullptr ) {
		//Log::warning( CRIMILD_CURRENT_CLASS_NAME, "No behavior found for event ", eventName );
		return false;
	}

	setCurrentBehaviorTree( crimild::get_ptr( tree ) );

	if ( auto behavior = tree->getRootBehavior() ) {
		behavior->init( getContext() );
	}
	
	_currentEvent = eventName;
	
	return true;
}

void BehaviorController::encode( coding::Encoder &encoder )
{
	NodeComponent::encode( encoder );

	encoder.encode( "context", _context );

	crimild::Array< SharedPointer< BehaviorTree >> behaviors;
	_behaviors.each( [ &behaviors ]( std::string, SharedPointer< BehaviorTree > &tree ) {
		behaviors.add( tree );
	});
	encoder.encode( "behaviors", behaviors );	
}

void BehaviorController::decode( coding::Decoder &decoder)
{
	NodeComponent::decode( decoder );

	decoder.decode( "context", _context );

	Array< SharedPointer< BehaviorTree >> behaviors;
	decoder.decode( "behaviors", behaviors );	
	behaviors.each( [ this ]( SharedPointer< BehaviorTree > &tree ) {
		attachBehaviorTree( tree );
	});
}

