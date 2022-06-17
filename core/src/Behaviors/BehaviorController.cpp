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
    if ( getCurrentBehavior() != nullptr ) {
        execute( getCurrentBehavior(), true );
    }
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

void BehaviorController::execute( SharedPointer< Behavior > const &behavior, bool force ) noexcept
{
    if ( !force && getCurrentBehavior() == behavior ) {
        return;
    }

    setCurrentBehavior( behavior );

    if ( getNode() == nullptr ) {
        // If this component is not yet started, delay behavior initialization
        // until start() is called.
        return;
    }

    if ( behavior != nullptr ) {
        behavior->init( getContext() );
    }
}

void BehaviorController::encode( coding::Encoder &encoder )
{
    NodeComponent::encode( encoder );

    encoder.encode( "context", _context );
    encoder.encode( "currentBehavior", m_currentBehavior );
}

void BehaviorController::decode( coding::Decoder &decoder )
{
    NodeComponent::decode( decoder );

    decoder.decode( "context", _context );

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
    other->m_currentBehavior = m_currentBehavior;
    return other;
}
