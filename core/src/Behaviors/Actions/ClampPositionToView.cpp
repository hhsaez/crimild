#include "Behaviors/Actions/ClampPositionToView.hpp"

#include "Mathematics/Matrix4_inverse.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Transformation_translation.hpp"
#include "Mathematics/clamp.hpp"
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

    const auto P = camera->getProjectionMatrix();
    const auto V = camera->getViewMatrix();
    const auto W = agent->getParent()->getWorld().mat;

    auto pos = P * V * W * vector4( location( agent->getLocal() ), 1 );
    pos.x = clamp( pos.x, -pos.w, pos.w );
    pos.y = clamp( pos.y, -pos.w, pos.w );

    const auto newPosition = inverse( W ) * inverse( V ) * inverse( P ) * pos;
    agent->setLocal( translation( newPosition.x, newPosition.y, newPosition.z ) );

    return Behavior::State::SUCCESS;
}
