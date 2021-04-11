#include "UISystem.hpp"

#include "Components/UIResponder.hpp"
#include "Simulation/Input.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/Apply.hpp"

using namespace crimild;
using namespace crimild::messaging;

void UISystem::start( void ) noexcept
{
    /*
    if ( !System::start() ) {
		return false;
	}

    // TODO: This should be a click, not just a mouse button up
    registerMessageHandler< MouseButtonUp >( []( MouseButtonUp const &msg ) {
        if ( msg.button == CRIMILD_INPUT_MOUSE_BUTTON_LEFT ) {
            Vector2f mousePos = Input::getInstance()->getNormalizedMousePosition();
			if ( Input::getInstance()->getMouseCursorMode() != Input::MouseCursorMode::NORMAL ) {
				mousePos = Vector2f( 0.5f, 0.5f );
			}

            auto scene = Simulation::getInstance()->getScene();
			if ( scene == nullptr ) {
				Log::error( CRIMILD_CURRENT_CLASS_NAME, "No valid scene" );
				return;
			}

            auto camera = Camera::getMainCamera();
			if ( camera == nullptr ) {
				Log::error( CRIMILD_CURRENT_CLASS_NAME, "Main camera is null" );
				return;
			}

            Ray3f ray;
            if ( camera->getPickRay( mousePos[ 0 ], mousePos[ 1 ], ray ) ) {

                float minJ = -1.0f;
                Node *result = nullptr;
                scene->perform( Apply( [&]( Node *node ) {
                    auto responder = node->getComponent< UIResponder >();
                    if ( responder == nullptr ) {
                        return;
                    }

                    if ( !responder->isEnabled() ) {
                        return ;
                    }

                    if ( responder->getBoundingVolume()->testIntersection( ray ) ) {
                        float j = Distance::computeSquared( node->getWorldBound()->getCenter(), camera->getWorld().getTranslate() );
                        if ( minJ < 0.0f || j < minJ ) {
                            minJ = j;
                            result = node;
                        }
                    }
                }));

                if ( result != nullptr ) {
                    result->getComponent< UIResponder >()->invoke();
                }

                // TODO: cancel message broadcasting
            }
        }
    });

	return true;
    */
}
