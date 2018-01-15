#include "BehaviorController.hpp"

#include "Simulation/Simulation.hpp"
#include "Concurrency/Async.hpp"

using namespace crimild;
using namespace crimild::messaging;
using namespace crimild::behaviors;

constexpr const char *BehaviorController::DEFAULT_BEHAVIOR_NAME;
constexpr const char *BehaviorController::SCENE_STARTED_BEHAVIOR_NAME;
constexpr const char *BehaviorController::SETTINGS_EXECUTE_BEHAVIORS;

BehaviorController::BehaviorController( void )
{

}

BehaviorController::~BehaviorController( void )
{

}

void BehaviorController::onAttach( void )
{
	_context.setAgent( getNode() );
}

void BehaviorController::onDetach( void )
{

}

void BehaviorController::start( void )
{
	registerMessageHandler< BehaviorEvent >( [this]( BehaviorEvent const &m ) {
		/*
		// retain the behavior
		auto behavior = _behaviors[ m.name ];
		if ( behavior == nullptr || crimild::get_ptr( behavior ) == getCurrentBehavior() ) {
			// no behavior associated with trigger or already running
			return;
		}
			
		// wait for the next frame before switching behaviors
		crimild::concurrency::sync_frame( [this, behavior]() {
			setCurrentBehavior( crimild::get_ptr( behavior ) );
			if ( getCurrentBehavior() != nullptr ) {
				getCurrentBehavior()->init( &_context );
			}
		});
		*/
		auto eventName = m.name;
		crimild::concurrency::sync_frame( [this, eventName] {
			executeBehavior( eventName );
		});
	});

	// init the default behavior, if any
	executeBehavior( DEFAULT_BEHAVIOR_NAME );

	// attempt to execute the start behavior, if any
	executeBehavior( SCENE_STARTED_BEHAVIOR_NAME );
}

void BehaviorController::update( const Clock &c )
{
	if ( !Simulation::getInstance()->getSettings()->get< bool >( SETTINGS_EXECUTE_BEHAVIORS, true ) ) {
		return;
	}

	_context.update( c );

	if ( getCurrentBehavior() == nullptr ) {
		setCurrentBehavior( crimild::get_ptr( _behaviors[ DEFAULT_BEHAVIOR_NAME ] ) );
		_currentEvent = DEFAULT_BEHAVIOR_NAME;
	}

	if ( getCurrentBehavior() != nullptr ) {
		// TODO: what if the behavior finishes (either fails or succeedes)?
		auto result = getCurrentBehavior()->step( &_context );
		if ( result != Behavior::State::RUNNING ) {
			setCurrentBehavior( nullptr );
		}
	}
}

void BehaviorController::attachBehavior( std::string eventName, BehaviorPtr const &behavior )
{
	_behaviors[ eventName ] = behavior;
}

bool BehaviorController::executeBehavior( std::string eventName )
{
	if ( eventName == _currentEvent ) {
		// behavior already running
		return true;
	}
	
	auto behavior = _behaviors[ eventName ];
	if ( behavior == nullptr ) {
		//Log::warning( CRIMILD_CURRENT_CLASS_NAME, "No behavior found for event ", eventName );
		return false;
	}

	setCurrentBehavior( crimild::get_ptr( behavior ) );
	getCurrentBehavior()->init( &_context );
	
	_currentEvent = eventName;
	
	return true;
}

