#include "BehaviorController.hpp"

#include "Behavior.hpp"
#include "BehaviorContext.hpp"
#include "BehaviorTree.hpp"
#include "Behaviors/Actions/Success.hpp"
#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"
#include "Concurrency/Async.hpp"
#include "Simulation/Simulation.hpp"

using namespace crimild;
using namespace crimild::messaging;
using namespace crimild::behaviors;

constexpr const crimild::Char *BehaviorController::DEFAULT_BEHAVIOR_NAME;
constexpr const crimild::Char *BehaviorController::SCENE_STARTED_BEHAVIOR_NAME;
constexpr const crimild::Char *BehaviorController::SETTINGS_EXECUTE_BEHAVIORS;

BehaviorController::BehaviorController( void )
    : _context( crimild::alloc< BehaviorContext >() )
{
    attachBehavior( DEFAULT_BEHAVIOR_NAME, crimild::alloc< actions::Success >() );
    attachBehavior( SCENE_STARTED_BEHAVIOR_NAME, crimild::alloc< actions::Success >() );
}

void BehaviorController::onAttach( void )
{
    _context->setAgent( getNode() );
}

void BehaviorController::start( void )
{
    // registerMessageHandler< BehaviorEvent >( [ this ]( BehaviorEvent const &m ) {
    //     auto eventName = m.name;
    //     crimild::concurrency::sync_frame( [ this, eventName ] {
    //         executeBehaviorTree( eventName );
    //     } );
    // } );

    // // init the default behavior, if any
    // executeBehaviorTree( DEFAULT_BEHAVIOR_NAME );

    // // attempt to execute the start behavior, if any
    // executeBehaviorTree( SCENE_STARTED_BEHAVIOR_NAME );
}

void BehaviorController::update( const Clock &c )
{
    getContext()->update( c );

    if ( auto behavior = getCurrentBehavior() ) {
        auto result = behavior->step( getContext() );
        if ( result != Behavior::State::RUNNING ) {
            setCurrentBehavior( nullptr );
        }
    }

    // if ( getCurrentBehaviorTree() == nullptr ) {
    //     setCurrentBehaviorTree( crimild::get_ptr( _behaviors[ DEFAULT_BEHAVIOR_NAME ] ) );
    //     _currentEvent = DEFAULT_BEHAVIOR_NAME;
    // }

    // if ( auto tree = getCurrentBehaviorTree() ) {
    //     if ( auto behavior = tree->getRootBehavior() ) {
    //         // TODO: what if the behavior finishes (either fails or succeedes)?
    //         auto result = behavior->step( getContext() );
    //         if ( result != Behavior::State::RUNNING ) {
    //             setCurrentBehaviorTree( nullptr );
    //         }
    //     }
    // }
}

Behavior *BehaviorController::getBehavior( std::string_view eventName ) noexcept
{
    if ( m_behaviors.count( std::string( eventName ) ) == 0 ) {
        return nullptr;
    }
    return m_behaviors.at( std::string( eventName ) ).get();
}

void BehaviorController::attachBehavior( std::string_view eventName, SharedPointer< Behavior > const &behavior ) noexcept
{
    m_behaviors[ std::string( eventName ) ] = behavior;
}

void BehaviorController::attachBehaviorTree( SharedPointer< BehaviorTree > const &behaviorTree )
{
    _behaviors[ behaviorTree->getName() ] = behaviorTree;
}

void BehaviorController::attachBehaviorTree( std::string eventName, SharedPointer< BehaviorTree > const &behaviorTree )
{
    _behaviors[ eventName ] = behaviorTree;
}

void BehaviorController::execute( SharedPointer< Behavior > const &behavior ) noexcept
{
    if ( getCurrentBehavior() == behavior ) {
        return;
    }

    setCurrentBehavior( behavior );

    if ( behavior != nullptr ) {
        behavior->init( getContext() );
    }
}

bool BehaviorController::executeBehaviorTree( std::string eventName )
{
    if ( eventName == _currentEvent ) {
        // behavior already running
        return true;
    }

    auto tree = _behaviors[ eventName ];
    if ( tree == nullptr ) {
        // Log::warning( CRIMILD_CURRENT_CLASS_NAME, "No behavior found for event ", eventName );
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

    crimild::Array< SharedPointer< BehaviorTree > > behaviors;
    _behaviors.each( [ &behaviors ]( std::string, SharedPointer< BehaviorTree > &tree ) {
        if ( tree != nullptr ) {
            behaviors.add( tree );
        }
    } );
    encoder.encode( "behaviors", behaviors );

    encoder.encode( "currentBehavior", m_currentBehavior );
}

void BehaviorController::decode( coding::Decoder &decoder )
{
    NodeComponent::decode( decoder );

    decoder.decode( "context", _context );

    Array< SharedPointer< BehaviorTree > > behaviors;
    decoder.decode( "behaviors", behaviors );
    behaviors.each( [ this ]( SharedPointer< BehaviorTree > &tree ) {
        if ( tree != nullptr ) {
            attachBehaviorTree( tree );
        }
    } );

    // When decoding the current behavior, don't forget to actually
    // started by calling Behavior::init(). Otherwise, it will end
    // up in an undefined state. For simpliciy, I'm using
    // execute() here which takes care of that state.
    SharedPointer< Behavior > behavior;
    decoder.decode( "currentBehavior", behavior );
    execute( behavior );
}

SharedPointer< NodeComponent > BehaviorController::clone( void )
{
    auto other = crimild::alloc< BehaviorController >();
    other->_context = _context;
    other->_behaviors = _behaviors;
    other->m_currentBehavior = m_currentBehavior;
    return other;
}
