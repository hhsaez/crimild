#include "Behaviors/Actions/ClampPositionToView.hpp"

#include "Crimild_Mathematics.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Node.hpp"
#include "Simulation/Simulation.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

void ClampPositionToView::init( BehaviorContext *context )
{
    Behavior::init( context );
}

Behavior::State ClampPositionToView::step( BehaviorContext *context )
{
    auto agent = context->getAgent();
    if ( agent == nullptr ) {
        return Behavior::State::FAILURE;
    }

    auto camera = Simulation::getInstance()->getMainCamera();
    if ( camera == nullptr ) {
        CRIMILD_LOG_WARNING( "No camera found" );
        return Behavior::State::FAILURE;
    }

    // local -> world
    // transform world position into screen position
    // clamp screen xy coordinates to [0, 1]
    // transform updated screen position to world
    // world -> local

    // TODO: Fix this
    /*
        const auto P = camera->getProjectionMatrix();
        const auto V = camera->getViewMatrix();
        const auto W = Matrix4f( agent->getParent()->getWorld() );

        auto pos = P * V * W * Vector4f( origin( agent->getLocal() ) );
        pos.x = clamp( pos.x, -pos.w, pos.w );
        pos.y = clamp( pos.y, -pos.w, pos.w );

        const auto newPosition = inverse( W ) * inverse( V ) * inverse( P ) * pos;
        agent->setLocal( translation( newPosition.x, newPosition.y, newPosition.z ) );
    */

    return Behavior::State::SUCCESS;
}
